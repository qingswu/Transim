//*********************************************************
//	Iteration_Loop.cpp - Interate Parition Processing
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Iteration_Loop
//---------------------------------------------------------

void Router::Iteration_Loop (void)
{
	int i, p, num, num_car, max_hhold, veh;
	int last_hhold, skip_hhold, skip_person;
	
	Dtime second;
	bool old_flag, duration_flag, last_skip, last_flag, link_last_flag, trip_last_flag, transit_last_flag;
	bool last_plan_set_flag, last_new_plan_flag, last_problem_flag, last_skim_only;
	double gap;

	Trip_Data trip_data;
	Plan_Data *plan_ptr, *old_plan_ptr;
	Path_Parameters param;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Select_Map_Itr sel_itr;
	Trip_Index trip_index, old_trip_index;
	Trip_Gap_Data trip_gap_data;
	Trip_Gap_Map_Stat map_stat;

	Set_Parameters (param);

	num_car = veh = num = 0;
	last_hhold = skip_hhold = skip_person = -1;
	max_hhold = MAX_INTEGER;
	plan_ptr = old_plan_ptr = 0;
	old_flag = duration_flag = last_skip = false;
	last_plan_set_flag = plan_set_flag;
	last_new_plan_flag = new_plan_flag;
	last_problem_flag = problem_flag;
	last_skim_only = param.skim_only;

	second.Seconds (1);

	//---- backup parking locations ----

	if (max_iteration == 0) max_iteration = 1;

	//---- process each iteration ----

	for (i=1; i <= max_iteration; i++) {
		last_flag = (i == max_iteration);
		first_iteration = (i == 1);

		if (!first_iteration) {
			Link_Delays (true);
		}

		if (Master ()) {
			if (max_iteration > 1) {
				Write (2, "Iteration Number ") << i << ":  Weighting Factor = " << factor;
				if (!thread_flag) Show_Message (1);
			}
			if (trip_set_flag) Show_Message (1);

			if (link_gap_flag) {
				if (i == 1) {
					link_gap_file.File () << "ITERATION";
					int num = sum_periods.Num_Periods ();
					for (int j=0; j < num; j++) {
						link_gap_file.File () << "\t" << sum_periods.Range_Label (j);
					}
					link_gap_file.File () << "\tTOTAL" << endl;
				}
				link_gap_file.File () << i;
			}
			if (trip_gap_flag) {
				if (i == 1) {
					trip_gap_file.File () << "ITERATION";
					int num = sum_periods.Num_Periods ();
					for (int j=0; j < num; j++) {
						trip_gap_file.File () << "\t" << sum_periods.Range_Label (j);
					}
					trip_gap_file.File () << "\tTOTAL" << endl;
				}
				trip_gap_file.File () << i;
			}
			if (first_iteration && (rider_flag || (System_File_Flag (RIDERSHIP) && param.cap_penalty_flag))) {
				line_array.Clear_Ridership (param.cap_penalty_flag);
			}
		}
		total_records = 0;

		if (last_flag) {
			plan_set_flag = last_plan_set_flag;
			new_plan_flag = last_new_plan_flag;
			problem_flag = last_problem_flag;
			param.skim_only = last_skim_only;
		} else {
			plan_set_flag = new_plan_flag = problem_flag = false;
			param.skim_only = true;
		}

		//---- process each partition ----

		part_processor.Read_Trips ();

		link_last_flag = trip_last_flag = transit_last_flag = true;

		//---- update transit penalties ----

		if (!last_flag && (rider_flag || (System_File_Flag (RIDERSHIP) && param.cap_penalty_flag))) {
			part_processor.Save_Riders ();

			if (save_flag && rider_flag) {
				System_File_Handle (NEW_RIDERSHIP)->Create ();
				Write_Ridership ();
			}
			gap = line_array.Clear_Ridership (param.cap_penalty_flag, factor);

			if (Master ()) {
				if (iteration_flag || plan_flag) {
					if (!iteration_flag) Print (1);
					Print (1, "Transit Capacity Gap = ") << gap;
					if (thread_flag || !iteration_flag) {
						Show_Message ("Transit Capacity Gap = ") << gap;
					} else {
						Show_Message (0, "\tTransit Capacity Gap = ") << gap;
					}
				}
			}
			if (transit_gap > 0.0 && gap > transit_gap) transit_last_flag = false;
		}

		//---- calculate the link gap ----

		if (Time_Updates ()) {
			if (MPI_Size () > 1) {
				gap = MPI_Link_Delay (last_flag);
			} else if (min_vht_flag) {
				gap = Minimize_VHT (factor, (!last_flag && !save_flag));
				Print (0, " to ") << factor;
			} else {
				gap = Merge_Delay (factor, (!last_flag && !save_flag));
			}
			if (Master ()) {
				if (iteration_flag || plan_flag) {
					if (!iteration_flag) Print (1);
					Print (1, "Link Convergence Gap = ") << gap;
					if (thread_flag || !iteration_flag || 
						(!last_flag && (rider_flag || (System_File_Flag (RIDERSHIP) && param.cap_penalty_flag)))) {
						Show_Message ("Link Convergence Gap = ") << gap;
					} else {
						Show_Message (0, "\tLink Convergence Gap = ") << gap;
					}
				}
				num_time_updates++;
			}
			if (link_gap > 0.0 && gap > link_gap) link_last_flag = false;

			if (!min_vht_flag) {
				factor += increment;
				if (factor > max_factor) factor = max_factor;
			}
		}

		//---- calculate the trip gap ----

		if (trip_gap_map_flag) {
			gap = Get_Trip_Gap ();

			if (Master ()) {
				Write (1, "Trip Convergence Gap = ") << gap;
			}
			if (trip_gap > 0.0 && gap > trip_gap) trip_last_flag = false;
		}

		//---- convergence check ----

		if (!last_flag && link_last_flag && trip_last_flag && transit_last_flag) {
			max_iteration = i + 1;
		}

		//---- print the iteration problems ----

		if (!last_flag && max_iteration > 1 && Report_Flag (ITERATION_PROBLEMS)) {
#ifdef MPI_EXE
			int lvalue [MAX_PROBLEM + 1];

			if (Master ()) {
				for (int j=1; j < MPI_Size (); j++) {
					Get_MPI_Array (lvalue, MAX_PROBLEM + 1, j);

					Add_Problems (lvalue);

					total_records += lvalue [MAX_PROBLEM];
				}
				Report_Problems (total_records, false);
			} else {
				memcpy (lvalue, Get_Problems (), MAX_PROBLEM * sizeof (int));
				lvalue [MAX_PROBLEM] = total_records;

				Send_MPI_Array (lvalue, MAX_PROBLEM + 1);
			}
#else
			if (Master ()) {
				Report_Problems (total_records, false);
			}
#endif
		}

		//---- reset the file counters ----

		if (!last_flag) {
			if (save_flag && System_File_Flag (NEW_LINK_DELAY)) {
				System_File_Handle (NEW_LINK_DELAY)->Create ();

				Write_Link_Delays ();

				link_delay_array.Zero_Flows (reroute_time);
				if (Turn_Flows ()) {
					turn_delay_array.Zero_Flows (reroute_time);
				}
			}
			if (save_flag && link_person_flag) {
				link_person_file.Create ();

				Write_Link_Delays (link_person_file, link_person_array);

				link_person_array.Zero_Flows (reroute_time);
			}
			if (save_flag && link_vehicle_flag) {
				link_vehicle_file.Create ();

				Write_Link_Delays (link_vehicle_file, link_vehicle_array);

				link_vehicle_array.Zero_Flows (reroute_time);
			}

			if (trip_flag) {
				if (trip_set_flag) {
					for (p=0; p < num_file_sets; p++) {
						trip_set [p]->Open (p);
						trip_set [p]->Reset_Counters ();
					}
				} else {
					trip_file->Open (0);
					trip_file->Reset_Counters ();
				}
			}
			if (plan_flag) {
				if (last_plan_set_flag) {
					for (p=0; p < num_file_sets; p++) {
						plan_set [p]->Open (p);
						plan_set [p]->Reset_Counters ();

						if (old_plan_flag) {
							if (old_plan_array [p] != 0) {
								delete old_plan_array [p];
							}
						}
					}
				} else {
					plan_file->Open (0);
					plan_file->Reset_Counters ();
				}
				num_reroute = num_reskim = 0;
			}
			Reset_Problems ();
		}
	}
	Show_Message (1);
}
