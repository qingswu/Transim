//*********************************************************
//	DUE_Loop.cpp - Dynamic User Equilibrium Processing
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	DUE_Loop
//---------------------------------------------------------

void Router::DUE_Loop (void)
{
	int i, num, last_hhold, skip;
	double gap, last_gap;
	bool converge_flag;

	Trip_Map_Itr map_itr;
	Trip_Data *trip_ptr;
	Plan_Ptr plan_ptr, old_plan_ptr;
	Plan_Ptr_Array *ptr_array;

	if (max_iteration == 0) max_iteration = 1;

	if (plan_processor == 0) {
		plan_processor = new Plan_Processor (this);
	}

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

		if (!first_iteration) {
			Use_Link_Delays (true);
		}
		if (Master ()) {
			Show_Message (1, String ("Iteration Number %d Record") % iteration);
			Print (2, "Iteration Number ") << iteration;
			Set_Progress ();

			if (link_gap_flag) {
				link_gap_file.File () << iteration;
			}
			if (trip_gap_flag) {
				trip_gap_file.File () << iteration;
			}
			if (first_iteration && (rider_flag || (System_File_Flag (RIDERSHIP) && Cap_Penalty_Flag ()))) {
				line_array.Clear_Ridership ();
			}
		}
		converge_flag = true;
		last_hhold = -1;
		total_records = skip = 0;

		//---- preload transit vehicles ----

		if (preload_flag) {
			Preload_Transit ();
		}

		//---- update the speed before each path building iteration ----

		//Update_Travel_Times (true, true);
		//num_time_updates++;

		ptr_array = new Plan_Ptr_Array ();
		plan_processor->Start_Processing (true, true);

		//---- process each trip ----

		for (map_itr = trip_map.begin (); map_itr != trip_map.end (); map_itr++) {
			Show_Progress ();

			trip_ptr = &trip_array [map_itr->second];

			//plan_ptr = new Plan_Data ();

			//*plan_ptr = *trip_ptr;

			//---- check the household id ----

			if (trip_ptr->Household () < 1) continue;
			total_records++;

			if (trip_ptr->Household () != last_hhold) {
				if (last_hhold > 0 && ptr_array->size () > 0) {
					plan_processor->Plan_Build (ptr_array);
					ptr_array = new Plan_Ptr_Array ();
				}
				last_hhold = trip_ptr->Household ();
			}

			//---- update the selection priority flag ----

			if (!first_iteration && select_priorities) {
				old_plan_ptr = &plan_array [trip_ptr->Index ()];
	
				if (!select_priority [old_plan_ptr->Priority ()]) {
					skip++;
					//plan_ptr = old_plan_ptr;
					//plan_ptr->Method (RESKIM_PLAN);
					plan_ptr = new Plan_Data ();
					*plan_ptr = *old_plan_ptr;

					plan_ptr->Method (UPDATE_PLAN);
				} else {
					plan_ptr = new Plan_Data ();
					*plan_ptr = *trip_ptr;

					plan_ptr->Priority (old_plan_ptr->Priority ());
					plan_ptr->Method (BUILD_PATH);
				}
			} else {
				plan_ptr = new Plan_Data ();
				*plan_ptr = *trip_ptr;

				plan_ptr->Method (BUILD_PATH);
			}
			ptr_array->push_back (plan_ptr);
		}

		//---- process the last household ----

		if (last_hhold > 0 && ptr_array->size () > 0) {
			plan_processor->Plan_Build (ptr_array);
		} else {
			delete ptr_array;
		}
		plan_processor->Stop_Processing (true);

		End_Progress (false);
		Show_Message (0, " -- Skip ") << skip;

		//---- gather flows and update travel times ----

		num = max_speed_updates;

		if (num > 0) {
			Show_Message (0, " -- Reskim");
			Set_Progress ();

			last_gap = 0.0;

			for (i=1; i <= num; i++) {
				gap = Reskim_Plans ((i != num));
				if (i < num && last_gap > 0) {
					if ((fabs (gap - last_gap) / last_gap) < min_speed_diff) num = i + 1;
				}
				last_gap = gap;
			}
			End_Progress ();

			Write (1, "Skim Convergence Gap = ") << last_gap;
		} else {
			Update_Travel_Times ();
			num_time_updates++;
		}

		//---- trip gap ----

		if (save_trip_gap) {
			gap = Get_Trip_Gap ();
			Write (1, "Trip Convergence Gap = ") << gap;

			if (trip_gap > 0.0 && gap > trip_gap) converge_flag = false;
		}

		//---- link gap ----

		if (save_link_gap) {
			gap = Get_Link_Gap (false);
			Write (1, "Link Convergence Gap = ") << gap;

			if (link_gap > 0.0 && gap > link_gap) converge_flag = false;
		}
		if (converge_flag) break;

		if (save_trip_gap || save_link_gap) {
			Show_Message (1);
		}

		//---- save / reset the iteration summary ----

		if (iteration < max_iteration) {
			if (save_iter_flag && save_iter_range.In_Range (iteration)) {
				if (System_File_Flag (NEW_PERFORMANCE)) {
					Db_File *file = System_File_Handle (NEW_PERFORMANCE);
					if (file->Part_Flag ()) {
						file->Open (iteration);
					} else {
						file->Create ();
					}
					Write_Performance (full_flag);
				}
			}

			if (Master ()) {

				//---- copy existing flow data ----

				old_perf_period_array.Copy_Flow_Data (perf_period_array, true, reroute_time);

				if (Turn_Flows ()) {
					old_turn_period_array.Copy_Turn_Data (turn_period_array, true, reroute_time);
				}

			} else {	//---- MPI slave ----

				//---- clear flow data ----

				perf_period_array.Zero_Flows ();

				if (Turn_Flows ()) {
					turn_period_array.Zero_Turns ();
				}
			}

			//---- print the iteration problems ----

			if (Report_Flag (ITERATION_PROBLEMS)) {
				Report_Problems (total_records, false);
			}
			Reset_Problems ();
		}
	}
	Show_Message (1);
}
