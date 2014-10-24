//*********************************************************
//	Save_Plans.cpp -  write the path building results
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	Save_Plans
//---------------------------------------------------------

bool Simulator::Save_Plans (Plan_Ptr_Array *array_ptr, int part)
{
	if (array_ptr == 0) return (false);

	bool keep_new;
	Dtime time1, time2, time_diff;
	int cost1, cost2, cost_diff, priority;
	double prob, ran;

	Plan_Ptr_Itr itr;
	Plan_Ptr plan_ptr, new_ptr;
	Random random;

	Time_Index time_index;
	Time_Map_Stat time_stat;

int iteration = 1;
part = 1;

	//---- process each plan in the array ----

	for (itr = array_ptr->begin (); itr != array_ptr->end (); itr++) {
		new_ptr = *itr;
		if (new_ptr == 0) continue;

		if (new_ptr->Problem () > 0) {
			Set_Problem ((Problem_Type) new_ptr->Problem ());
		} else {
			//if (rider_flag) {
			//	part_processor.Sum_Ridership (*new_ptr, part);
			//}			
			//if (new_ptr->Method () == REROUTE_PATH) {
			//	num_reroute++;
			//} else if (new_ptr->Method () == UPDATE_PLAN) {
			//	num_update++;
			//} else {
			//	num_build++;
			//}
		}

		//---- select the best plan ----
					
		plan_ptr = &plan_array [new_ptr->Index ()];

		keep_new = (plan_ptr->Household () <= 0 || plan_ptr->size () == 0 || plan_ptr->Problem () > 0);

		if (plan_ptr->Priority () == NO_PRIORITY) {
			priority = NO_PRIORITY;
			keep_new = true;
		} else {
			priority = CRITICAL;
		}
		if (!keep_new && new_ptr->Problem () == 0) {

			//---- select the plan to keep ----

			if (plan_ptr->Impedance () > 0) {
				priority = LOW;

				//---- compare plan times ----

				if (time_diff_flag) {
					time1 = new_ptr->Arrive () - new_ptr->Depart ();
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

				//---- compare trip times ----

				if (trip_diff_flag) {
					time1 = new_ptr->Arrive () - new_ptr->Depart ();
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
			} else {
				keep_new = true;
			}
		}
		if (keep_new) {
			plan_ptr->clear ();
			*plan_ptr = *new_ptr;
		}
		plan_ptr->Priority (priority);

		if (plan_ptr->Problem () == 0) {
			plan_ptr->Get_Index (time_index);

			time_stat = plan_time_map.insert (Time_Map_Data (time_index, plan_ptr->Index ()));

			if (!time_stat.second) {
				Warning (String ("Duplicate Plan Index = %s-%d-%d") % 
					time_index.Start ().Time_String () % 
					time_index.Household () % time_index.Person ());
			}
		}

		//if (select_priorities && select_priority [plan_ptr->Priority ()]) {
		//	num_selected++;
		//}

		//if (save_trip_gap || trip_gap_map_flag) {
		//	Gap_Data *gap_ptr;

		//	gap_ptr = &gap_data_array [plan_ptr->Index ()];
		//	gap_ptr->current = (int) plan_ptr->Impedance ();

		//	if (plan_ptr->Constraint () == END_TIME) {
		//		gap_ptr->time = plan_ptr->End ();
		//	} else {
		//		gap_ptr->time = plan_ptr->Start ();
		//	}
		//}
		delete new_ptr;
	}
	delete array_ptr;
	return (true);
}

