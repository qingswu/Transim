//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PathSkim::Program_Control (void)
{
	int value;
	String key;

	if (!Set_Control_Flag (UPDATE_FLOW_RATES) && !Set_Control_Flag (UPDATE_TRAVEL_TIMES)) {
		System_File_False (NEW_PERFORMANCE);
	}

	//---- create the network files ----

	Router_Service::Program_Control ();

	//---- check the output files ----

	if (System_File_Flag (ZONE)) {
		zone_flag = true;
		zone_file = System_Zone_File ();
	}
	zone_loc_flag = Zone_Loc_Flag ();

	if (System_File_Flag (NEW_SKIM)) {
		skim_flag = true;
		skim_file = System_Skim_File (true);

		zone_skim_flag = (skim_file->OD_Units () == ZONE_OD);
		district_flag = (skim_file->OD_Units () == DISTRICT_OD);

		Skim_Total_Time (skim_file->Total_Time_Flag ());
	}
	if (System_File_Flag (NEW_PLAN)) {
		plan_flag = true;
		plan_file = System_Plan_File (true);
	}
	if (System_File_Flag (NEW_PROBLEM)) {
		problem_flag = true;
		problem_file = System_Problem_File (true);
		problem_file->Router_Data ();
	}
	if (!skim_flag && !plan_flag && !problem_flag && !System_File_Flag (NEW_PERFORMANCE)) {
		Error ("No Output Files are Specified");
	}
	if (zone_skim_flag && (plan_flag || problem_flag)) {
		Error ("Zone Skims are Incompatible with Plan and Problem File Output");
	}
	if (district_flag && (plan_flag || problem_flag)) {
		Error ("District Skims are Incompatible with Plan and Problem File Output");
	}
	Print (2, String ("%s Control Keys:") % Program ());

	//---- route from specified locations -----

	key = Get_Control_Text (ROUTE_FROM_SPECIFIED_LOCATIONS);

	if (!key.empty ()) {
		if (!key.Equals ("ALL")) {
			if (!org_range.Add_Ranges (key)) {
				Error ("Adding Origin Ranges");
			}
			select_org = true;
		}
	}

	//---- route to specified locations -----

	key = Get_Control_Text (ROUTE_TO_SPECIFIED_LOCATIONS);

	if (!key.empty ()) {
		if (!key.Equals ("ALL")) {
			if (!des_range.Add_Ranges (key)) {
				Error ("Adding Destination Ranges");
			}
			select_des = true;
		}
	}

	//---- route at specified times -----

	key = Get_Control_Text (ROUTE_AT_SPECIFIED_TIMES);

	if (!key.empty ()) {

		//---- route by time increments -----

		route_periods.Increment (Get_Control_Time (ROUTE_BY_TIME_INCREMENT));

		if (!key.Equals ("ALL")) {
			route_periods.Add_Ranges (key);

			if (skim_flag && !skim_file->Range_Flag () && 
				skim_file->Num_Periods () <= 1 && skim_file->End () == Model_End_Time ()) {
				skim_file->Add_Ranges (key);
			}
		}
		select_time = true;
	}

	//---- route with time constraint ----
	
	key = Get_Control_Text (ROUTE_WITH_TIME_CONSTRAINT);

	if (!key.empty ()) {
		constraint = Constraint_Code (key);
	} else {
		constraint = NO_CONSTRAINT;
	}
	forward_flag = (constraint != END_TIME);

	//---- route with specified mode ----
	
	key = Get_Control_Text (ROUTE_WITH_SPECIFIED_MODE);

	if (!key.empty ()) {
		mode_flag = true;

		new_mode = Mode_Code (key);

		if (new_mode < 1 || new_mode >= MAX_MODE) {
			Error (String ("Specified Mode %d is Out of Range (1..%d)") % new_mode % (MAX_MODE-1));
		}
		if (key [0] >= '0' && key [0] <= '9') {
			Print (0, String (" (%s)") % Mode_Code ((Mode_Type) new_mode));
		}
		if (skim_flag) {
			if (skim_file->Skim_Mode () != new_mode) {
				skim_file->Skim_Mode ((Mode_Type) new_mode);
				skim_file->Create_Fields ();
				skim_file->Write_Header ();
			}
			if (skim_file->Skim_Mode () == TRANSIT_MODE) {
				if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) || 
					!System_File_Flag (TRANSIT_SCHEDULE) || !System_File_Flag (TRANSIT_DRIVER)) {
					Error ("Transit Network Files are Required for Transit Skims");
				}
			}
		}
	} else {
		mode_flag = false;
	}

	//---- route with use restriction ----
	
	key = Get_Control_Text (ROUTE_WITH_USE_RESTRICTION);

	if (!key.empty ()) {
		use_type = Veh_Use_Code (key);

		if (key [0] >= '0' && key [0] <= '9') {
			Print (0, String (" (%s)") % Veh_Use_Code ((Use_Type) use_type));
		}
	}

	//---- route using vehicle type ----
	
	key = Get_Control_Text (ROUTE_USING_VEHICLE_TYPE);

	if (!key.empty ()) {
		veh_type = key.Integer ();

		if (veh_type == 0) veh_type = -1;
	}

	//---- route using traveler type ----
	
	key = Get_Control_Text (ROUTE_USING_TRAVELER_TYPE);

	if (!key.empty ()) {
		traveler_type = key.Integer ();
	}

	//---- route from specified zones -----

	key = Get_Control_Text (ROUTE_FROM_SPECIFIED_ZONES);

	if (!key.empty ()) {
		if (!key.Equals ("ALL")) {
			if (!org_zone_range.Add_Ranges (key)) {
				Error ("Adding Origin Zone Ranges");
			}
			sel_org_zone = true;
		}
	}

	//---- route to specified zones -----

	key = Get_Control_Text (ROUTE_TO_SPECIFIED_ZONES);

	if (!key.empty ()) {
		if (!key.Equals ("ALL")) {
			if (!des_zone_range.Add_Ranges (key)) {
				Error ("Adding Destination Zone Ranges");
			}
			sel_des_zone = true;
		}
	}

	//---- origin locations per zone -----

	num_org = Get_Control_Integer (ORIGIN_LOCATIONS_PER_ZONE);

	//---- desination locations per zone ----

	num_des = Get_Control_Integer (DESTINATION_LOCATIONS_PER_ZONE);

	//---- location selection method ----

	if (num_org > 0 || num_des > 0 || Check_Control_Key (LOCATION_SELECTION_METHOD)) {
		method = Loc_Method_Code (Get_Control_Text (LOCATION_SELECTION_METHOD));

		if (method == USER_LOC) {
			user_loc_flag = true;
		} else if (method == RANDOM_LOC) {
			random_loc.Seed (random.Seed () + 1);
		} else if (!zone_flag) {
			Error ("A Zone File is needed for Location Selection");
		}
	}

	//---- origin zone file ----

	if (!sel_org_zone) {
		key = Get_Control_String (ORIGIN_ZONE_FILE);

		if (!key.empty ()) {
			Print (1);
			Db_File file;
			file.File_Type ("Origin Zone File");

			if (!file.Open (Project_Filename (key))) {
				File_Error ("Opening Origin Zone File", key);
			}
			while (file.Read ()) {
				value = file.Record_String ().Integer ();

				if (value > 0 && !org_zone_range.Add_Ranges (file.Record_String ())) {
					Error ("Adding Origin Zone Value");
				}
			}
			file.Close ();
			sel_org_zone = true;
		}
	}

	//---- destination zone file ----

	if (!sel_des_zone) {
		key = Get_Control_String (DESTINATION_ZONE_FILE);

		if (!key.empty ()) {
			Print (1);
			Db_File file;
			file.File_Type ("Destination Zone File");

			if (!file.Open (Project_Filename (key))) {
				File_Error ("Opening Destination Zone File", key);
			}
			while (file.Read ()) {
				value = file.Record_String ().Integer ();

				if (value > 0 && !des_zone_range.Add_Ranges (file.Record_String ())) {
					Error ("Adding Destination Zone Value");
				}
			}
			file.Close ();
			sel_des_zone = true;
		}
	}
	if (method != RANDOM_LOC && !user_loc_flag && (sel_org_zone || sel_des_zone) && !zone_flag) {
		Error ("A Zone File is needed for Zone Skimming");
	}

	//---- open the origin location file ----

	if (!select_org || user_loc_flag) {
		key = Get_Control_String (ORIGIN_LOCATION_FILE);

		if (!key.empty ()) {
			Print (1);
			user_org_file.File_Type ("Origin Location File");

			if (!user_org_file.Open (Project_Filename (key))) {
				File_Error ("Opening Origin Location File", key);
			}
			if (!user_loc_flag) {
				while (user_org_file.Read ()) {
					value = user_org_file.Record_String ().Integer ();
					if (value == 0) continue;

					if (!org_range.Add_Ranges (user_org_file.Record_String ())) {
						Error ("Adding Origin Location Value");
					}
				}
				user_org_file.Close ();
				select_org = true;
			}
		} else if (user_loc_flag) {
			Error ("An Origin Location File is Required for User Selection Method");
		}
	}

	//---- open the destination location file ----

	if (!select_des || user_loc_flag) {
		key = Get_Control_String (DESTINATION_LOCATION_FILE);

		if (!key.empty ()) {
			Print (1);
			user_des_file.File_Type ("Destination Location File");

			if (!user_des_file.Open (Project_Filename (key))) {
				File_Error ("Opening Destination Location File", key);
			}
			if (!user_loc_flag) {
				while (user_des_file.Read ()) {
					value = user_des_file.Record_String ().Integer ();
					if (value == 0) continue;

					if (!des_range.Add_Ranges (user_des_file.Record_String ())) {
						Error ("Adding Destination Location Value");
					}
				}
				user_des_file.Close ();
				select_des = true;
			}
		} else if (user_loc_flag) {
			Error ("A Destination Location File is Required for User Selection Method");
		}
	}

	if (zone_skim_flag || district_flag) {

		//---- open the new origin location file ----

		if (!select_org) {
			key = Get_Control_String (NEW_ORIGIN_LOCATION_FILE);

			if (!key.empty ()) {
				Print (1);
				org_loc_file.File_Type ("New Origin Location File");

				if (!org_loc_file.Create (Project_Filename (key))) {
					File_Error ("Creating New Origin Location File", key);
				}
				org_loc_flag = true;
			}
		}

		//---- open the new destination location file ----

		if (!select_des) {
			key = Get_Control_String (NEW_DESTINATION_LOCATION_FILE);

			if (!key.empty ()) {
				Print (1);
				des_loc_file.File_Type ("New Destination Location File");

				if (!des_loc_file.Create (Project_Filename (key))) {
					File_Error ("Creating New Destination Location File", key);
				}
				des_loc_flag = true;
			}
		}
	}

	//---- skim transit load factor ----

	Print (1);
	load_factor = Get_Control_Double (SKIM_TRANSIT_LOAD_FACTOR);

	if (load_factor > 0) {
		if (!System_File_Flag (RIDERSHIP)) {
			Error ("A Ridership file is Required for Load Factor Processing");
		}
		Skim_Check_Flag (true);
	}

	//---- open the new accessibility file ----

	key = Get_Control_String (NEW_ACCESSIBILITY_FILE);

	if (!key.empty ()) {
		Print (1);
		accessibility_file.File_Type ("New Accessibility File");

		if (Check_Control_Key (NEW_ACCESSIBILITY_FORMAT)) {
			accessibility_file.Dbase_Format (Get_Control_String (NEW_ACCESSIBILITY_FORMAT));
		}
		accessibility_file.Add_Field ("ZONE", DB_INTEGER, 10);
		accessibility_file.Add_Field ("ORG_WT", DB_DOUBLE, 12.2);
		accessibility_file.Add_Field ("DES_WT", DB_DOUBLE, 12.2);

		if (!accessibility_file.Create (Project_Filename (key))) {
			File_Error ("Creating New Accessibility File", key);
		}
		accessibility_flag = true;
		if (!zone_flag) {
			Error ("A Zone File is Required for Accessibility Processing");
		}
		if (!zone_skim_flag) {
			Error ("Accessibility Requires Zone Skims");
		}

		//---- origin weight field ----

		key = Get_Control_Text (ORIGIN_WEIGHT_FIELD);

		if (!key.empty ()) {
			org_wt_fld = zone_file->Field_Number (key);
			if (org_wt_fld < 0) {
				Error (String ("Origin Weight Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (org_wt_fld + 1);
		}

		//---- destination weight field ----

		key = Get_Control_Text (DESTINATION_WEIGHT_FIELD);

		if (!key.empty ()) {
			des_wt_fld = zone_file->Field_Number (key);
			if (des_wt_fld < 0) {
				Error (String ("Destination Weight Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (des_wt_fld + 1);
		}

		//---- maximum travel time ----

		max_travel_time = Get_Control_Time (MAXIMUM_TRAVEL_TIME);
	}

	//---- read report types ----

	List_Reports ();

	if (Zone_Equiv_Flag ()) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}

	//---- update the performance controls ----

	if (System_File_Flag (NEW_PERFORMANCE)) {
		Link_Flows (Flow_Updates ());
	}
}
