//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void NewFormat::Program_Control (void)
{
	bool type_flag, flat_flag, time_flag;
	String key;

	//---- open the support files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();

	if (Check_Control_Key (OLD_PLAN_FILE) && Check_Control_Key (TRIP_SORT_TYPE)) {
		Warning ("Plan Sorting is Limited to Version 6 Plan files");
		Show_Message (1);
	}	
	Print (2, String ("%s Control Keys:") % Program ());

	//---- copy existing fields ----

	copy_flag = Get_Control_Flag (COPY_EXISTING_FIELDS);

	if (copy_flag) {
		if (System_File_Flag (LOCATION) && System_File_Flag (NEW_LOCATION)) {
			Location_File *file = (Location_File *) System_File_Handle (LOCATION);
			new_loc_file = (Location_File *) System_File_Handle (NEW_LOCATION);
			new_loc_file->Add_User_Fields (file);
		}
		if (System_File_Flag (ZONE) && System_File_Flag (NEW_ZONE)) {
			Zone_File *file = (Zone_File *) System_File_Handle (ZONE);
			new_zone_file = (Zone_File *) System_File_Handle (NEW_ZONE);
			new_zone_file->Add_User_Fields (file);
		}
	}
	new_plan_flag = System_File_Flag (NEW_PLAN);
	new_trip_flag = System_File_Flag (NEW_TRIP);

	//---- flatten output flag ----

	flat_flag = Get_Control_Flag (FLATTEN_OUTPUT_FLAG);

	if (flat_flag) {
		if (System_File_Flag (NEW_SHAPE)) {
			System_File_Header (NEW_SHAPE)->Flatten_File ();
		}
		if (System_File_Flag (NEW_PARKING)) {
			System_File_Header (NEW_PARKING)->Flatten_File ();
		}
		if (System_File_Flag (NEW_SIGNAL)) {
			System_File_Header (NEW_SIGNAL)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TIMING_PLAN)) {
			System_File_Header (NEW_TIMING_PLAN)->Flatten_File ();
		}
		if (System_File_Flag (NEW_PHASING_PLAN)) {
			System_File_Header (NEW_PHASING_PLAN)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TRANSIT_ROUTE)) {
			System_File_Header (NEW_TRANSIT_ROUTE)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TRANSIT_SCHEDULE)) {
			System_File_Header (NEW_TRANSIT_SCHEDULE)->Flatten_File ();
		}
		if (System_File_Flag (NEW_TRANSIT_DRIVER)) {
			System_File_Header (NEW_TRANSIT_DRIVER)->Flatten_File ();
		}
		if (System_File_Flag (NEW_ROUTE_NODES)) {
			System_File_Header (NEW_ROUTE_NODES)->Flatten_File ();
		}
		if (System_File_Flag (NEW_HOUSEHOLD)) {
			System_File_Header (NEW_HOUSEHOLD)->Flatten_File ();
		}
		if (new_plan_flag) {
			System_File_Header (NEW_PLAN)->Flatten_File ();
		}
	}

	//---- toll file ----

	key = Get_Control_String (TOLL_FILE);

	if (!key.empty ()) {
		toll_file.Open (Project_Filename (key));
		toll_flag = true;
	}

	//---- activity file ----

	key = Get_Control_String (ACTIVITY_FILE);

	if (!key.empty ()) {
		activity_file.Open (Project_Filename (key));
		activity_flag = true;

		if (!new_trip_flag) {
			Error ("A New Trip file is required to convert an Activity file");
		}
		if (!System_File_Flag (VEHICLE_TYPE)) {
			Error ("A Vehicle Type file is required to convert an Activity file");
		}
	}

	//---- person file ----

	key = Get_Control_String (PERSON_FILE);

	if (!key.empty ()) {
		person_file.Open (Project_Filename (key));
		person_flag = true;
	}

	//---- vehicle file ----

	key = Get_Control_String (VEHICLE_FILE);

	if (!key.empty ()) {
		vehicle_file.Open (Project_Filename (key));
		vehicle_flag = true;
	} else {
		if (activity_flag) {
			Error ("A Vehicle file is required to convert an Activity file");
		} else if (new_trip_flag && System_File_Flag (TRIP)) {
			Trip_File *file = (Trip_File *) System_File_Handle (TRIP);

			if (file->Version () < 40) {
				Error ("A Vehicle file is required to convert an old Trip file");

				if (!System_File_Flag (VEHICLE_TYPE)) {
					Error ("A Vehicle Type file is required to convert an old Trip file");
				}
			}
		}
	}

	//---- snapshot file ----

	key = Get_Control_String (SNAPSHOT_FILE);

	if (!key.empty ()) {
		Print (1);
		snap_file.Open (Project_Filename (key));
		snap_flag = true;
	}

	//---- new snapshot file ----

	key = Get_Control_String (NEW_SNAPSHOT_FILE);

	if (!key.empty ()) {
		new_snap_file.Compress_Flag (Set_Control_Flag (NEW_SNAPSHOT_COMPRESSION));

		if (new_snap_file.Compress_Flag ()) {
			new_snap_file.Dbase_Format (BINARY);
		} else {
			if (Check_Control_Key (NEW_SNAPSHOT_FORMAT)) {
				new_snap_file.Dbase_Format (Get_Control_String (NEW_SNAPSHOT_FORMAT));
			}
			new_snap_file.Location_Flag (snap_file.Location_Flag ());
			new_snap_file.Cell_Flag (snap_file.Cell_Flag ());
			new_snap_file.Status_Flag (snap_file.Status_Flag ());
		}
		new_snap_file.Create (Project_Filename (key));

		Get_Control_Flag (NEW_SNAPSHOT_COMPRESSION);
	} else if (snap_flag) {
		Error ("A New Snapshot File is required for Output");
	}

	//---- old time format ----

	time_units = Units_Code (Get_Control_Text (OLD_TIME_FORMAT));
	time_flag = Check_Control_Key (OLD_TIME_FORMAT);

	if (time_flag) {
		int num;
		Field_Ptr fld_ptr;

		if (System_File_Flag (TRIP)) {
			Trip_File *file = (Trip_File *) System_File_Base (TRIP);
			if (file->Version () <= 40) {
				num = file->Optional_Field (START_FIELD_NAMES);
				if (num >= 0) {
					fld_ptr = file->Field (num);
					fld_ptr->Units (time_units);
				}
				num = file->Optional_Field (END_FIELD_NAMES);
				if (num >= 0) {
					fld_ptr = file->Field (num);
					fld_ptr->Units (time_units);
				}
				num = file->Optional_Field (DURATION_FIELD_NAMES);
				if (num >= 0) {
					fld_ptr = file->Field (num);
					fld_ptr->Units (time_units);
				}
			}
		}
		if (activity_flag) {
			num = activity_file.Optional_Field ("START_MIN", "START", "START_TIME", "STARTTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("START_MAX", "START", "START_TIME", "STARTTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("END_MIN", "END", "END_TIME", "ENDTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("END_MAX", "END", "END_TIME", "ENDTIME");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("TIME_MIN", "DURATION");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
			num = activity_file.Optional_Field ("TIME_MAX", "DURATION");
			if (num >= 0) {
				fld_ptr = activity_file.Field (num);
				fld_ptr->Units (time_units);
			}
		}
	}

	//---- plan file ----

	if (Check_Control_Key (OLD_PLAN_FILE)) {
		if (System_File_Flag (PLAN)) {
			Error ("A Plan file and old Plan file must be processed separately");
		}
		if (!System_File_Flag (NEW_PLAN)) {
			Error ("A new Plan file is required to convert an old Plan file");
		}
		if (!vehicle_flag) {
			Error ("A Vehicle file is required to convert an old Plan file");
		}
		if (!System_File_Flag (VEHICLE_TYPE)) {
			Error ("A Vehicle Type file is required to convert an old Plan files");
		}
		Print (1);
		plan_flag = true;
		old_plan.File_Type ("Old Plan File");

		//---- get the list type ----

		key = Get_Control_String (NODE_LIST_PATHS);

		if (!key.empty ()) {
			type_flag = key.Bool ();
		} else {
			type_flag = true;
		}
		old_plan.Node_Based_Flag (type_flag);

		//---- get the traveler scale ----

		key = Get_Control_String (TRAVELER_SCALING_FACTOR);

		if (!key.empty ()) {
			scale = key.Integer ();

			if (scale < 2 || scale > 100) {
				Control_Key_Error (TRAVELER_SCALING_FACTOR, "(2..100)");
			}
			old_plan.Traveler_Scale (scale);
		}

		//---- get the file format ----

		if (Check_Control_Key (OLD_PLAN_FORMAT)) {
			old_plan.File_Format (Get_Control_String (OLD_PLAN_FORMAT));
		}

		//---- open the file and print the parameters ----

		if (!old_plan.Open (Project_Filename (Get_Control_String (OLD_PLAN_FILE)))) {
			File_Error ("Opening Old Plan File", old_plan.Filename ());
		}
		Get_Control_Text (OLD_PLAN_FORMAT);

		type_flag = old_plan.Node_Based_Flag ();

		if (type_flag) {
			Print (1, "Plan File contains Node List Paths");
			if (!System_File_Flag (NODE) || !System_File_Flag (LINK) || !System_File_Flag (PARKING)) {
				Error ("A Node, Link and Parking Files are Required for Node-Based Plan Files");
			}
			AB_Map_Flag (true);
		} else {
			Print (1, "Plan File contains Link List Paths");
		}
		Get_Control_Integer (TRAVELER_SCALING_FACTOR);

	} else if (snap_flag && snap_file.Version () <= 40) {

		//---- get the traveler scale ----

		scale= Get_Control_Integer (TRAVELER_SCALING_FACTOR);

		if (scale < 2 || scale > 100) {
			Control_Key_Error (TRAVELER_SCALING_FACTOR, "(2..100)");
		}
	}

	//---- Old Route Header and Node files ----

	if (Check_Control_Key (OLD_ROUTE_HEADER_FILE)) {
		if (!System_File_Flag (NEW_ROUTE_NODES)) {
			Error ("A New Route Nodes File is Required");
		}
		int i, num;
		String field;
		Field_Ptr fld_ptr;

		if (!time_flag) time_units = MINUTES;

		Print (1);
		route_flag = true;

		Route_Nodes_File *file = (Route_Nodes_File *) System_File_Base (NEW_ROUTE_NODES);

		key = Get_Control_String (OLD_ROUTE_HEADER_FILE);

		route_header.File_Type ("Old Route Header File");
		route_header.File_ID ("Header");

		route_header.Open (Project_Filename (key));

		for (i=1; i <= 24; i++) {
			field ("HEADWAY_%d") % i;
			num = route_header.Optional_Field (field.c_str ());
			if (num < 0) break;

			fld_ptr = route_header.Field (num);
			fld_ptr->Units (time_units);

			field ("OFFSET_%d") % i;
			num = route_header.Optional_Field (field.c_str ());
			if (num >= 0) {
				fld_ptr = route_header.Field (num);
				fld_ptr->Units (time_units);
			}
			field ("TTIME_%d") % i;
			num = route_header.Optional_Field (field.c_str ());
			if (num >= 0) {
				fld_ptr = route_header.Field (num);
				fld_ptr->Units (time_units);
			}
		}
		route_periods = i - 1;
		if (route_periods != file->Num_Periods ()) {
			file->Num_Periods (route_periods);
			file->Clear_Fields ();
			file->Create_Fields ();
			file->Write_Header ();
		}
		key = Get_Control_String (OLD_ROUTE_NODES_FILE);

		if (key.empty ()) {
			Error ("A Old Route Nodes File is Required");
		}
		route_nodes.File_Type ("Old Route Nodes File");
		route_nodes.File_ID ("Nodes");

		route_nodes.Open (Project_Filename (key));

	} else if (System_File_Flag (NEW_ROUTE_NODES)) {
		new_route_flag = true;

		Route_Nodes_File *file = (Route_Nodes_File *) System_File_Base (NEW_ROUTE_NODES);

		if (System_File_Flag (ROUTE_NODES)) {
			Route_Nodes_File *in_file = (Route_Nodes_File *) System_File_Base (ROUTE_NODES);

			file->Num_Periods (in_file->Num_Periods ());
			file->TTime_Flag (in_file->TTime_Flag ());
			file->Pattern_Flag (in_file->Pattern_Flag ());
			file->Dwell_Flag (in_file->Dwell_Flag ());
			file->Time_Flag (in_file->Time_Flag ());
			file->Speed_Flag (in_file->Speed_Flag ());
			file->Type_Flag (in_file->Type_Flag ());

			file->Clear_Fields ();
			file->Create_Fields ();
			file->Write_Header ();
		} else if (sum_periods.Num_Periods () != file->Num_Periods ()) {
			file->Num_Periods (sum_periods.Num_Periods ());
			file->TTime_Flag (false);
			file->Pattern_Flag (false);
			file->Dwell_Flag (false);
			file->Time_Flag (false);
			file->Speed_Flag (false);
			file->Type_Flag (false);

			file->Clear_Fields ();
			file->Create_Fields ();
			file->Write_Header ();
		}
	}

	//---- link delay file ----

	if (Check_Control_Key (OLD_LINK_DELAY_FILE)) {
		if (System_File_Flag (PERFORMANCE) || Check_Control_Key (OLD_PERFORMANCE_FILE)) {
			Error ("A Performance and Link Delay file must be processed separately");
		}
		if (!System_File_Flag (NEW_PERFORMANCE)) {
			Error ("A new Performance file is required to convert a Old Link Delay file");
		}
		old_delay_file.Open (Project_Filename (Get_Control_String (OLD_LINK_DELAY_FILE)));
		old_delay_flag = true;
	} else {

		//---- old perf file ----

		if (Check_Control_Key (OLD_PERFORMANCE_FILE)) {
			if (System_File_Flag (PERFORMANCE)) {
				Error ("A Performance and Old Performance file must be processed separately");
			}
			if (!System_File_Flag (NEW_PERFORMANCE)) {
				Error ("A new Performance file is required to convert a Old Performance file");
			}
			old_delay_file.Open (Project_Filename (Get_Control_String (OLD_PERFORMANCE_FILE)));
			old_delay_flag = true;
		} else if (System_File_Flag (NEW_PERFORMANCE) && !System_File_Flag (PERFORMANCE)) {
			Error ("A Performance file is required for creating a New Performance file");
		}
	}
}
