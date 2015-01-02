//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Router::Execute (void)
{
	int i;

	//---- read the network data ----

	Router_Service::Execute ();

	//---- build performance arrays ----

	if (Flow_Updates () || Time_Updates ()) {

		Build_Perf_Arrays (old_perf_period_array);
		if (Turn_Updates ()) {
			Build_Turn_Arrays (old_turn_period_array);
		}

		if (!System_File_Flag (PERFORMANCE)) {
			Build_Perf_Arrays ();
		} else {
			if (reroute_flag && Time_Updates ()) {
				Update_Travel_Times (1, reroute_time);
				num_time_updates++;
			}
			old_perf_period_array.Copy_Flow_Data (perf_period_array, true, reroute_time);
		}
		if (!System_File_Flag (TURN_DELAY)) {
			Build_Turn_Arrays ();
		} else if (Turn_Updates ()) {
			old_turn_period_array.Copy_Turn_Data (turn_period_array, true, reroute_time);
		}
	}

	//---- create the file partitions ----

	Set_Partitions ();

	//---- allocate memory ----

	if (Memory_Flag ()) {
		Input_Trips ();
		Initialize_Trip_Gap ();
	} else if (save_trip_gap && plan_memory_flag) {
		Initialize_Trip_Gap ();
	}
	Print (1);

	first_iteration = !(System_File_Flag (PLAN) && System_File_Flag (PERFORMANCE));

	//---- processing method ----

	if (method == DUE_PLANS) {

		DUE_Loop ();

	} else {

		//---- initialize the plan processor ---

		if (trip_flag) {
			Iteration_Loop ();
		} else {
			if (preload_flag) {
				Preload_Transit ();
			}
			part_processor.Copy_Plans ();

			if (Time_Updates () && System_File_Flag (NEW_PERFORMANCE)) {
				Update_Travel_Times (1, reroute_time);
				num_time_updates++;
			}
		}
	}

	//---- save the performance data ----

	if (System_File_Flag (NEW_PERFORMANCE)) {
		if (save_iter_flag) {
			Performance_File *file = System_Performance_File (true);
			if (file->Part_Flag ()) {
				file->Open (0);
			} else {
				file->Create ();
			}
		}
		Write_Performance (full_flag);
	}

	//---- save the turn time data ----

	if (System_File_Flag (NEW_TURN_DELAY)) {
		if (save_iter_flag) { 
			Turn_Delay_File *file = System_Turn_Delay_File (true);
			if (file->Part_Flag ()) {
				file->Open (0);
			} else {
				file->Create ();
			}
		}
		Write_Turn_Delays (full_flag);
	}

	//---- save the transit ridership ----

	if (rider_flag) {
		part_processor.Save_Riders ();
		if (save_iter_flag) System_Ridership_File (true)->Create ();
		Write_Ridership ();
	}

	//---- gather summary statistics ----

	if (plan_memory_flag) {
		if (new_plan_flag) {
			Write_Plan_Files ();
		} else {
			Plan_Itr plan_itr;
			Reset_Problems ();

			for (plan_itr = plan_array.begin (); plan_itr != plan_array.end (); plan_itr++) {
				if (plan_itr->Problem () > 0) {
					Set_Problem ((Problem_Type) plan_itr->Problem ());

					if (problem_flag) {
						plan_itr->External_IDs ();

						if (problem_set_flag) {
							i = plan_itr->Partition ();
							Write_Problem (problem_set [i], &(*plan_itr));
						} else {
							Write_Problem (problem_file, &(*plan_itr));
						}
					}
				}
			}
		}
	} else {
		if (trip_flag && trip_set_flag) {
			for (i=0; i < num_file_sets; i++) {
				trip_file->Add_Counters (trip_file_set [i]);
				trip_file_set [i]->Close ();
			}
			if (trip_file->Num_Files () == 0) trip_file->Num_Files (num_file_sets);
		}
		if (plan_flag && new_set_flag) {
			for (i=0; i < num_file_sets; i++) {
				plan_file->Add_Counters (plan_file_set [i]);
				plan_file_set [i]->Close ();
			}
			if (plan_file->Num_Files () == 0) plan_file->Num_Files (num_file_sets);
		}
	}
	if (new_plan_flag && new_set_flag) {
		for (i=0; i < num_file_sets; i++) {
			new_plan_file->Add_Counters (new_file_set [i]);
			new_file_set [i]->Close ();
		}
		if (new_plan_file->Num_Files () == 0) new_plan_file->Num_Files (num_file_sets);
	}
	if (problem_set_flag) {
		for (i=0; i < num_file_sets; i++) {
			problem_file->Add_Counters (problem_set [i]);
			problem_set [i]->Close ();
		}
		if (problem_file->Num_Files () == 0) problem_file->Num_Files (num_file_sets);
	}

	//---- print reports ----

	Print_Reports ();

	//---- print summary statistics ----
	
	if (Time_Updates ()) Print (2, "Number of Travel Time Updates = ") << num_time_updates;

	if (trip_flag) {
		trip_file->Print_Summary ();

		if (num_trip_sel != num_trip_rec) {
			Print (1, "Number of Records Selected = ") << num_trip_sel;
		}
	}
	if (plan_flag) {
		plan_file->Print_Summary ();
		if (num_reroute > 0) Print (1, "Number of Re-Routed Plans      = ") << num_reroute;
		if (num_update > 0) Print (1,  "Number of Updated Plans        = ") << num_update;
		if (num_copied > 0) Print (1,  "Number of Plans Copied         = ") << num_copied;
	}
	if (new_plan_flag) {
		new_plan_file->Print_Summary ();
	}
	if (problem_flag) {
		if (problem_file->Num_Trips () > 0) {
			problem_file->Print_Summary ();
		}
	}

	//---- end the program ----

	Report_Problems (total_records);
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Print_Reports
//---------------------------------------------------------

void Router::Print_Reports (void)
{
	Router_Service::Print_Reports ();
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Router::Page_Header (void)
{
	switch (Header_Number ()) {
		case 0:
		default:
			Router_Service::Page_Header ();
			break;
	}
}

