//*********************************************************
//	Read_Trips.cpp - Read the Trip File
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

bool Router::Read_Trips (int part, Plan_Processor *plan_process_ptr)
{
	int p, p0, num, num_car, max_hhold, hhold, person, mode;
	int last_hhold, last_person, partition;

	bool keep_flag, old_flag, duration_flag, last_skip, gap_flag, gap_ptr_flag, first;

	Trip_File *file = 0;
	Plan_File *plan_file = 0;
	Plan_Ptr_Array *ptr_array;
	Plan_Data *new_ptr, *plan_ptr;
	Path_Parameters param;
	Select_Map_Itr sel_itr;
	Trip_Index trip_index, old_trip_index;
	Gap_Data gap_data;
	Trip_Gap_Map_Stat map_stat;
	Trip_Gap_Map *trip_gap_map_ptr;
	Trip_Itr trip_itr;
	Trip_Data trip_rec;
	Plan_Itr plan_itr;
	
	Set_Parameters (param);

	num_car = 0;
	last_hhold = last_person = -1;
	max_hhold = MAX_INTEGER;
	new_ptr = plan_ptr = 0;
	old_flag = duration_flag = last_skip = false;

	gap_flag = (trip_gap_map_flag && first_iteration);
	gap_ptr_flag = false;
	trip_gap_map_ptr = &trip_gap_map;

	if (gap_flag) {
		if (thread_flag) {
			trip_gap_map_ptr = trip_gap_map_array [part];
		} else if (part_processor.Thread_Flag ()) {
			gap_ptr_flag = true;
		}
	}
	partition = part;

	num = 1;
	ptr_array = new Plan_Ptr_Array ();
	new_ptr = new Plan_Data ();

	if (!trip_memory_flag) {
		file = (trip_set_flag) ? trip_file_set [part] : trip_file;
	}
	if (trip_set_flag) {
		p0 = part;
		num = part + 1;
	} else {
		p0 = 0;
		num = num_file_sets;
	}

	if (plan_flag) {
		if (plan_memory_flag) {
			if (!trip_memory_flag) {
				plan_ptr = new Plan_Data ();
				plan_itr = plan_array.begin ();
				if (plan_itr == plan_array.end ()) {
					plan_ptr->Household (0);
				} else {
					*plan_ptr = *plan_itr;
				}
			}
		} else {
			if (new_set_flag) {
				for (p=p0; p < num; p++) {
					plan_file = plan_file_set [p];

					if (plan_set_flag) {
						plan_ptr_arrays [p] = plan_ptr = new Plan_Data ();
					} else {
						plan_ptr = new Plan_Data ();
					}
					for (;;) {
						if (!plan_file->Read_Plan (*plan_ptr)) {
							plan_ptr->Household (0);
						} else {
							plan_ptr->Partition (p);
							if (!Selection (plan_ptr)) continue;
							plan_ptr->Internal_IDs ();
						}
						break;
					}
				}
			} else {
				plan_ptr = new Plan_Data ();
				plan_file = Router::plan_file;

				for (;;) {
					if (!plan_file->Read_Plan (*plan_ptr)) {
						plan_ptr->Household (0);
					} else {
						if (!Selection (plan_ptr)) continue;
						plan_ptr->Internal_IDs ();
					}
					break;
				}
			}
		}
	}
	if (select_households) {
		max_hhold = hhold_range.Max_Value ();
	}
	if (select_flag) {
		hhold = select_map.Max_Household ();
		if (hhold < max_hhold) max_hhold = hhold;
	}

	if (Master ()) {
		if (trip_memory_flag) {
			Show_Message ("Processing Trip Record");
			Set_Progress ();
		} else {
			if (thread_flag) {
				MAIN_LOCK
				if (trip_set_flag) {
					Show_Message (String ("Reading %s %d") % file->File_Type () % file->Part_Number ());
				} else {
					Show_Message (String ("Reading %s") % file->File_Type ());
				}
				END_LOCK
			} else {
				if (trip_set_flag) {
					Show_Message (0, String ("\tReading %s %d -- Trip") % file->File_Type () % file->Part_Number ());
				} else {
					Show_Message (String ("Reading %s -- Trip") % file->File_Type ());
				}
				Set_Progress ();
			}
		}
	}
	last_hhold = last_person = 0;

	for (first=true; ; first=false) {
		if (Master ()) {
			if (thread_flag) {
				Show_Dot ();
			} else {
				Show_Progress ();
			}
		}
		if (trip_memory_flag) {
			if (first) {
				trip_itr = trip_array.begin ();
			} else {
				trip_itr++;
			}
			if (trip_itr == trip_array.end ()) break;
			*new_ptr = *trip_itr;
		} else {
			if (!file->Read_Trip (trip_rec)) break;

			if (priority_flag) {
				trip_rec.Priority (initial_priority);
			}
			if (trip_rec.Household () < 1 || !Selection (&trip_rec)) continue;

			if (!trip_rec.Internal_IDs ()) continue;
			*new_ptr = trip_rec;

			new_ptr->Depart (new_ptr->Start ());
			new_ptr->Arrive (new_ptr->End ());
			new_ptr->Activity (new_ptr->Duration ());
		}

		//---- check the household id ----

		hhold = new_ptr->Household ();
		if (hhold < 1) continue;

		if (hhold > max_hhold) {
			if (last_hhold > 0 && ptr_array->size () > 0) {
				part_processor.Plan_Build (ptr_array, partition, plan_process_ptr);
				ptr_array = new Plan_Ptr_Array ();
			}
			last_hhold = 0;
			break;
		}
		person = new_ptr->Person ();

		//---- update the priority flag ----

		if (!first_iteration && select_priorities && plan_memory_flag) {
			new_ptr->Priority (plan_array [new_ptr->Index ()].Priority ());
		}
		keep_flag = (!select_priorities || select_priority [new_ptr->Priority ()]);
		old_flag = false;

		part = new_ptr->Partition ();

		//---- plan processing ----

		if (plan_flag) {
			new_ptr->Get_Index (trip_index);

			if (plan_memory_flag) {
				if (trip_memory_flag) {
					plan_ptr = &plan_array [new_ptr->Index ()];
				} else {
					plan_ptr->Get_Index (old_trip_index);
				}
			} else {
				if (plan_set_flag && !plan_memory_flag) {
					plan_file = plan_file_set [part];
					plan_ptr = plan_ptr_arrays [part];
				}
				plan_ptr->Get_Index (old_trip_index);
			}

			//---- process plans up to the current household ----

			while (plan_ptr->Household () > 0 && old_trip_index < trip_index) {

				//---- check the selection records ----

				//if (!Selection (plan_ptr)) goto next;

				//---- process plans from the previous household ----

				if (plan_ptr->Household () != last_hhold) {
					if (last_hhold > 0 && ptr_array->size () > 0) {
						part_processor.Plan_Build (ptr_array, partition, plan_process_ptr);
						ptr_array = new Plan_Ptr_Array ();
					}
					last_hhold = plan_ptr->Household ();
					last_person = plan_ptr->Person ();
					partition = part;
				}

				//---- add to the trip gap map ----

				if (gap_flag) {
					if (plan_ptr->Constraint () == END_TIME) {
						gap_data.time = plan_ptr->End ();
					} else {
						gap_data.time = plan_ptr->Start ();
					}
					gap_data.current = 0;
					gap_data.previous = (int) plan_ptr->Impedance ();

					if (gap_ptr_flag) {
						trip_gap_map_ptr = trip_gap_map_array [part];
					}
					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (plan_ptr->Get_Trip_Index (), gap_data));

					if (!map_stat.second) {
						map_stat.first->second.previous = (int) plan_ptr->Impedance ();
					}
				}	

				//---- save the old plan to the household array ----

				if (update_flag) {
					plan_ptr->Method (UPDATE_PLAN);
				} else if (Link_Flows ()) {
					plan_ptr->Method (PATH_FLOWS);
				} else {
					plan_ptr->Method (COPY_PLAN);
				}
				ptr_array->push_back (plan_ptr);

				//---- read the next plan ----

				plan_ptr = new Plan_Data ();
//next:
				if (plan_memory_flag) {
					if (plan_itr != plan_array.end ()) {
						if (++plan_itr == plan_array.end ()) {
							plan_ptr->Household (0);
						} else {
							*plan_ptr = *plan_itr;
						}
					} else {
						plan_ptr->Household (0);
					}
				} else {
					for (;;) {
						if (!plan_file->Read_Plan (*plan_ptr)) {
							plan_ptr->Household (0);
						} else {
							plan_ptr->Partition (part);
							if (!Selection (plan_ptr)) continue;
							plan_ptr->Internal_IDs ();
						}
						break;
					}
					if (plan_ptr->Household () == 0) break;
				}
				plan_ptr->Get_Index (old_trip_index);
			}
			if (keep_flag && old_trip_index == trip_index) {

				//---- add to the trip gap map ----

				if (gap_flag) {
					if (plan_ptr->Constraint () == END_TIME) {
						gap_data.time = plan_ptr->End ();
					} else {
						gap_data.time = plan_ptr->Start ();
					}
					gap_data.current = 0;
					gap_data.previous = (int) plan_ptr->Impedance ();

					if (gap_ptr_flag) {
						trip_gap_map_ptr = trip_gap_map_array [part];
					}
					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (plan_ptr->Get_Trip_Index (), gap_data));

					if (!map_stat.second) {
						map_stat.first->second.previous = (int) plan_ptr->Impedance ();
					}
				}

				//---- read the next plan ----

				if (reroute_flag && plan_ptr->Depart () < reroute_time && 
					plan_ptr->Arrive () > reroute_time) {

					//---- update the destination ----

					plan_ptr->End (new_ptr->End ());
					plan_ptr->Destination (new_ptr->Destination ());
					plan_ptr->Type (new_ptr->Type ());

					*new_ptr = *plan_ptr;
					plan_ptr = new Plan_Data ();
					old_flag = true;
				}
				if (plan_memory_flag) {
					if (plan_itr != plan_array.end ()) {
						if (++plan_itr == plan_array.end ()) {
							plan_ptr->Household (0);
						} else {
							*plan_ptr = *plan_itr;
						}
					} else {
						plan_ptr->Household (0);
					}
				} else {
					for (;;) {
						if (!plan_file->Read_Plan (*plan_ptr)) {
							plan_ptr->Household (0);
						} else {
							plan_ptr->Partition (part);
							if (!Selection (plan_ptr)) continue;
							plan_ptr->Internal_IDs ();
						}
						break;
					}
				}
			}
			if (plan_set_flag && !plan_memory_flag) {
				plan_ptr_arrays [part] = plan_ptr;
			}
		}

		if (!keep_flag) {
			continue;
		}
		if (hhold != last_hhold) {
			if (last_hhold > 0 && ptr_array->size () > 0) {
				part_processor.Plan_Build (ptr_array, partition, plan_process_ptr);
				ptr_array = new Plan_Ptr_Array ();
			}
			last_hhold = hhold;
			last_person = person;
			partition = part;
		} else if (person != last_person) {
			last_person = person;
		}

		//---- reroute the existing plan ----

		if (old_flag) {

			if (new_ptr->Depart () >= reroute_time) {
				new_ptr->Method (BUILD_PATH);
				new_ptr->Depart (new_ptr->Start ());
				new_ptr->Arrive (new_ptr->End ());
				new_ptr->Activity (new_ptr->Duration ());

			} else if (new_ptr->Arrive () < reroute_time) {
				//if (Link_Flows ()) {
				//	new_ptr->Method (PATH_FLOWS);
				//} else {
					new_ptr->Method (COPY_PLAN);
				//}
			} else {
				new_ptr->Method (REROUTE_PATH);
				new_ptr->Arrive (reroute_time);
			}
		} else {
			
			//---- initialize the plan data ----
					
			new_ptr->Method (BUILD_PATH);
			new_ptr->Depart (new_ptr->Start ());
			new_ptr->Arrive (new_ptr->End ());
			new_ptr->Activity (new_ptr->Duration ());
		}
		ptr_array->push_back (new_ptr);
		mode = new_ptr->Mode ();

		new_ptr = new Plan_Data ();

		//---- update the link travel times ----

		if (Time_Updates () && update_rate > 0) {
			if (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
				mode != TRANSIT_MODE && mode != RIDE_MODE && mode != OTHER_MODE) {

				if (!(++num_car % update_rate)) {
					plan_process_ptr->Save_Flows ();
					Update_Travel_Times (MPI_Size (), reroute_time);
					num_time_updates++;
				}
			}
		}
	}
	if (Master () && !thread_flag) End_Progress ();
	if (!trip_memory_flag) file->Close ();
	delete new_ptr;

	//---- copy remaining plans ----

	if (plan_flag) {
		for (p=p0; p < num; p++) {
			if (plan_memory_flag) {
				if (plan_itr != plan_array.end ()) {
					if (++plan_itr == plan_array.end ()) {
						plan_ptr->Household (0);
					} else {
						*plan_ptr = *plan_itr;
					}
				} else {
					plan_ptr->Household (0);
				}
			} else if (plan_set_flag) {
				plan_file = plan_file_set [p];
				plan_ptr = plan_ptr_arrays [p];
			}
			while (plan_ptr->Household () > 0) {
				if (plan_ptr->Household () > max_hhold) break;
				//if (!Selection (plan_ptr)) continue;

				//---- process plans from the previous household ----

				if (plan_ptr->Household () != last_hhold) {
					if (last_hhold > 0 && ptr_array->size () > 0) {
						part_processor.Plan_Build (ptr_array, partition, plan_process_ptr);
						ptr_array = new Plan_Ptr_Array ();
					}
					last_hhold = plan_ptr->Household ();
					last_person = plan_ptr->Person ();
					partition = p;
				}

				//---- add to the trip gap map ----

				if (gap_flag) {
					if (plan_ptr->Constraint () == END_TIME) {
						gap_data.time = plan_ptr->End ();
					} else {
						gap_data.time = plan_ptr->Start ();
					}
					gap_data.current = 0;
					gap_data.previous = (int) plan_ptr->Impedance ();

					if (gap_ptr_flag) {
						trip_gap_map_ptr = trip_gap_map_array [p];
					}
					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (plan_ptr->Get_Trip_Index (), gap_data));

					if (!map_stat.second) {
						map_stat.first->second.previous = (int) plan_ptr->Impedance ();
					}
				}

				//---- save the old plan to the household array ----

				if (reroute_flag) {
					if (plan_ptr->Depart () < reroute_time && plan_ptr->Arrive () > reroute_time) {
						plan_ptr->Method (REROUTE_PATH);
						plan_ptr->Arrive (reroute_time);
					} else if (plan_ptr->Depart () >= reroute_time) {
						plan_ptr->Method (BUILD_PATH);
					} else if (plan_ptr->Arrive () < reroute_time) {
						//if (Link_Flows ()) {
						//	plan_ptr->Method (PATH_FLOWS);
						//} else {
							plan_ptr->Method (COPY_PLAN);
						//}
					}
				} else if (update_flag) {
					plan_ptr->Method (UPDATE_PLAN);
				} else if (Link_Flows ()) {
					plan_ptr->Method (PATH_FLOWS);
				} else {
					plan_ptr->Method (COPY_PLAN);
				}
				ptr_array->push_back (plan_ptr);

				//---- read the next plan ----

				plan_ptr = new Plan_Data ();

				if (plan_memory_flag) {
					if (plan_itr != plan_array.end ()) {
						if (++plan_itr == plan_array.end ()) {
							plan_ptr->Household (0);
						} else {
							*plan_ptr = *plan_itr;
						}
					} else {
						plan_ptr->Household (0);
					}
				} else {
					for (;;) {
						if (!plan_file->Read_Plan (*plan_ptr)) {
							plan_ptr->Household (0);
						} else {
							if (!Selection (plan_ptr)) continue;
							plan_ptr->Internal_IDs ();
						}
						break;
					}
				}
			}
			if (last_hhold > 0 && ptr_array->size () > 0) {
				part_processor.Plan_Build (ptr_array, partition, plan_process_ptr);
				ptr_array = new Plan_Ptr_Array ();
			}
			if (plan_set_flag && !plan_memory_flag) {
				delete plan_ptr;
			}
		}
		if (!plan_set_flag) {
			delete plan_ptr;
		}
	}
	if (last_hhold > 0 && ptr_array->size () > 0) {
		part_processor.Plan_Build (ptr_array, partition, plan_process_ptr);
	} else {
		delete ptr_array;
	}
	return (true);
}
