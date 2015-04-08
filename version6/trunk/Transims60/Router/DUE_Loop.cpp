//*********************************************************
//	DUE_Loop.cpp - Dynamic User Equilibrium Processing
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	DUE_Loop
//---------------------------------------------------------

void Router::DUE_Loop (void)
{
	int i, num;
	double gap, last_gap;
	bool converge_flag;

	clock_t path_time, update_time, total_time;

	if (plan_array.size () == 0) return;
	
	if (max_iteration == 0) max_iteration = 1;

	//---- process each iteration ----

	for (iteration=1; iteration <= max_iteration; iteration++) {

		Show_Message (1, String ("Iteration Number %d") % iteration);
		Print (2, "Iteration Number ") << iteration;
		Set_Progress ();

		Iteration_Setup ();

		converge_flag = true;

		Show_Message (0, " -- Trip");
		Set_Progress ();

		//---- update the speed before each path building iteration ----

		part_processor.plan_process->Start_Processing (true, true);

		path_time = clock ();

		Trip_Loop ();

		part_processor.plan_process->Stop_Processing (true);

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
			Update_Travel_Times (Average_Times () && iteration != max_iteration);
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

		//---- update transit penalties ----

		if (iteration != max_iteration && (rider_flag || (System_File_Flag (RIDERSHIP) && Cap_Penalty_Flag ()))) {
			part_processor.Save_Riders ();

			gap = line_array.Ridership_Gap (Cap_Penalty_Flag (), initial_factor);

			Write (1, "Transit Capacity Gap  = ") << gap;

			if (transit_gap > 0.0 && gap > transit_gap) converge_flag = false;
		}

		//---- build count ----

		Write (2, String ("Build Selection Total = %d (%.1lf%%)") % potential % (total_percent * 100.0) % FINISH);

		Write (1, "Number of Paths Built = ") << num_build;
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

		if (save_trip_gap || save_link_gap || rider_flag) {
			Show_Message (1);
		}

		//---- save / reset the iteration summary ----

		if (iteration < max_iteration) {
			Iteration_Output ();
		}
	}
	Show_Message (1);
}
