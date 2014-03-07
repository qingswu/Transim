//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Router::Execute (void)
{
	//---- read the network data ----

	Router_Service::Execute ();


	//---- build link delay arrays ----

	if (Flow_Updates () || Time_Updates ()) {
		if (Master ()) Build_Flow_Time_Arrays (old_link_array, old_turn_array);

		if (!System_File_Flag (LINK_DELAY)) {
			Build_Flow_Time_Arrays ();
		} else {
			Link_Delay_File *file = (Link_Delay_File *) System_File_Handle (LINK_DELAY);

			if (Turn_Updates () && turn_delay_array.size () == 0) {
				Build_Turn_Arrays ();
			}
			if (Master ()) {

				//---- copy existing flow data ----

				old_link_array.Copy_Flow_Data (link_delay_array, true);

				if (Turn_Flows () && file->Turn_Flag ()) {
					old_turn_array.Copy_Flow_Data (turn_delay_array, true);
				}

			} else {	//---- MPI slave ----

				//---- clear flows data ----

				link_delay_array.Zero_Flows (reroute_time);

				if (Turn_Flows () && file->Turn_Flag ()) {
					turn_delay_array.Zero_Flows (reroute_time);
				}
			}
		}
	}
	if (link_person_flag) {
		link_person_array.Initialize (&time_periods);

		if (max_iteration > 1 || old_person_flag) {
			old_person_array.Initialize (&time_periods);

			if (old_person_flag) {
				Flow_Time_Period_Array dummy_turn;
				Read_Link_Delays (old_person_file, old_person_array, dummy_turn);
			}
		}
	}
	if (link_vehicle_flag) {
		link_vehicle_array.Initialize (&time_periods);

		if (max_iteration > 1 || old_vehicle_flag) {
			old_vehicle_array.Initialize (&time_periods);

			if (old_vehicle_flag) {
				Flow_Time_Period_Array dummy_turn;
				Read_Link_Delays (old_vehicle_file, old_vehicle_array, dummy_turn);
			}
		}
	}

	//---- create the file partitions ----

	Set_Partitions ();

	//---- initialize the plan processor ---

	if (trip_flag) {
		Iteration_Loop ();
	} else {
		part_processor.Copy_Plans ();

		if (Time_Updates () && System_File_Flag (NEW_LINK_DELAY)) {
			Update_Travel_Times (1, reroute_time);
			num_time_updates++;
		}
	}

	//---- save the link delays ----

	if (System_File_Flag (NEW_LINK_DELAY)) {
		if (save_flag) System_File_Handle (NEW_LINK_DELAY)->Create ();
		Write_Link_Delays ();
	}

	//---- save the transit ridership ----

	if (rider_flag) {
		part_processor.Save_Riders ();
		if (save_flag) System_File_Handle (NEW_RIDERSHIP)->Create ();
		Write_Ridership ();
	}

	//---- save the link persons ----

	if (link_person_flag) {
		part_processor.Save_Persons ();
		if (save_flag) link_person_file.Create ();
		Write_Link_Delays (link_person_file, link_person_array);
	}

	//---- save the link vehicles ----

	if (link_vehicle_flag) {
		part_processor.Save_Vehicles ();
		if (save_flag) link_vehicle_file.Create ();
		Write_Link_Delays (link_vehicle_file, link_vehicle_array);
	}

	//---- gather summary statistics ----

	if (trip_flag && trip_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			trip_file->Add_Counters (trip_set [i]);
			trip_set [i]->Close ();
		}
		if (trip_file->Num_Files () == 0) trip_file->Num_Files (num_file_sets);
	}
	if (plan_flag && plan_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			plan_file->Add_Counters (plan_set [i]);
			plan_set [i]->Close ();
		}
		if (plan_file->Num_Files () == 0) plan_file->Num_Files (num_file_sets);
	}
	if (new_plan_flag && plan_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			new_plan_file->Add_Counters (new_plan_set [i]);
			new_plan_set [i]->Close ();
		}
		if (new_plan_file->Num_Files () == 0) new_plan_file->Num_Files (num_file_sets);
	}
	if (problem_set_flag) {
		for (int i=0; i < num_file_sets; i++) {
			problem_file->Add_Counters (problem_set [i]);
			problem_set [i]->Close ();
		}
		if (problem_file->Num_Files () == 0) problem_file->Num_Files (num_file_sets);
	}

	//---- collect data from MPI slaves ----

	MPI_Processing ();

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LINK_GAP:			//---- Link Gap Report ----
				if (iteration_flag) Link_Gap_Report ();
				break;
			case TRIP_GAP:			//---- Trip Gap Report ----
				if (iteration_flag) Trip_Gap_Report ();
				break;
			default:
				break;
		}
	}

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
		if (num_reskim > 0) Print (1,  "Number of Re-Skimmed Plans     = ") << num_reskim;
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
//	Page_Header
//---------------------------------------------------------

void Router::Page_Header (void)
{
	switch (Header_Number ()) {
		case LINK_GAP:			//---- Link Gap Report ----
			Link_Gap_Header ();
			break;
		case TRIP_GAP:			//---- Trip Gap Report ----
			Trip_Gap_Header ();
			break;
		default:
			break;
	}
}

