//*********************************************************
//	DUE_Loop.cpp - Dynamic User Equilibrium Processing
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	DUE_Loop
//---------------------------------------------------------

void Router::DUE_Loop (void)
{
	int i, num, last_hhold;
	double gap, last_gap;
	bool converge_flag, build_flag;

	clock_t path_time, update_time, total_time;

	Trip_Map_Itr map_itr;
	Plan_Ptr plan_ptr;
	Plan_Ptr_Array *ptr_array;
	
	if (max_iteration == 0) max_iteration = 1;

	if (plan_processor == 0) {
		plan_processor = new Plan_Processor (this);
	}

	//---- process each iteration ----

	for (iteration=1; iteration <= max_iteration; iteration++) {

		if (!first_iteration) {
			Use_Link_Delays (true);
		}
		Show_Message (1, String ("Iteration Number %d") % iteration);
		Print (2, "Iteration Number ") << iteration;
		Set_Progress ();

		Iteration_Setup ();

		if (first_iteration && (rider_flag || (System_File_Flag (RIDERSHIP) && Cap_Penalty_Flag ()))) {
			line_array.Clear_Ridership ();
		}
		converge_flag = true;
		last_hhold = -1;

		if (total_records > 0 && max_percent_flag) {
			percent_selected = ((double) num_selected / total_records);

			if (percent_selected > max_percent_select) {
				percent_selected = max_percent_select / percent_selected;
			} else {
				percent_selected = 1.0;
			}
		} else {
			percent_selected = 1.0;
		}
		total_records = num_selected = 0;

		//---- preload transit vehicles ----

		if (preload_flag) {
			Preload_Transit ();
		}
		Show_Message (0, " -- Trip");
		Set_Progress ();

		//---- update the speed before each path building iteration ----

		//Update_Travel_Times (true, true);
		//num_time_updates++;

		ptr_array = new Plan_Ptr_Array ();
		plan_processor->Start_Processing (true, true);

		path_time = clock ();

		//---- process each trip ----

		for (map_itr = plan_trip_map.begin (); map_itr != plan_trip_map.end (); map_itr++) {
			Show_Progress ();

			plan_ptr = new Plan_Data ();

			*plan_ptr = plan_array [map_itr->second];

			//---- check the household id ----

			if (plan_ptr->Household () < 1) continue;

			if (plan_ptr->Household () != last_hhold) {
				if (last_hhold > 0 && ptr_array->size () > 0) {
					plan_processor->Plan_Build (ptr_array);
					ptr_array = new Plan_Ptr_Array ();
				}
				last_hhold = plan_ptr->Household ();
			}

			//---- update the selection priority flag ----

			if (plan_ptr->Priority () == NO_PRIORITY) {
				plan_ptr->Method (COPY_PLAN);
			} else if (!first_iteration && select_priorities) {
				build_flag = select_priority [plan_ptr->Priority ()];

				if (build_flag && max_percent_flag && percent_selected < 1.0) {
					build_flag = (random_select.Probability () <= percent_selected);
				}
				if (build_flag) {
					plan_ptr->Method (BUILD_PATH);
				} else {
					plan_ptr->Method (COPY_PLAN);
				}
			} else {
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
		
		path_time = (clock () - path_time);

		//---- gather flows and update travel times ----

		num = max_speed_updates;
		update_time = clock ();

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

			Write (1, "Skim Convergence Gap  = ") << last_gap;
		} else {
			Update_Travel_Times ();
			num_time_updates++;
		}
		update_time = (clock () - update_time);

		//---- trip gap ----

		if (save_trip_gap) {
			gap = Get_Trip_Gap ();
			Write (1, "Trip Convergence Gap  = ") << gap;

			if (trip_gap > 0.0 && gap > trip_gap) converge_flag = false;
		}

		//---- link gap ----

		if (save_link_gap) {
			gap = Get_Link_Gap (false);
			Write (1, "Link Convergence Gap  = ") << gap;

			if (link_gap > 0.0 && gap > link_gap) converge_flag = false;
		}

		//---- build count ----

		Write (2, "Number of Paths Built = ") << num_build;
		num = num_build + num_update + num_copied;
		if (num > 0) Write (0, String (" (%.1lf%%)") % (num_build * 100.0 / num) % FINISH);

		//---- processing time summary ----
	
		total_time = path_time + update_time;
		if (total_time == 0) total_time = 1;

		Write (1, String ("Path Building Seconds = %.1lf (%.1lf%%)") % 
			((double) path_time / CLOCKS_PER_SEC) % (100.0 * path_time / total_time) % FINISH);
		Write (1, String ("Time Updating Seconds = %.1lf (%.1lf%%)") %
			((double) update_time / CLOCKS_PER_SEC) % (100.0 * update_time / total_time) % FINISH);

		if (converge_flag) break;

		if (save_trip_gap || save_link_gap) {
			Show_Message (1);
		}

		//---- save / reset the iteration summary ----

		if (iteration < max_iteration) {
			if (save_iter_flag && save_iter_range.In_Range (iteration)) {
				if (System_File_Flag (NEW_PERFORMANCE)) {
					Performance_File *file = System_Performance_File (true);
					if (file->Part_Flag ()) {
						file->Open (iteration);
					} else {
						file->Create ();
					}
					Write_Performance (full_flag);
				}
				if (System_File_Flag (NEW_TURN_DELAY) && System_File_Flag (SIGNAL)) {
					Turn_Delay_File *file = System_Turn_Delay_File (true);
					if (file->Part_Flag ()) {
						file->Open (iteration);
					} else {
						file->Create ();
					}
					Write_Turn_Delays (full_flag);
				}
			}


			//---- copy existing flow data ----

			old_perf_period_array.Copy_Flow_Data (perf_period_array, true, reroute_time);

			if (Turn_Flows ()) {
				old_turn_period_array.Copy_Turn_Data (turn_period_array, true, reroute_time);
			}

			//---- print the iteration problems ----

			if (Report_Flag (ITERATION_PROBLEMS)) {
				Report_Problems (total_records, false);
			}
			num_build = num_reroute = num_reskim = num_update = num_copied = 0;
			Reset_Problems ();
		}
	}
	Show_Message (1);
}
