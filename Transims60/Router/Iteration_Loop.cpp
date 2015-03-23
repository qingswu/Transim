//*********************************************************
//	Iteration_Loop.cpp - Interate Parition Processing
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Iteration_Loop
//---------------------------------------------------------

void Router::Iteration_Loop (void)
{
	int num, num_car, max_hhold, veh;
>>>>>>> .merge-right.r1529
	int last_hhold, skip_hhold, skip_person;

	Dtime second;
	bool old_flag, duration_flag, last_skip, last_flag, link_last_flag, trip_last_flag, transit_last_flag;
	bool last_new_plan_flag, last_problem_flag, last_skim_only, save_flag;
	double gap, new_factor;
	
	clock_t path_time, update_time, total_time;

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

	//---- process each iteration ----

	for (iteration=1; iteration <= max_iteration; iteration++) {
		last_flag = (iteration == max_iteration);

		save_flag = (save_iter_flag && save_iter_range.In_Range (iteration));

		if (max_iteration > 1) {
			Print (1);
			Write (1, "Iteration Number ") << iteration << ":  Weighting Factor = " << initial_factor;
			if (!thread_flag) Show_Message (1);
		}
		if (trip_set_flag) Show_Message (1);

		Iteration_Setup ();

		if (last_flag) {
			new_plan_flag = last_new_plan_flag;
			problem_flag = last_problem_flag;
			param.skim_only = last_skim_only;
		} else {
			new_plan_flag = problem_flag = false;
			param.skim_only = true;
		}

		//---- process each partition ----
		
		path_time = clock ();

		part_processor.Read_Trips ();
		
		path_time = (clock () - path_time);

		link_last_flag = trip_last_flag = transit_last_flag = true;
		new_factor = initial_factor;

		//---- calculate the link gap ----
		
		update_time = clock ();

		if (Time_Updates ()) {
			if (min_vht_flag) {
				gap = Minimize_VHT (new_factor, (!last_flag && !save_flag));
				Print (0, " to ") << new_factor;
			} else {
				gap = Merge_Delay (new_factor, (!last_flag && !save_flag));
			}
			if (max_iteration > 1) Write (1, "Link Convergence Gap  = ") << gap;
			num_time_updates++;

			if (link_gap > 0.0 && gap > link_gap) link_last_flag = false;

			if (!min_vht_flag) {
				new_factor += factor_increment;
				if (new_factor > maximum_factor) new_factor = maximum_factor;
			}
		}
		update_time = (clock () - update_time);

		//---- calculate the trip gap ----

		if (trip_gap_map_flag) {
			gap = Get_Trip_Gap ();

			Write (1, "Trip Convergence Gap  = ") << gap;

			if (trip_gap > 0.0 && gap > trip_gap) trip_last_flag = false;
		}

		//---- update transit penalties ----

		if (!last_flag && (rider_flag || (System_File_Flag (RIDERSHIP) && param.cap_penalty_flag))) {

			part_processor.Save_Riders ();

			gap = line_array.Ridership_Gap (param.cap_penalty_flag, initial_factor);

			Write (1, "Transit Capacity Gap  = ") << gap;

			if (transit_gap > 0.0 && gap > transit_gap) transit_last_flag = false;
		}

		//---- build count ----

		if (method != DTA_FLOWS) {
			Write (2, String ("Build Selection Total = %d (%.1lf%%)") % potential % (total_percent * 100.0) % FINISH);
		} else {
			Write (1);
		}
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

		//---- convergence check ----

		if (!last_flag && link_last_flag && trip_last_flag && transit_last_flag) {
			max_iteration = iteration + 1;
		}
		initial_factor = new_factor;

		//---- print the iteration problems ----

		if (!last_flag && max_iteration > 1 && Report_Flag (ITERATION_PROBLEMS)) {
			Report_Problems (total_records, false);
		}

		//---- reset the file counters ----

		Show_Message (1);

		if (!last_flag) {
			Iteration_Output ();
		}
	}
}
