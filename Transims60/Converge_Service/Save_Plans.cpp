//*********************************************************
//	Save_Plans.cpp -  write the path building results
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Save_Plans
//---------------------------------------------------------

bool Converge_Service::Save_Plans (Plan_Ptr_Array *array_ptr, int part)
{
	if (array_ptr == 0) return (false);

	bool keep_new, copy_flag, cap_flag;
	Dtime time1, time2, time_diff;
	int cost1, cost2, cost_diff, priority;
	double prob, ran;

	Plan_Ptr_Itr itr;
	Plan_Ptr plan_ptr, new_ptr;

	//---- process each plan in the array ----

	for (itr = array_ptr->begin (); itr != array_ptr->end (); itr++) {
		new_ptr = *itr;
		if (new_ptr == 0) continue;

		if (new_ptr->Method () == RESKIM_PLAN) {
			if (save_trip_gap) {
				Gap_Data *gap_ptr;

				gap_ptr = &gap_data_array [new_ptr->Index ()];
				gap_ptr->current = new_ptr->Impedance ();
				gap_ptr->time = (new_ptr->Constraint () == END_TIME) ? new_ptr->End (): new_ptr->Start ();
			}
MAIN_LOCK
			num_reskim++;
END_LOCK
			continue;
		}
		if (new_ptr->Problem () == 0 && rider_flag) {
			part_processor.Sum_Ridership (*new_ptr, part);
		}

MAIN_LOCK
		total_records++;
		copy_flag = false;

		if (new_ptr->Problem () > 0) {
			Set_Problem ((Problem_Type) new_ptr->Problem ());
		} else if (new_ptr->Method () == REROUTE_PATH) {
			num_reroute++;
		} else if (new_ptr->Method () == UPDATE_PLAN) {
			num_update++;
		} else if (new_ptr->Method () == COPY_PLAN || new_ptr->Method () == EXTEND_COPY) {
			copy_flag = true;
			num_copied++;
		} else {
			num_build++;
		}
END_LOCK

		//---- memory-based processing ----

		if (plan_memory_flag) {

			//---- select the best plan ----
					
			plan_ptr = &plan_array [new_ptr->Index ()];

			keep_new = (plan_ptr->Household () <= 0 || plan_ptr->size () == 0 || plan_ptr->Path_Problem ());
			time1 = 0;
			priority = CRITICAL;

			if (plan_ptr->Priority () == SKIP) {
				if (!plan_ptr->Path_Problem () && new_ptr->Path_Problem ()) {
					priority = HIGH;
					goto select_plans;
				} else {
					priority = NO_PRIORITY;
					keep_new = true;
				}
			} else if (copy_flag) {
				time1 = new_ptr->Activity ();
				new_ptr->Activity (new_ptr->Duration ());

				if (!time_diff_flag && !trip_diff_flag) {
					priority = HIGH;
					goto select_plans;
				}
			}

			if (new_ptr->Problem () == 0) {

				//---- select the plan to keep ----

				if (!keep_new && plan_ptr->Impedance () > 0) {
					priority = NO_PRIORITY;	

					//---- compare plan times ----

					if (time_diff_flag) {
						if (!copy_flag) {
							time1 = new_ptr->Arrive () - new_ptr->Depart ();
						}
						time2 = plan_ptr->Arrive () - plan_ptr->Depart ();

						time_diff = time1 - time2;

						if (time_diff > max_time_diff) {
							priority = CRITICAL;
							keep_new = true;
						} else if (time_diff >= min_time_diff && time2 > 0) {
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

					//---- compare plan costs ----

					if (cost_diff_flag) {
						cost1 = new_ptr->Impedance ();
						cost2 = plan_ptr->Impedance ();

						cost_diff = cost1 - cost2;

						if (cost_diff > max_cost_diff) {
							priority = CRITICAL;
							keep_new = true;
						} else if (cost_diff >= min_cost_diff && cost2 > 0) {
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

					//---- selection difference ----

					if (!keep_new) {
						ran = random.Probability (new_ptr->Household () + iteration);
						prob = 1.0 - ((double) new_ptr->Impedance () / (new_ptr->Impedance () + plan_ptr->Impedance ()));
						keep_new = (ran < prob);
						priority = MAX ((int) (CRITICAL * fabs (prob - 0.5) * 2.0 + 0.5), priority);
					}
					if (priority > CRITICAL) priority = CRITICAL;
				}

				//---- compare trip times ----

				if (trip_diff_flag && (!keep_new || priority < HIGH)) {
					if (!copy_flag) {
						time1 = new_ptr->Arrive () - new_ptr->Depart ();
					}
					time2 = new_ptr->End () - new_ptr->Start ();

					time_diff = time1 - time2;

					if (time_diff > max_trip_diff) {
						priority = CRITICAL;
						keep_new = true;
					} else if (time_diff >= min_trip_diff && time2 > 0) {
						prob = (double) time_diff / time2;
						if (prob >= percent_trip_diff) {
							if (max_min_trip_diff > 1) {
								time_diff = time_diff - min_trip_diff;
								priority = CRITICAL * time_diff / max_min_trip_diff + MEDIUM;
								if (priority > CRITICAL) priority = CRITICAL;
							} else {
								priority = CRITICAL;
							}
							keep_new = true;
						}
					}
				} else {
					keep_new = true;
				}
			}

			if (keep_new) {
				plan_ptr->clear ();
				*plan_ptr = *new_ptr;
			}
select_plans:
			if (plan_ptr->Priority () != SKIP) {
				if (iteration > 1 && plan_ptr->Priority () == CRITICAL && priority == CRITICAL) {

					//---- avoid rerouting the same plan twice in a row ----

					plan_ptr->Priority (MEDIUM);
				} else {
					plan_ptr->Priority (priority);
				}
				if (select_priorities && select_priority [plan_ptr->Priority ()]) {
MAIN_LOCK
					select_records++;
					select_weight += plan_ptr->Priority ();
END_LOCK
				}
			}

			//---- process completed plans ----

			if (plan_ptr->Problem () == 0) {

				//---- calculate the trip gap ----

				if (save_trip_gap || trip_gap_map_flag) {
					Gap_Data *gap_ptr;

					gap_ptr = &gap_data_array [plan_ptr->Index ()];
					gap_ptr->current = (int) plan_ptr->Impedance ();

					if (copy_flag) {
						time2 = plan_ptr->Arrive () - plan_ptr->Depart ();
						if (time2 > 0) {
							gap_ptr->current = (int) ((double) gap_ptr->current * time1 / time2 + 0.5);
						}
					}
					if (plan_ptr->Constraint () == END_TIME) {
						gap_ptr->time = plan_ptr->End ();
					} else {
						gap_ptr->time = plan_ptr->Start ();
					}
				}

				//---- check for a capacity constraint ----

				if (capacity_flag) {
					cap_flag = Capacity_Check (*plan_ptr);
				} else {
					cap_flag = false;
				}

				//---- check the fuel supply ----

				if (fuel_flag && !cap_flag) {
					Fuel_Check (*plan_ptr);
				}

			}

		} else {	//---- file-based processing ----

			if (new_ptr->Problem () == 0) {
				if (new_plan_flag) {
					if (new_ptr->External_IDs ()) {
						if (new_set_flag) {
							new_file_set [new_ptr->Partition ()]->Write_Plan (*new_ptr);
						} else {
							new_plan_file->Write_Plan (*new_ptr);
						}
					}
				}

				//if (save_trip_gap || trip_gap_map_flag) {
				if (Trip_Gap_Map_Parts ()) {
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

					trip_gap_map_ptr = trip_gap_map_array [new_ptr->Partition ()];

					map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (new_ptr->Get_Trip_Index (), gap_data));

					if (!map_stat.second) {
						map_stat.first->second.current = (int) new_ptr->Impedance ();
					}
				}
				if (save_plan_flag && iteration < max_iteration && save_iter_range.In_Range (iteration)) {
					if (save_hhold_range.In_Range (new_ptr->Household ())) {
						if (!new_plan_flag) {
							if (!new_ptr->External_IDs ()) {
								delete new_ptr;
								continue;
							}
						}
						if (!save_plan_file.Part_Flag ()) {
							new_ptr->Person (iteration);
						}
						save_plan_file.Write_Plan (*new_ptr);
					}
				}
			} else if (problem_flag) {
				if (new_ptr->External_IDs ()) {
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
		}
		delete new_ptr;
	}
	delete array_ptr;
	return (true);
}
