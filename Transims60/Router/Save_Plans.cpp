//*********************************************************
//	Save_Plans.cpp -  write the path building results
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Save_Plans
//---------------------------------------------------------

bool Router::Save_Plans (Plan_Ptr_Array *array_ptr, int part)
{
	if (array_ptr == 0) return (false);

	bool keep_new;
	Dtime time1, time2, time_diff;
	int cost1, cost2, cost_diff, priority;
	double prob, ran;

	Plan_Ptr_Itr itr;
	Plan_Ptr plan_ptr, new_ptr;
	Random random;

	//---- process each plan in the array ----

	for (itr = array_ptr->begin (); itr != array_ptr->end (); itr++) {
		new_ptr = *itr;
		if (new_ptr == 0) continue;
			
		if (new_ptr->Method () == RESKIM_PLAN) continue;
		total_records++;

		if (new_ptr->Problem () != 0) {
			Set_Problem ((Problem_Type) new_ptr->Problem ());
		} else {
			if (rider_flag) {
				part_processor.Sum_Ridership (*new_ptr, part);
			}			
			if (new_ptr->Method () == REROUTE_PATH) {
				num_reroute++;
			} else if (new_ptr->Method () == UPDATE_PLAN) {
				num_reskim++;
			}
		}

		//---- memory-based processing ----

		if (plan_memory_flag) {

			//---- select the best plan ----
					
			plan_ptr = &plan_array [new_ptr->Index ()];

			keep_new = (plan_ptr->Household () <= 0 || plan_ptr->size () == 0 || plan_ptr->Problem () > 0);
			priority = CRITICAL;

			if (!keep_new && new_ptr->Problem () == 0) {

				//---- select the plan to keep ----

				if (plan_ptr->Impedance () > 0) {
					priority = LOW;

					//---- compare plan times ----

					if (time_diff_flag) {
						time1 = new_ptr->Arrive () - new_ptr->Depart ();
						time2 = plan_ptr->Arrive () - plan_ptr->Depart ();

						time_diff = abs (time1 - time2);
						if (time_diff >= min_time_diff) {
							if (time_diff < max_time_diff && time2 > 0) {
								prob = (double) time_diff / time2;
								if (prob >= percent_time_diff) {
									if (max_min_time_diff > 1) {
										time_diff = time_diff - min_time_diff;
										priority = CRITICAL * time_diff / max_min_time_diff + MEDIUM;
									} else {
										priority = CRITICAL;
									}
									keep_new = true;
								}
							}
						}
					}

					//---- compare plan costs ----

					if (cost_diff_flag) {
						cost1 = new_ptr->Impedance ();
						cost2 = plan_ptr->Impedance ();

						cost_diff = abs (cost1 - cost2);
						if (cost_diff >= min_cost_diff) {
							if (cost_diff < max_cost_diff && cost2 > 0) {
								prob = (double) cost_diff / cost2;
								if (prob >= percent_cost_diff) {
									if (max_min_cost_diff > 1) {
										cost_diff = cost_diff - min_cost_diff;
										priority = MAX ((CRITICAL * cost_diff / max_min_cost_diff + MEDIUM), priority);
									} else {
										priority = CRITICAL;
									}
									keep_new = true;
								}
							}
						}
					}

					//---- selection difference ----

					if (!keep_new) {
						ran = random.Probability (new_ptr->Household () + iteration);
						prob = 1.0 - ((double) new_ptr->Impedance () / (new_ptr->Impedance () + plan_ptr->Impedance ()));
						keep_new = (ran < prob);
						priority = MAX ((int) (CRITICAL * fabs (prob - 0.5) * 2.0 + 0.5), priority);
					}
					if (priority > CRITICAL) priority = CRITICAL;
				} else {
					keep_new = true;
				}
			}
			if (keep_new) {
				plan_ptr->clear ();
				*plan_ptr = *new_ptr;

				plan_ptr->Priority (priority);
			}

			if (save_trip_gap || trip_gap_map_flag) {
				Gap_Data *gap_ptr;

				gap_ptr = &gap_data_array [plan_ptr->Index ()];
				gap_ptr->current = (int) plan_ptr->Impedance ();

				if (plan_ptr->Constraint () == END_TIME) {
					gap_ptr->time = plan_ptr->End ();
				} else {
					gap_ptr->time = plan_ptr->Start ();
				}
			}

		} else {	//---- file-based processing ----

			if (new_ptr->Problem () == 0) {
				if (new_plan_flag) {
					new_ptr->External_IDs ();

					if (new_set_flag) {
						new_file_set [new_ptr->Partition ()]->Write_Plan (*new_ptr);
					} else {
						new_plan_file->Write_Plan (*new_ptr);
					}
				}

				if (save_trip_gap || trip_gap_map_flag) {
					Gap_Data gap_data;
					Trip_Gap_Map_Stat map_stat;
					Trip_Gap_Map *trip_gap_map_ptr;

					if (new_ptr->Constraint () == END_TIME) {
						gap_data.time = new_ptr->End ();
					} else {
						gap_data.time = new_ptr->Start ();
					}
					gap_data.current = (int) new_ptr->Impedance ();
					gap_data.previous = 0;

					if (part_processor.Thread_Flag ()) {
						trip_gap_map_ptr = trip_gap_map_array [new_ptr->Partition ()];
					} else {
						trip_gap_map_ptr = &trip_gap_map;
					}
					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (new_ptr->Get_Trip_Index (), gap_data));

					if (!map_stat.second) {
						map_stat.first->second.current = (int) new_ptr->Impedance ();
					}
				}

			} else if (problem_flag) {
				new_ptr->External_IDs ();

				if (problem_set_flag) {
					Write_Problem (problem_set [new_ptr->Partition ()], new_ptr);
				} else if (thread_flag) {
					problem_file->Lock ();
					Write_Problem (problem_file, new_ptr);
					problem_file->UnLock ();
				} else {
					Write_Problem (problem_file, new_ptr);
				}
			}
		}
		delete new_ptr;
	}
	delete array_ptr;
	return (true);
}

//---------------------------------------------------------
//	Write_Problem
//---------------------------------------------------------

void Router::Write_Problem (Problem_File *file, Plan_Data *plan_ptr)
{
	file->Problem (plan_ptr->Problem ());
	file->Household (plan_ptr->Household ());
	file->Person (plan_ptr->Person ());
	file->Tour (plan_ptr->Tour ());
	file->Trip (plan_ptr->Trip ());
	file->Start (plan_ptr->Start ());
	file->End (plan_ptr->End ());
	file->Duration (plan_ptr->Duration ());
	file->Origin (plan_ptr->Origin ());
	file->Destination (plan_ptr->Destination ());
	file->Purpose (plan_ptr->Purpose ());
	file->Mode (plan_ptr->Mode ());
	file->Constraint (plan_ptr->Constraint ());
	file->Priority (plan_ptr->Priority ());
	file->Veh_Type (plan_ptr->Veh_Type ());
	file->Vehicle (plan_ptr->Vehicle ());
	file->Type (plan_ptr->Type ());
	file->Notes ((char *) Problem_Code ((Problem_Type) plan_ptr->Problem ()));

	if (!file->Write ()) {
		Warning ("Writing ") << file->File_Type ();
		problem_flag = false;
	}
	file->Add_Trip (plan_ptr->Household (), plan_ptr->Person (), plan_ptr->Tour ());
}
