//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void RoutePrep::Program_Control (void)
{
	String key, in_format, out_format;
	int i, j, num;

	Integer_List list;
	Int_Itr itr;
	String_List str_list;
	Str_Itr str_itr;

	in_route_flag = (Highest_Control_Group (INPUT_ROUTE_FILE, 0) > 0);
	out_route_flag = (Highest_Control_Group (NEW_ROUTE_FILE, 0) > 0);

	if (!in_route_flag || !out_route_flag) {
		new_route_flag = true;
		System_File_True (NEW_ROUTE_NODES);
	}

	//---- create the network files ----

	Data_Service::Program_Control ();

	//---- get the projection data ----

	proj_service.Read_Control ();

	projection.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

	Projection_Data output = proj_service.Output_Projection ();

	if (output.Type () != NO_PROJECTION && 
		((output.Units () != METERS && Metric_Flag ()) || 
		(output.Units () != FEET && !Metric_Flag ()))) {
	
		Error ("Projection Output and Units of Measure are Incompatible");
	}
	shape_flag = System_File_Flag (NEW_SHAPE);

	Print (2, String ("%s Control Keys:") % Program ());	
	
	new_link_file = (Link_File *) System_File_Base (NEW_LINK);

	//---- transit route data ----

	if (new_route_flag) {
		new_route_nodes = System_Route_Nodes_File (true);
		new_route_nodes->File_ID ("NewRoute");
	}

	//---- first node number ----

	new_node = Get_Control_Integer (FIRST_NODE_NUMBER);

	//---- first link number ----

	new_link = Get_Control_Integer (FIRST_LINK_NUMBER);

	//---- first route number ----

	new_route = Get_Control_Integer (FIRST_ROUTE_NUMBER);

	//---- convert node numbers ----

	if (Check_Control_Key (CONVERT_NODE_NUMBERS)) Print (1);

	convert_node_flag = Get_Control_Flag (CONVERT_NODE_NUMBERS);

	if (convert_node_flag) {
		
		//---- check for link and node files ----

		if (!System_File_Flag (NODE) || !System_File_Flag (LINK)) {
			Error ("A Link and Node File are Required for Node Conversion");
		}

		//---- get the input node file ----

		key = Get_Control_String (INPUT_NODE_FILE);

		if (!key.empty ()) {
			input_node_flag = true;
			input_node_file.File_Type ("Input Node File");

			//---- get the file format ----

			if (Check_Control_Key (INPUT_NODE_FORMAT)) {
				input_node_file.Dbase_Format (Get_Control_String (INPUT_NODE_FORMAT));
			}
			input_node_file.Open (Project_Filename (key));
		}

		//---- node map file ----

		key = Get_Control_String (NODE_MAP_FILE);

		if (!key.empty ()) {
			node_map_flag = true;
			node_map_file.File_Type ("Node Map File");
			node_map_file.Open (Project_Filename (key));
		}

		if (!input_node_flag && !node_map_flag) {
			Error ("An Input Node File and/or a Node Map File are Required");
		}

		//---- new node map file ----

		key = Get_Control_String (NEW_NODE_MAP_FILE);

		if (!key.empty ()) {
			new_map_flag = true;
			new_map_file.File_Type ("New Node Map File");
			new_map_file.Create (Project_Filename (key));
		}
	}

	if (System_File_Flag (ROUTE_NODES) && new_route_flag) {
		Copy_File_Header (ROUTE_NODES, NEW_ROUTE_NODES);

		num_periods = new_route_nodes->Num_Periods ();
		offset_flag = new_route_nodes->Offset_Flag ();
		time_flag = new_route_nodes->TTime_Flag ();

	} else {

		//---- get the time periods ----

		num_periods = transit_time_periods.Num_Periods ();
	}

	//---- coordinate resolution ----

	resolution = Scale (Get_Control_Double (COORDINATE_RESOLUTION));

	//---- open the conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (!key.empty ()) {
		Print (1);
		script_file.File_Type ("Conversion Script");

		script_file.Open (Project_Filename (key));
		convert_flag = true;
	}
	Print (1);

	//---- route shape file ----
		
	num = Highest_Control_Group (ROUTE_SHAPE_FILE, 0);

	if (num > 0) {
		for (i=1; i <= num; i++) {
			key = Get_Control_String (ROUTE_SHAPE_FILE, i);

			if (key.empty ()) continue;

			if (!key.Ends_With (".shp")) {
				Error ("Route Shape File is not an ArcView Shapefile");
			}
			route_shape_file = new Arcview_File ();

			shape_file_array.push_back (route_shape_file);

			//---- set the projection data ----

			route_shape_file->Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

			route_shape_file->File_Type (String ("Route Shape File #%d") % i);
			route_shape_file->File_ID ("Route");
			route_shape_flag = true;

			route_shape_file->Open (Project_Filename (key));
		}

		//---- route name field ----

		Print (1);
		key = Get_Control_Text (ROUTE_NAME_FIELD);

		if (!key.empty ()) {
			route_name_field = route_shape_file->Required_Field (key);

			Print (0, String (" (Number = %d)") % (route_name_field + 1));
		}

		//---- route link field ----

		key = Get_Control_Text (ROUTE_LINK_FIELD);

		if (!key.empty ()) {
			route_link_field = route_shape_file->Required_Field (key);

			Print (0, String (" (Number = %d)") % (route_link_field + 1));
		}

		//---- route order field ----

		key = Get_Control_Text (ROUTE_ORDER_FIELD);

		if (!key.empty ()) {
			route_order_field = route_shape_file->Required_Field (key);

			Print (0, String (" (Number = %d)") % (route_order_field + 1));
		}

		//---- determine the number of headway fields ----

		num = Highest_Control_Group (ROUTE_HEADWAY_FIELD, 0);

		if (num > 0) {
			if (num > num_periods) {
				Warning ("Number of Headway Fields is Greater than Time Periods");
				num = num_periods;
			}
			route_headway_flds.assign (num, -1);

			for (i=0; i < num; i++) {
				key = Get_Control_Text (ROUTE_HEADWAY_FIELD, i+1);
				if (!key.empty ()) {
					j = route_shape_file->Required_Field (key);
					Print (0, String (" (Number = %d)") % (j + 1));

					route_headway_flds [i] = j;
				}
			}
		} else {
			num = Highest_Control_Group (ROUTE_FREQUENCY_FIELD, 0);

			if (num > 0) {
				if (num > num_periods) {
					Warning ("Number of Frequency Fields is Greater than Time Periods");
					num = num_periods;
				}
				route_headway_flds.assign (num, -1);
				route_freq_flag = true;

				for (i=0; i < num; i++) {
					key = Get_Control_Text (ROUTE_FREQUENCY_FIELD, i+1);
					if (!key.empty ()) {
						j = route_shape_file->Required_Field (key);
						Print (0, String (" (Number = %d)") % (j + 1));

						route_headway_flds [i] = j;
					}
				}
			}
		}

		//---- route link length ----

		link_length = Get_Control_Double (ROUTE_LINK_LENGTH);
		
		//---- route stop file ----

		key = Get_Control_String (ROUTE_STOP_FILE);

		if (!key.empty ()) {
			Print (1);
			route_stop_file.File_Type ("Route Stop File");

			//---- get the file format ----

			if (Check_Control_Key (ROUTE_STOP_FORMAT)) {
				route_stop_file.Dbase_Format (Get_Control_String (ROUTE_STOP_FORMAT));
			}
			route_stop_file.Open (Project_Filename (key));
			route_stop_flag = true;

			//---- route stop route field ----

			key = Get_Control_Text (ROUTE_STOP_ROUTE_FIELD);

			rstop_route_field = route_stop_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (rstop_route_field + 1));

			//---- route stop stop field ----

			key = Get_Control_Text (ROUTE_STOP_STOP_FIELD);

			rstop_stop_field = route_stop_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (rstop_stop_field + 1));

			//---- route stop offset field ----

			key = Get_Control_Text (ROUTE_STOP_OFFSET_FIELD);

			if (!key.empty ()) {
				rstop_offset_field = route_stop_file.Required_Field (key);

				Print (0, String (" (Number = %d)") % (rstop_offset_field + 1));
			}

			//---- route stop order field ----

			key = Get_Control_Text (ROUTE_STOP_ORDER_FIELD);

			if (!key.empty ()) {

				rstop_order_field = route_stop_file.Required_Field (key);

				Print (0, String (" (Number = %d)") % (rstop_order_field + 1));
			}
		}

	} else {

		//---- station node file ----

		key = Get_Control_String (STATION_NODE_FILE);

		if (!key.empty ()) {
			Print (1);
			station_file.File_Type ("Station Node File");

			//---- get the file format ----

			if (Check_Control_Key (STATION_NODE_FORMAT)) {
				station_file.Dbase_Format (Get_Control_String (STATION_NODE_FORMAT));
			}
			station_file.Open (Project_Filename (key));
			station_flag = true;

			//---- station node field ----

			key = Get_Control_Text (STATION_NODE_FIELD);

			if (!key.empty ()) {
				station_node_field = station_file.Required_Field (key);

				Print (0, String (" (Number = %d)") % (station_node_field + 1));
			}

			//---- station name field ----

			key = Get_Control_Text (STATION_NAME_FIELD);

			if (!key.empty ()) {
				station_name_field = station_file.Required_Field (key);

				Print (0, String (" (Number = %d)") % (station_name_field + 1));
			}

			//---- station x field ----

			key = Get_Control_Text (STATION_X_FIELD);

			if (!key.empty ()) {
				station_x_field = station_file.Required_Field (key);

				Print (0, String (" (Number = %d)") % (station_x_field + 1));
			}

			//---- station y field ----

			key = Get_Control_Text (STATION_Y_FIELD);

			if (!key.empty ()) {
				station_y_field = station_file.Required_Field (key);

				Print (0, String (" (Number = %d)") % (station_y_field + 1));
			}

			if (station_node_field < 0 && (station_x_field < 0 || station_y_field < 0)) {
				Error ("Station Node or XY Fields are Required");
			}

			//---- station link length ----

			station_length = Get_Control_Double (STATION_LINK_LENGTH);

			//---- station platform time ----

			platform_time = Get_Control_Time (STATION_PLATFORM_TIME);

			if (platform_time > 0) {
				platform_speed = Scale (station_length) / platform_time;
			}

			//---- station platform flag ----

			platform_flag = Get_Control_Flag (STATION_PLATFORM_FLAG);

			if (platform_flag) {

				//---- station platform offset ----

				platform_offset = Get_Control_Double (STATION_PLATFORM_OFFSET);
			}
		}
	}

	//---- input route files ----

	if (in_route_flag) {
		File_Group group_rec;

		segment_report = Report_Flag (SEGMENT_NODES);

		//---- determine the number of file groups ----

		num = Highest_Control_Group (INPUT_ROUTE_FILE, 0);

		if (num != 0) {

			in_format = Get_Control_String (INPUT_ROUTE_FORMAT, 1);
			out_format = Get_Control_String (NEW_ROUTE_FORMAT, 1);

			//---- read the input route files ----

			for (i=1; i <= num; i++) {

				//---- open the input route file ----
			
				key = Get_Control_String (INPUT_ROUTE_FILE, i);
				if (key.empty ()) continue;

				group_rec.group = i;
				group_rec.in_file = new Route_Nodes_File ();
				group_rec.period_map.assign (num_periods, 0);
				group_rec.out_file = new Route_Nodes_File ();

				Print (1);
				group_rec.in_file->File_Type (String ("Input Route File #%d") % i);

				//---- input route format ----

				if (Check_Control_Key (INPUT_ROUTE_FORMAT, i)) {
					group_rec.in_file->Dbase_Format (Get_Control_String (INPUT_ROUTE_FORMAT, i));
				} else {
					group_rec.in_file->Dbase_Format (in_format);
				}
				group_rec.in_file->Open (Project_Filename (key));	

				//---- process the period map ----

				if (!Get_Control_List (ROUTE_PERIOD_MAP, list, i)) {
					Error ("Route Period Map was Not Found");
				}
				itr = list.begin ();

				for (j=0, ++itr;  itr != list.end (); itr++, j++) {
					if (j < num_periods) {
						group_rec.period_map [j] = *itr;
					}
				}

				//---- vehicle type ----

				group_rec.veh_type = Get_Control_Integer (ROUTE_VEHICLE_TYPE, i);

				//---- output route file ----

				key = Get_Control_String (NEW_ROUTE_FILE, i);
				if (!key.empty ()) {
					group_rec.out_file->File_Type (String ("New Route File #%d") % i);

					//---- new route format ----

					if (Check_Control_Key (NEW_ROUTE_FORMAT, i)) {
						group_rec.out_file->Dbase_Format (Get_Control_String (NEW_ROUTE_FORMAT, i));
					} else {
						group_rec.out_file->Dbase_Format (out_format);
					}
					group_rec.out_file->Create (Project_Filename (key));
				}

				//---- save the record ----

				file_groups.push_back (group_rec);
			}
		}

		//---- route speed file ----

		key = Get_Control_String (ROUTE_SPEED_FILE);

		if (!key.empty ()) {
			Print (1);
			speed_file.File_Type ("Route Speed File");

			//---- get the file format ----

			if (Check_Control_Key (ROUTE_SPEED_FORMAT)) {
				speed_file.Dbase_Format (Get_Control_String (ROUTE_SPEED_FORMAT));
			}
			speed_file.Open (Project_Filename (key));
			route_speed_flag = true;

			//---- speed route field ----

			key = Get_Control_Text (SPEED_ROUTE_FIELD);

			speed_route_field = speed_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (speed_route_field + 1));

			//---- speed peak field ----

			key = Get_Control_Text (SPEED_PEAK_FIELD);

			speed_peak_field = speed_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (speed_peak_field + 1));

			//---- speed offpeak field ----

			key = Get_Control_Text (SPEED_OFFPEAK_FIELD);

			speed_offpeak_field = speed_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (speed_offpeak_field + 1));
		}

		//---- segment speed factor file ----

		key = Get_Control_String (SEGMENT_SPEED_FACTOR_FILE);

		if (!key.empty ()) {
			Print (1);
			seg_fac_file.File_Type ("Segment Speed Factor File");

			//---- get the file format ----

			if (Check_Control_Key (SEGMENT_SPEED_FACTOR_FORMAT)) {
				seg_fac_file.Dbase_Format (Get_Control_String (SEGMENT_SPEED_FACTOR_FORMAT));
			}
			seg_fac_file.Open (Project_Filename (key));
			seg_fac_flag = true;

			//---- segment field ----

			key = Get_Control_Text (SEGMENT_FIELD);

			segment_field = seg_fac_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (segment_field + 1));

			//---- speed factor field ----

			key = Get_Control_Text (SPEED_FACTOR_FIELD);

			speed_fac_field = seg_fac_file.Required_Field (key);

			Print (0, String (" (Number = %d)") % (speed_fac_field + 1));

			ignore_speeds = Get_Control_Flag (IGNORE_EXISTING_SPEEDS);
		}
	}

	//---- transcad route system ----

	key = Get_Control_String (TRANSCAD_ROUTE_SYSTEM);

	if (!key.empty ()) {
		Db_File rts_file;
		String name, filename;

		Print (1);
		rts_file.File_Type ("TransCAD Route System");

		filename = Project_Filename (key);
		tcad_route_flag = true;

		if (filename.Ends_With (".rts")) {
			if (filename.size () > 4 && filename [filename.size () - 4] == '.') {
				filename.erase (filename.size () - 4, 4);
			}
		}

		//---- route header file ----

		name = filename + "r.bin";

		tcad_route_header.File_Type ("TransCAD Route Header");
		tcad_route_header.Dbase_Format ("TRANSCAD:BINARY");
		tcad_route_header.Open (name);

		//---- route line file ----

		name = filename + "l.bin";
		tcad_route_link.File_Type ("TransCAD Route Link");
		tcad_route_link.Dbase_Format ("TRANSCAD:BINARY");
		tcad_route_link.Open (name);

		//---- stop node file ----

		name = filename + "s.bin";
		tcad_stop_node.File_Type ("TransCAD Stop Node");
		tcad_stop_node.Dbase_Format ("TRANSCAD:BINARY");
		tcad_stop_node.Open (name);
	}
}
