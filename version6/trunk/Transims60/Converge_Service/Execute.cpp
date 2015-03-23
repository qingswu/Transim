//*********************************************************
//	Execute.cpp - path building service
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Converge_Service::Execute (void)
{
	if (!Control_Flag ()) {
		Error ("Converge_Service::Program_Control has Not Been Called");
	}

	//---- compile the type script ----

	if (script_flag) {
		if (Report_Flag (TRAVELER_SCRIPT)) {
			Header_Number (TRAVELER_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		Db_Base_Array files;

		files.push_back (System_File_Base (HOUSEHOLD));

		type_script.Initialize (files, random.Seed () + 1);

		if (!type_script.Compile (script_file, Report_Flag (TRAVELER_SCRIPT))) {
			Error ("Compiling Traveler Type Script");
		}
		if (Report_Flag (TRAVELER_STACK)) {
			Header_Number (TRAVELER_STACK);

			type_script.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- initialize path building ----

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
		if (trip_flag) {
			Input_Trips ();
		}
		Initialize_Trip_Gap ();
	} else if (save_trip_gap && plan_memory_flag) {
		Initialize_Trip_Gap ();
	}

	//---- convert to internal location numbers ----

	if (capacity_flag) {
		Loc_Cap_Itr cap_itr;
		Int_Map_Itr map_itr;
		Int_Itr itr;

		for (cap_itr = loc_cap_array.begin (); cap_itr != loc_cap_array.end (); cap_itr++) {
			for (itr = cap_itr->next_des.begin (); itr != cap_itr->next_des.end (); itr++) {
				map_itr = location_map.find (*itr);

				if (map_itr == location_map.end ()) {
					*itr = -1;
				} else {
					*itr = map_itr->second;
				}
			}
		}
	}

	//---- convert to internal location numbers ----

	if (fuel_flag) {
		int i, fuel_cap;
		Plan_Itr plan_itr;
		Veh_Type_Data *veh_type_ptr;

		initial_fuel.assign (plan_array.size (), 100);

		for (i=0, plan_itr = plan_array.begin (); plan_itr != plan_array.end (); plan_itr++, i++) {
			if (plan_itr->Veh_Type () >= 0) {
				veh_type_ptr = &veh_type_array [plan_itr->Veh_Type ()];
				fuel_cap = veh_type_ptr->Fuel_Cap ();
				if (fuel_cap == 0) fuel_cap = 100;

				initial_fuel [i] = DTOI (((100.0 - minimum_fuel) * random.Probability () + minimum_fuel) * fuel_cap / 100.0);
			}
		}
	}
	if (time_order_flag) {
		initial_time_map = plan_time_map;
	}
	Print (1);

	first_iteration = !(System_File_Flag (PLAN) && System_File_Flag (PERFORMANCE));
}

//---------------------------------------------------------
//	Print_Reports
//---------------------------------------------------------

void Converge_Service::Print_Reports (void)
{
	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LINK_GAP:			//---- Link Gap Report ----
				if (iteration_flag) Link_Gap_Report (LINK_GAP);
				break;
			case TRIP_GAP:			//---- Trip Gap Report ----
				if (iteration_flag) Trip_Gap_Report (TRIP_GAP);
				break;
			default:
				break;
		}
	}
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Converge_Service::Page_Header (void)
{
	switch (Header_Number ()) {
		case TRAVELER_SCRIPT:		//---- Type Script ----
			Print (1, "Traveler Type Script");
			Print (1);
			break;
		case TRAVELER_STACK:		//---- Type Stack ----
			Print (1, "Traveler Type Stack");
			Print (1);
			break;
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
