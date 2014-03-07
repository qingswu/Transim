//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TransitNet::Program_Control (void)
{
	int i, low, high;
	String key;
	Strings parts;
	Str_Itr str_itr;

	//---- check for a park & ride file ----

	if (Check_Control_Key (PARK_AND_RIDE_FILE)) {
		parkride_flag = true;
		System_File_True (PARKING);
		System_File_True (NEW_PARKING);
	} else {
		System_Option_False (PARKING);
		System_Option_False (NEW_PARKING);
	}

	//---- open network files ----

	Data_Service::Program_Control ();

	access_flag = System_File_Flag (NEW_ACCESS_LINK);
	new_link_flag = System_File_Flag (NEW_LINK);

	//---- set processing flags ----

	Link_File *link_file = (Link_File *) System_File_Handle (LINK);

	at_flag = !link_file->Area_Type_Flag () && System_File_Flag (ZONE);

	Route_Nodes_File *nodes_file = (Route_Nodes_File *) System_File_Handle (ROUTE_NODES);

	dwell_flag = nodes_file->Dwell_Flag ();
	time_flag = nodes_file->Time_Flag ();
	speed_flag = nodes_file->Speed_Flag ();
	
	Print (2, String ("%s Control Keys:") % Program ());

	//---- route data file ----

	key = Get_Control_String (ROUTE_DATA_FILE);

	if (!key.empty ()) {
		route_data_file.File_Type ("Route Data File");
		route_data_flag = true;

		//---- get the file format ----

		if (Check_Control_Key (ROUTE_DATA_FORMAT)) {
			route_data_file.Dbase_Format (Get_Control_String (ROUTE_DATA_FORMAT));
		}
		route_data_file.Open (Project_Filename (key));

		//---- route join field ----

		if (Check_Control_Key (ROUTE_JOIN_FIELD)) {
			key = Get_Control_Text (ROUTE_JOIN_FIELD);

			route_join_field = route_data_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (route_join_field + 1));
		} else {
			route_data_field = route_data_file.Required_Field (ROUTE_FIELD_NAMES);
		}

		Line_File *file = (Line_File *) System_File_Handle (NEW_TRANSIT_ROUTE);
		Field_Ptr fld;

		for (i=0; i < route_data_file.Num_Fields (); i++) {
			if (i == route_data_field) continue;

			fld = route_data_file.Field (i);
			if (file->Field_Number (fld->Name ()) >= 0) continue;

			file->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units ());
			route_data_array.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units ());
		}
		file->Write_Header ();
	}

	//---- open the park and ride file ----

	if (parkride_flag) {
		key = Get_Control_String (PARK_AND_RIDE_FILE);

		parkride_file.File_Type ("Park and Ride File");

		parkride_file.Open (Project_Filename (key));

		//---- PNR access distance ----

		PNR_distance = Get_Control_Integer (PNR_ACCESS_DISTANCE);
		Print (1);
	}

	//---- station access distance ----

	station_distance = Get_Control_Integer (STATION_ACCESS_DISTANCE);

	//---- get the min spacing length ----
	
	Get_Control_List_Groups (STOP_SPACING_BY_AREA_TYPE, min_stop_spacing);

	//---- get the stop facility type range ----

	key = Get_Control_Text (STOP_FACILITY_TYPE_RANGE);

	if (key.empty ()) {
		key = "PRINCIPAL..FRONTAGE";
	}
	key.Parse (parts, COMMA_DELIMITERS);

	for (str_itr = parts.begin (); str_itr != parts.end (); str_itr++) {

		if (!Type_Range (*str_itr, FACILITY_CODE, low, high)) {
			Error (String ("Facility Type Range %s is Illegal") % *str_itr);
		}
		for (i=low; i <= high; i++) {
			facility_flag [i] = true;
		}
	}

	//---- get the time periods ----

	key = Get_Control_Text (TRANSIT_TIME_PERIODS);

	if (!key.empty ()) {
		schedule_periods.Add_Breaks (key);
	}
	num_periods = schedule_periods.Num_Periods ();

	Route_Nodes_File *fh = (Route_Nodes_File *) System_File_Handle (ROUTE_NODES);

	if (fh->Num_Periods () != num_periods) {
		Error (String ("Transit Time Periods = %d do not match Route Nodes Periods = %d") % num_periods % fh->Num_Periods ());
	}

	//---- set the time factors ----

	Get_Control_List_Groups (TRANSIT_TRAVEL_TIME_FACTORS, time_factor);

	//---- minimum dwell time ----

	min_dwell = Get_Control_Time (MINIMUM_DWELL_TIME);

	//---- intersection stop type ----

	stop_type = Stop_Code (Get_Control_Text (INTERSECTION_STOP_TYPE));

	//---- intersection stop offset ----

	stop_offset = Round (Get_Control_Double (INTERSECTION_STOP_OFFSET));

	if (stop_type == FARSIDE && stop_offset < Round (Internal_Units (10, METERS))) {
		Warning ("Farside Stops and Stop Offset are Incompatible");
	}

	//---- coordinate schedules -----

	coordinate_flag = Get_Control_Flag (COORDINATE_SCHEDULES);

	//---- ignore path errors -----

	ignore_errors_flag = Get_Control_Flag (IGNORE_PATH_ERRORS);

	//---- new route change file ----

	key = Get_Control_String (NEW_ROUTE_CHANGE_FILE);

	if (!key.empty ()) {
		change_file.File_Type ("New Route Change File");
		change_flag = true;

		//---- get the file format ----

		if (Check_Control_Key (NEW_ROUTE_CHANGE_FORMAT)) {
			change_file.Dbase_Format (Get_Control_String (NEW_ROUTE_CHANGE_FORMAT));
		}
		change_route_fld = change_file.Add_Field ("ROUTE", DB_INTEGER, 10);
		change_mode_fld = change_file.Add_Field ("MODE", DB_STRING, 16, TRANSIT_CODE);
		change_name_fld = change_file.Add_Field ("NAME", DB_STRING, 20);
		change_in_len_fld = change_file.Add_Field ("IN_LENGTH", DB_INTEGER, 10, FEET);
		change_out_len_fld = change_file.Add_Field ("OUT_LENGTH", DB_INTEGER, 10, FEET);
		change_in_stops_fld = change_file.Add_Field ("IN_STOPS", DB_INTEGER, 10);
		change_out_stops_fld = change_file.Add_Field ("OUT_STOPS", DB_INTEGER, 10);

		change_file.Create (Project_Filename (key));
	}

	//---- write the report names ----

	List_Reports ();

	//---- read the zone equiv ----

	equiv_flag = Zone_Equiv_Flag ();

	if (equiv_flag) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
} 
