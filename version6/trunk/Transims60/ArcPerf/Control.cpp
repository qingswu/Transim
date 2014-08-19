//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcPerf.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcPerf::Program_Control (void)
{
	int i, nfield, n, num, type;
	String key, ext;
	Strings result;
	Str_Itr str_itr;
	double dvalue;
	bool binary, ignore_flag, perf_in, turn_in, z_flag, m_flag;

	Db_Field *fld;
	Image_Data image_rec;

	//---- open network files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();

	if (Control_Key_Status (BANDWIDTH_FIELD)) {
		bandwidth_flag = !(Get_Control_String (BANDWIDTH_FIELD)).empty ();
	}
	perf_in = System_File_Flag (PERFORMANCE);
	turn_in = System_File_Flag (TURN_DELAY);
	index_flag = Set_Control_Flag (ADD_LINK_DIRECTION_INDEX);
	ignore_flag = Set_Control_Flag (IGNORE_TIME_RANGE_FIELDS);
	event_flag = Check_Control_Key (SYSTEM_EVENT_FILE);

	//---- get the projection data ----

	projection.Read_Control ();

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();

	//---- get the link datafile ----

	key = Get_Control_String (LINK_DATA_FILE);

	if (!key.empty ()) {
		link_data_flag = true;

		Print (1);
		link_data_file.Open (Project_Filename (key));
	}

	//---- get the link direction file ----

	key = Get_Control_String (LINK_DIRECTION_FILE);

	if (!key.empty ()) {
		link_dir_flag = true;
		link_dir_file.Ignore_Periods (ignore_flag);

		Print (1);
		link_dir_file.Open (Project_Filename (key));

		if (ignore_flag) {
			Print (1, "Time Range Fields are Ignored");
		}
	}
	if (!perf_in && !link_data_flag && !link_dir_flag) {
		Error ("A Performance, Link Data, or Link Direction file are Required");
	}

	//---- open the system event file ----

	if (event_flag) {
		key = Get_Control_String (SYSTEM_EVENT_FILE);

		//Print (1);
		//event_file.Open (Project_Filename (key));

		//if (!Network_File_Flag (PHASING_PLAN)) {
		//	Error ("A Phasing Plan file is required for System Event Processing");
		//}
	}

	//---- get the arcview link data file ----

	key = Get_Control_String (NEW_ARC_LINK_DATA_FILE);

	if (!key.empty ()) {
		if (!link_data_flag) {
			Error ("The Input Link Data File is Missing");
		}
		Print (1);
		arcview_link_data.File_Type ("New Arc Link Data File");
		arcview_link_data.File_Access (CREATE);
		if (bandwidth_flag) {
			arcview_link_data.Shape_Type (POLYGON);
		} else {
			arcview_link_data.Shape_Type (VECTOR);
		}
		arcview_link_data.Z_Flag (z_flag);
		arcview_link_data.M_Flag (m_flag);

		link_period_flag = Set_Control_Flag (LINK_DATA_BY_PERIOD);

		if (link_period_flag) {
			fld = link_data_file.Field (3); 
			arcview_link_data.Add_Field ("LINK", DB_INTEGER, 10);
			arcview_link_data.Add_Field ("ANODE", DB_INTEGER, 10);
			arcview_link_data.Add_Field ("BNODE", DB_INTEGER, 10);
			if (link_data_file.Lane_Use_Flows ()) {
				arcview_link_data.Add_Field ("TYPE", DB_INTEGER, 1);
			}
			arcview_link_data.Add_Field ("AB", fld->Type (), fld->Size ());
			arcview_link_data.Add_Field ("BA", fld->Type (), fld->Size ());
		} else {
			arcview_link_data.Replicate_Fields (&link_data_file);
		}
		arcview_link_data.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		if (link_period_flag) {
		
			//---- set the file name ----

			key.Split_Last (ext, ".");

			linkname = key = Project_Filename (key);
			key = key + "_time_of_day.shp";

			Print_Filename (2, arcview_link_data.File_Type (), key);

			Get_Control_Flag (LINK_DATA_BY_PERIOD);

		} else {

			if (!arcview_link_data.Open (Project_Filename (key))) {
				File_Error ("Opening ArcView Link Data File", arcview_link_data.Shape_Filename ());
			}
			arcview_link_data.Write_Header ();
		}

	} else if (link_data_flag) {
		Error ("The Output Arcview Link Data File is Missing");
	}

	//---- get the arcview link dir file ----

	key = Get_Control_String (NEW_ARC_LINK_DIR_FILE);

	if (!key.empty ()) {
		if (!link_dir_flag) {
			Error ("The Input Link Direction File is Missing");
		}
		Print (1);
		arcview_link_dir.File_Type ("New Arc Link Dir File");
		arcview_link_dir.File_Access (CREATE);
		if (bandwidth_flag) {
			arcview_link_dir.Shape_Type (POLYGON);
		} else {
			arcview_link_dir.Shape_Type (VECTOR);
		}
		arcview_link_dir.Z_Flag (z_flag);
		arcview_link_dir.M_Flag (m_flag);

		dir_period_flag = Set_Control_Flag (LINK_DIR_BY_PERIOD);

		if (index_flag) {
			arcview_link_dir.Add_Field ("INDEX", DB_INTEGER, 10);
		}

		arcview_link_dir.Replicate_Fields (&link_dir_file);

		arcview_link_dir.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		if (!arcview_link_dir.Open (Project_Filename (key))) {
			File_Error ("Opening ArcView Link Dir File", arcview_link_dir.Shape_Filename ());
		}
		arcview_link_dir.Write_Header ();

		if (index_flag) {
			Print (1, "Link Direction Index Field was Added");
		}
	} else if (link_dir_flag) {
		Error ("The Output Arcview Link Dir File is Missing");
	}

	//---- get the arcview performance file ----

	key = Get_Control_String (NEW_ARC_PERFORMANCE_FILE);

	if (!key.empty ()) {
		if (!perf_in) {
			Error ("The Input Performance File is Missing");
		}
		perf_flag = true;

		key.Split_Last (ext, ".");

		shapename = Project_Filename (key);

		arcview_perf.File_Type ("New Arc Performance File");
		arcview_perf.File_Access (CREATE);
		if (bandwidth_flag) {
			arcview_perf.Shape_Type (POLYGON);
		} else {
			arcview_perf.Shape_Type (VECTOR);
		}
		arcview_perf.Z_Flag (z_flag);
		arcview_perf.M_Flag (m_flag);

		//---- copy the link fields ----

		perf_file = (Performance_File *) System_File_Handle (PERFORMANCE);

		binary = (perf_file->Record_Format () == BINARY);
		nfield = perf_file->Num_Fields ();

		if (index_flag) {
			arcview_perf.Add_Field ("INDEX", DB_INTEGER, 10);
		}

		for (i=0; i < nfield; i++) {
			fld = perf_file->Field (i);
			if (fld != 0) {
				arcview_perf.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary, NO_NEST);
			}
		}

		//---- add calculated performance fields ----

		length_field = arcview_perf.Add_Field ("LENGTH", DB_DOUBLE, 8.1, FEET);
		arcview_perf.Set_Units (length_field, FEET);

		lane_len_field = arcview_perf.Add_Field ("LANE_LEN", DB_DOUBLE, 8.3, MILES);
		arcview_perf.Set_Units (lane_len_field, MILES);

		vc_field = arcview_perf.Add_Field ("VC_RATIO", DB_DOUBLE, 6.2, Performance_Units_Map (VC_DATA));
		cong_time_field = arcview_perf.Add_Field ("CONG_TIME", DB_DOUBLE, 14.2, Performance_Units_Map (CONG_TIME_DATA));
		cong_vmt_field = arcview_perf.Add_Field ("CONG_VMT", DB_DOUBLE, 14.2, Performance_Units_Map (CONG_VMT_DATA));
		cong_vht_field = arcview_perf.Add_Field ("CONG_VHT", DB_DOUBLE, 14.2, Performance_Units_Map (CONG_VHT_DATA));

		arcview_perf.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		//---- maximum period field ----

		key = Get_Control_String (PERIOD_MAXIMUM_FIELD);

		if (!key.empty ()) {
			max_period_field = arcview_perf.Required_Field (key);
			max_period_flag = true;
		}

		//---- set the file name ----

		if (max_period_flag) {
			Print (1);
			key = shapename + "_MAX.shp";

			arcview_perf.Create (key);

			Get_Control_Text (PERIOD_MAXIMUM_FIELD);
		} else {
			key = shapename + "_time_of_day.shp";

			Print_Filename (2, arcview_perf.File_Type (), key);

			if (index_flag) {
				Print (1, "Link Direction Index Field was Added");
			}
		}
	}

	//---- get the arcview intersection file ----

	key = Get_Control_String (NEW_ARC_INTERSECTION_FILE);

	if (!key.empty ()) {
		if (!turn_in) {
			Error ("The Input Turn Delay File is Missing");
		}
		intersection_flag = true;

		arcview_node.File_Type ("New Arc Intersection File");
		arcview_node.File_Access (CREATE);
		arcview_node.Shape_Type (DOT);
		arcview_node.Z_Flag (z_flag);
		arcview_node.M_Flag (m_flag);

		//---- copy the turn delay fields ----

		turn_file = (Turn_Delay_File *) System_File_Handle (TURN_DELAY);

		binary = (turn_file->Record_Format () == BINARY);
		nfield = turn_file->Num_Fields ();
			
		arcview_node.Add_Field ("NODE", DB_INTEGER, 10);

		for (i=0; i < nfield; i++) {
			fld = turn_file->Field (i);
			if (fld != 0) {
				if (fld->Name ().Equals ("LINK") || fld->Name ().Equals ("NODE") ||
					fld->Name ().Equals ("DIR") || fld->Name ().Equals ("LANE")) continue;

				arcview_node.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary, NO_NEST);
			}
		}
		arcview_node.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		//---- set the file name ----

		key.Split_Last (ext, ".");

		nodename = key = Project_Filename (key);
		key = key + "_time_of_day.shp";

		Print_Filename (2, arcview_node.File_Type (), key);

		if (index_flag) {
			Print (1, "Link Direction Index Field was Added");
		}
		Print_Filename (2, arcview_node.File_Type (),key);
	}

	//---- get the arcview turn delay file ----

	key = Get_Control_String (NEW_ARC_TURN_DELAY_FILE);

	if (!key.empty ()) {
		if (!turn_in) {
			Error ("The Input Turn Delay File is Missing");
		}
		if (!System_File_Flag (CONNECTION)) {
			Error ("Turning Delays required a Connection File");
		}

		//---- initialize the turn file ----

		arcview_turn.File_Type ("New Arc Turn File");
		arcview_turn.File_Access (CREATE);
		arcview_turn.Shape_Type (VECTOR);
		arcview_turn.Z_Flag (z_flag);
		arcview_turn.M_Flag (m_flag);

		turn_file = (Turn_Delay_File *) System_File_Handle (TURN_DELAY);
		
		binary = (turn_file->Record_Format () == BINARY);
		nfield = turn_file->Num_Fields ();

		for (i=0; i < nfield; i++) {
			fld = turn_file->Field (i);
			if (fld == 0) continue;
			arcview_turn.Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary, NO_NEST);
		}
		if (event_flag) {
			arcview_turn.Add_Field ("GREEN", DB_INTEGER, 6);
			arcview_turn.Add_Field ("YELLOW", DB_INTEGER, 6);
			arcview_turn.Add_Field ("RED", DB_INTEGER, 6);
			arcview_turn.Add_Field ("SPLIT", DB_INTEGER, 4);

			green_field = arcview_turn.Field_Number ("GREEN");
			yellow_field = arcview_turn.Field_Number ("YELLOW");
			red_field = arcview_turn.Field_Number ("RED");
			split_field = arcview_turn.Field_Number ("SPLIT");
		}
		arcview_turn.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		key.Split_Last (ext, ".");

		turnname = key = Project_Filename (key);
		key = key + "_time_of_day.shp";

		Print_Filename (2, arcview_turn.File_Type (), key);
		turn_flag = true;

	} else {
		if (event_flag) {
			Error ("System Event File Processing required an ArcView Turn File");
		}
		//System_File_False (PHASING_PLAN);
	}

	//---- get the arcview delay file ----
		
	num = Highest_Control_Group (NEW_ARC_TRAFFIC_IMAGE, 0);

	for (n=1; n <= num; n++) {
		key = Get_Control_String (NEW_ARC_TRAFFIC_IMAGE, n);

		if (key.empty ()) continue;

		if (!perf_in) {
			Error ("The Input Performance File is Missing");
		}
		image_flag = true;
		image_rec.number = n;
		image_rec.file = new Arcview_File (CREATE);
		image_rec.file->File_Type (String ("New Arc Traffic Image #%d") % n);
		image_rec.file->Shape_Type ((shape_flag) ? POLYGON : DOT);
		image_rec.file->Z_Flag (z_flag);
		image_rec.file->M_Flag (m_flag);

		//---- set the file name ----

		key.Split_Last (ext, ".");

		image_rec.name = key = Project_Filename (key);
		key = key + "_time_of_day.shp";

		Print_Filename (2, image_rec.file->File_Type (), key);

		//---- copy the link fields ----

		perf_file = (Performance_File *) System_File_Handle (PERFORMANCE);

		binary = (perf_file->Record_Format () == BINARY);
		nfield = perf_file->Num_Fields ();

		for (i=0; i < nfield; i++) {
			fld = perf_file->Field (i);
			if (fld != 0) {
				image_rec.file->Add_Field (fld->Name (), fld->Type (), fld->Size (), fld->Units (), binary, NO_NEST);
			}
		}
		image_rec.file->Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

		//---- image attribute ----

		key = Get_Control_Text (TRAFFIC_IMAGE_ATTRIBUTE, n);

		if (!key.empty ()) {
			Error ("Traffic Image Attribute was Not Found");
		}
		image_rec.type = type = Performance_Code (key);

		if (type != FLOW_DATA && type != DENSITY_DATA && 
			type != MAX_DENSITY_DATA && type != QUEUE_DATA &&
			type != MAX_QUEUE_DATA && type != FAILURE_DATA) {

			Error (String ("Traffic Image Attribute %s is Not an Image Option") % key);
		}
		image_array.push_back (image_rec);
	}

	//---- check support file flags ----

	if (image_flag) {
		if (!System_File_Flag (VEHICLE_TYPE)) {
			Error ("A Vehicle Type File is required for Traffic Images");
		}
		if (!System_File_Flag (POCKET)) {
			Error ("A Pocket Lane File is required for Traffic Images");
		}
		if (!System_File_Flag (LANE_USE)) {
			Write (2, "Warning: A Lane Use File is desirable for Traffic Images");
			Show_Message (1);
		}

		//---- vehicle type distribution ----
		
		key = Get_Control_String (VEHICLE_TYPE_DISTRIBUTION);

		Print (2, "Vehicle Type Distribution = ");

		key.Parse (result);

		for (str_itr = result.begin (); str_itr != result.end (); str_itr++) {
			dvalue = str_itr->Double ();

			if (dvalue < 0.0 || dvalue > 100.0) {
				Error (String ("Vehicle Type Distribution Value %.1lf is Out of Range (0..100)") % dvalue);
			}
			Print (0, String (" %.1lf ") % dvalue);

			type_share.push_back (dvalue);
		}
		type_share.push_back (100.0);
		Print (0, "percent");

	} else {
		System_File_False (POCKET);
		System_File_False (LANE_USE);
		System_File_False (VEHICLE_TYPE);
	}

	//---- draw lanes ----

	Read_Draw_Keys ();

	if (bandwidth_flag) {
		if (link_data_flag) {
			width_field = arcview_link_data.Required_Field (bandwidth_field);
		} else if (link_dir_flag) {
			width_field = arcview_link_dir.Required_Field (bandwidth_field);
		} else if (perf_flag) {
			width_field = arcview_perf.Required_Field (bandwidth_field);
		} else {
			Error ("Bandwidths require an Arcview Link or Performance File");
		}
	}
}
