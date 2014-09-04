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
	bool last_new_plan_flag, last_problem_flag, last_skim_only;
	double gap, new_factor, save_flag;

	Trip_Data trip_data;
	Path_Parameters param;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Select_Map_Itr sel_itr;
	Trip_Index trip_index, old_trip_index;
	Trip_Gap_Map_Stat map_stat;

	Set_Parameters (param);

	num_car = veh = num = 0;
	last_hhold = skip_hhold = skip_person = -1;
	max_hhold = MAX_INTEGER;
	old_flag = duration_flag = last_skip = false;
	last_new_plan_flag = new_plan_flag;
	last_problem_flag = problem_flag;
	last_skim_only = param.skim_only;

	second.Seconds (1);

	if (max_iteration == 0) max_iteration = 1;

	if (Master ()) {
		if (link_gap_flag) {
			link_gap_file.File () << "ITERATION";
			num = sum_periods.Num_Periods ();
			for (i=0; i < num; i++) {
				link_gap_file.File () << "\t" << sum_periods.Range_Label (i);
			}
			link_gap_file.File () << "\tTOTAL" << endl;
		}
		if (trip_gap_flag) {
			trip_gap_file.File () << "ITERATION";
			num = sum_periods.Num_Periods ();
			for (i=0; i < num; i++) {
				trip_gap_file.File () << "\t" << sum_periods.Range_Label (i);
			}
			trip_gap_file.File () << "\tTOTAL" << endl;
		}
	}

	//---- process each iteration ----

	for (iteration=1; iteration <= max_iteration; iteration++) {
		last_flag = (iteration == max_iteration);
		first_iteration = (iteration == 1);

		if (!first_iteration) {
			Use_Link_Delays (true);
		}
		save_flag = (save_iter_flag && save_iter_range.In_Range (iteration));

		if (Master ()) {
			if (max_iteration > 1) {
				Print (1);
				Write (1, "Iteration Number ") << iteration << ":  Weighting Factor = " << factor;
				if (!thread_flag) Show_Message (1);
			}
			if (trip_set_flag) Show_Message (1);

			if (link_gap_flag) {
				link_gap_file.File () << iteration;
			}
			if (trip_gap_flag) {
				trip_gap_file.File () << iteration;
			}
			if (rider_flag || (System_File_Flag (RIDERSHIP) && param.cap_penalty_flag)) {
				line_array.Clear_Ridership ();
			}
		}
		total_records = 0;

		if (last_flag) {
			new_plan_flag = last_new_plan_flag;
			problem_flag = last_problem_flag;
			param.skim_only = last_skim_only;
		} else {
			new_plan_flag = problem_flag = false;
			param.skim_only = true;
		}

		//---- preload transit vehicles ----

		if (preload_flag) {
			Preload_Transit ();
		}

		//---- process each partition ----

		part_processor.Read_Trips ();

		link_last_flag = trip_last_flag = transit_last_flag = true;
		new_factor = factor;

		//---- calculate the link gap ----

		if (Time_Updates ()) {
			if (MPI_Size () > 1) {
				gap = MPI_Link_Delay (last_flag);
			} else if (min_vht_flag) {
				gap = Minimize_VHT (new_factor, (!last_flag && !save_flag));
				Print (0, " to ") << new_factor;
			} else {
				gap = Merge_Delay (new_factor, (!last_flag && !save_flag));
			}
			if (Master ()) {
				if (max_iteration > 1) Write (1, "Link Convergence Gap  = ") << gap;
				num_time_updates++;
			}
			if (link_gap > 0.0 && gap > link_gap) link_last_flag = false;

			if (!min_vht_flag) {
				new_factor += increment;
				if (new_factor > max_factor) new_factor = max_factor;
			}
		}

		//---- calculate the trip gap ----

		if (trip_gap_map_flag) {
			gap = Get_Trip_Gap ();

			if (Master ()) {
				Write (1, "Trip Convergence Gap  = ") << gap;
			}
			if (trip_gap > 0.0 && gap > trip_gap) trip_last_flag = false;
		}

		//---- update transit penalties ----

		if (!last_flag && (rider_flag || (System_File_Flag (RIDERSHIP) && param.cap_penalty_flag))) {
			part_processor.Save_Riders ();

			gap = line_array.Ridership_Gap (param.cap_penalty_flag, factor);

			if (Master ()) {
				Write (1, "Transit Capacity Gap  = ") << gap;
			}
			if (transit_gap > 0.0 && gap > transit_gap) transit_last_flag = false;
		}

		//---- build count ----

		Write (1, "Number of Paths Built = ") << num_build;
		num = num_build + num_update;
		if (num > 0) Write (0, String (" (%.1lf%%)") % (num_build * 100.0 / num) % FINISH);
		 
		//---- convergence check ----

		if (!last_flag && link_last_flag && trip_last_flag && transit_last_flag) {
			max_iteration = iteration + 1;
		}
		factor = new_factor;

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

		Show_Message (1);

		if (!last_flag) {
			if (save_flag) {
				if (System_File_Flag (NEW_PERFORMANCE)) {
					Db_File *file = System_File_Handle (NEW_PERFORMANCE);
					if (file->Part_Flag ()) {
						file->Open (iteration);
					} else {
						file->Create ();
					}
					Write_Performance (full_flag);

					perf_period_array.Zero_Flows (reroute_time);
					if (Turn_Flows ()) {
						turn_period_array.Zero_Turns (reroute_time);
					}
				}
				if (rider_flag) {
					System_File_Handle (NEW_RIDERSHIP)->Create ();
					Write_Ridership ();
				}
			}
			if (trip_flag && !trip_memory_flag) {
				if (trip_set_flag) {
					for (p=0; p < num_file_sets; p++) {
						trip_file_set [p]->Open (p);
						trip_file_set [p]->Reset_Counters ();
					}
				} else {
					trip_file->Open (0);
					trip_file->Reset_Counters ();
				}
			}
			if (plan_flag && !plan_memory_flag) {
				if (new_set_flag) {
					for (p=0; p < num_file_sets; p++) {
						plan_file_set [p]->Open (p);
						plan_file_set [p]->Reset_Counters ();
					}
				} else {
					plan_file->Open (0);
					plan_file->Reset_Counters ();
				}
			}
			num_build = num_reroute = num_reskim = num_update = 0;
			Reset_Problems ();
		}
	}
}
