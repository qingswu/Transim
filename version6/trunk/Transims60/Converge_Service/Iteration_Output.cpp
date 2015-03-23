//*********************************************************
//	Iteration_Output.cpp - save intermediate iteration data
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Iteration_Output
//---------------------------------------------------------

void Converge_Service::Iteration_Output (void)
{
	//---- save / reset the iteration summary ----

	if (save_iter_flag && save_iter_range.In_Range (iteration)) {

		if (System_File_Flag (NEW_PERFORMANCE)) {
			Performance_File *file = System_Performance_File (true);
			if (file->Part_Flag ()) {
				file->Open (iteration);
			} else {
				file->Create ();
			}
			Write_Performance ();
		}
		if (System_File_Flag (NEW_TURN_DELAY) && System_File_Flag (SIGNAL)) {
			Turn_Delay_File *file = System_Turn_Delay_File (true);
			if (file->Part_Flag ()) {
				file->Open (iteration);
			} else {
				file->Create ();
			}
			Write_Turn_Delays ();
		}
		if (rider_flag) {
			System_Ridership_File (true)->Create ();
			Write_Ridership ();
		}
		if (save_plan_flag) {
			Trip_Map_Itr map_itr;
			Plan_Ptr plan_ptr;
			Plan_Data plan_data;

			Show_Message (1, "Saving Plan Records");
			Set_Progress ();

			for (map_itr = plan_trip_map.begin (); map_itr != plan_trip_map.end (); map_itr++) {
				Show_Progress ();

				plan_ptr = &plan_array [map_itr->second];
				if (!plan_ptr->Path_Problem ()) {
					if (save_hhold_range.In_Range (plan_ptr->Household ())) {
						plan_data = *plan_ptr;
						if (plan_data.External_IDs ()) {
							if (!save_plan_file.Part_Flag ()) {
								plan_data.Person (iteration);
							}
							save_plan_file.Write_Plan (plan_data);
						}
					}
				}
			}
			End_Progress ();
		}
	}

	//---- copy existing flow data ----

	old_perf_period_array.Copy_Flow_Data (perf_period_array, true, reroute_time);

	if (Turn_Flows ()) {
		old_turn_period_array.Copy_Turn_Data (turn_period_array, true, reroute_time);
	}

	if (trip_flag && !trip_memory_flag) {
		if (trip_set_flag) {
			for (int p=0; p < num_file_sets; p++) {
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
			for (int p=0; p < num_file_sets; p++) {
				plan_file_set [p]->Open (p);
				plan_file_set [p]->Reset_Counters ();
			}
		} else {
			plan_file->Open (0);
			plan_file->Reset_Counters ();
		}
	}

	//---- print the iteration problems ----

	if (Report_Flag (ITERATION_PROBLEMS)) {
		Report_Problems (total_records, false);
	}
	num_build = num_reroute = num_reskim = num_update = num_copied = 0;
	Reset_Problems ();

	//---- reset capacity constraint ----

	if (capacity_flag) {
		Loc_Cap_Itr itr;

		for (itr = loc_cap_array.begin (); itr != loc_cap_array.end (); itr++) {
			itr->demand = 0;
			itr->failed = 0;
		}
	}

	//---- reset fuel constraint ----

	if (fuel_flag) {
		Loc_Fuel_Itr itr;

		for (itr = loc_fuel_array.begin (); itr != loc_fuel_array.end (); itr++) {
			itr->consumed = 0;
			itr->failed = 0;
		}
	}

	//---- reset the plan sort key ----

	if (time_order_flag) {
		plan_time_map = initial_time_map;
	}
}
