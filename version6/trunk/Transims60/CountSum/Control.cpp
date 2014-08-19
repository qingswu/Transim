//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void CountSum::Program_Control (void)
{
	int i, num;
	String key;
	Strings strings;
	Str_Itr str_itr;
	Integers decimals;
	Data_Group data_rec, *data_ptr;
	Data_Itr data_itr;

	//---- create the network files ----

	Data_Service::Program_Control ();
		
	projection.Read_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	key = Get_Control_String (ARC_SIGNAL_FILE);

	if (!key.empty ()) {
		arc_signals.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
		arc_signal_flag = true;

		Print (1);
		arc_signals.File_Type ("Arc Signal File");

		arc_signals.Open (Project_Filename (key));

		//--- get the signal field name ----

		key = Get_Control_Text (SIGNAL_ID_FIELD);

		if (key.empty ()) {
			Error ("A Signal ID Field is Required for ArcGIS Processing");
		}
		signal_field = arc_signals.Field_Number (key);

		if (signal_field < 0) {
			Error (String ("Field %s was Not Found in Arc Signal File") % key);
		}
		Print (0, ", Number = ") << (signal_field + 1);

		//--- get the signal type field name ----

		key = Get_Control_Text (SIGNAL_TYPE_FIELD);

		if (!key.empty ()) {
			type_field = arc_signals.Field_Number (key);

			if (type_field < 0) {
				Error (String ("Field %s was Not Found in Arc Signal File") % key);
			}
			Print (0, ", Number = ") << (type_field + 1);
		}

		//--- get the select signal types ----

		key = Get_Control_Text (SELECT_SIGNAL_TYPES);

		if (!key.empty ()) {
            key.Parse (signal_types);
		}

		//---- primary street name ----

		key = Get_Control_Text (SIGNAL_PRIMARY_STREET);

		if (!key.empty ()) {
			primary_field = arc_signals.Field_Number (key);

			if (primary_field < 0) {
				Error (String ("Field %s was Not Found in Arc Signal File") % key);
			}
			Print (0, ", Number = ") << (primary_field + 1);
		}

		//---- cross street name ----

		key = Get_Control_Text (SIGNAL_CROSS_STREET);

		if (!key.empty ()) {
			cross_field = arc_signals.Field_Number (key);

			if (cross_field < 0) {
				Error (String ("Field %s was Not Found in Arc Signal File") % key);
			}
			Print (0, ", Number = ") << (cross_field + 1);
		}
	}

	//---- signal id map file ----

	key = Get_Control_String (SIGNAL_ID_MAP_FILE);

	if (!key.empty ()) {
		Print (1);
		signal_map_file.File_Type ("Signal ID Map File");
		signal_map_flag = true;

		if (Check_Control_Key (SIGNAL_ID_MAP_FORMAT)) {
			signal_map_file.Dbase_Format (Get_Control_String (SIGNAL_ID_MAP_FORMAT));
		}
		signal_map_file.Open (Project_Filename (key));

		//--- get the signal id map field name ----

		key = Get_Control_Text (SIGNAL_ID_MAP_FIELD);

		if (key.empty ()) {
			Error ("A Signal ID Map Field is Required for Signal ID Map Processing");
		}
		map_id_field = signal_map_file.Field_Number (key);

		if (map_id_field < 0) {
			Error (String ("Field %s was Not Found in Signal ID Map File") % key);
		}
		Print (0, ", Number = ") << (map_id_field + 1);

		//--- get the signal node field name ----

		key = Get_Control_Text (SIGNAL_NODE_FIELD);

		if (!key.empty ()) {
			node_field = signal_map_file.Field_Number (key);

			if (node_field < 0) {
				Error (String ("Field %s was Not Found in Signal ID Map File") % key);
			}
			Print (0, ", Number = ") << (node_field + 1);
		}
	}

	//---- detector map ----

	key = Get_Control_String (SIGNAL_DETECTOR_FILE);

	if (!key.empty ()) {
		Print (1);
		signal_detect_flag = true;
		detector_file.File_Type ("Signal Detector File");

		if (Check_Control_Key (SIGNAL_DETECTOR_FORMAT)) {
			detector_file.Dbase_Format (Get_Control_String (SIGNAL_DETECTOR_FORMAT));
		}
		detector_file.Open (Project_Filename (key));

		//--- get the device field name ----

		key = Get_Control_Text (DEVICE_ID_FIELD);

		if (!key.empty ()) {
			device_field = detector_file.Field_Number (key);

			if (device_field < 0) {
				Error (String ("Field %s was Not Found in Signal Detector File") % key);
			}
			Print (0, ", Number = ") << (device_field + 1);
		}

		//--- get the detector field name ----

		key = Get_Control_Text (DETECTOR_ID_FIELD);

		if (!key.empty ()) {
			detector_field = detector_file.Field_Number (key);

			if (detector_field < 0) {
				Error (String ("Field %s was Not Found in Signal Detector File") % key);
			}
			Print (0, ", Number = ") << (detector_field + 1);
		}

		//--- get the detector type field name ----

		key = Get_Control_Text (DETECTOR_TYPE_FIELD);

		if (!key.empty ()) {
			det_type_field = detector_file.Field_Number (key);

			if (det_type_field < 0) {
				Error (String ("Field %s was Not Found in Signal Detector File") % key);
			}
			Print (0, ", Number = ") << (det_type_field + 1);
		}

		//--- get the select detector types ----

		key = Get_Control_Text (SELECT_DETECTOR_TYPES);

		if (!key.empty ()) {
            key.Parse (detector_types, ",");
		}

		//---- detector name ----

		key = Get_Control_Text (DETECTOR_NAME_FIELD);

		if (!key.empty ()) {
			det_name_field = detector_file.Field_Number (key);

			if (det_name_field < 0) {
				Error (String ("Field %s was Not Found in Signal Detctor File") % key);
			}
			Print (0, ", Number = ") << (det_name_field + 1);
		}
	}

	//----- arc station file 

	key = Get_Control_String (ARC_STATION_FILE);

	if (!key.empty ()) {
		arc_stations.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
		arc_station_flag = true;

		Print (1);
		arc_stations.File_Type ("Arc Station File");

		arc_stations.Open (Project_Filename (key));

		//--- get the stationk ID field name ----

		key = Get_Control_Text (STATION_ID_FIELD);

		if (key.empty ()) {
			Error ("A Station ID Field is Required for ArcGIS Processing");
		}
		station_id_field = arc_stations.Field_Number (key);

		if (station_id_field < 0) {
			Error (String ("Field %s was Not Found in Arc Station File") % key);
		}
		Print (0, ", Number = ") << (station_id_field + 1);

		//--- get the station dir field name ----

		key = Get_Control_Text (STATION_DIR_FIELD);

		if (!key.empty ()) {
			station_dir_field = arc_stations.Field_Number (key);

			if (station_dir_field < 0) {
				Error (String ("Field %s was Not Found in Arc Station File") % key);
			}
			Print (0, ", Number = ") << (station_dir_field + 1);
		}

		//--- get the station offset field name ----

		key = Get_Control_Text (STATION_OFFSET_FIELD);

		if (!key.empty ()) {
			station_offset_field = arc_stations.Field_Number (key);

			if (station_offset_field < 0) {
				Error (String ("Field %s was Not Found in Arc Station File") % key);
			}
			Print (0, ", Number = ") << (station_offset_field + 1);
		}

		//--- get the station type field name ----

		key = Get_Control_Text (STATION_TYPE_FIELD);

		if (!key.empty ()) {
			station_type_field = arc_stations.Field_Number (key);

			if (station_type_field < 0) {
				Error (String ("Field %s was Not Found in Arc Station File") % key);
			}
			Print (0, ", Number = ") << (station_type_field + 1);
		}

		//--- get the select station types ----

		num = Highest_Control_Group (SELECT_STATION_TYPES);
		if (num == 0) {
			num = Highest_Control_Group (SELECT_FACILITY_TYPES);
		}
		if (num > 0) {
			num++;
			strings.clear ();
			decimals.clear ();

			station_types.assign (num, strings);
			facility_types.assign (num, decimals);

			for (i=1; i < num; i++) {

				key = Get_Control_Text (SELECT_STATION_TYPES, i);

				if (!key.empty ()) {
					key.Parse (station_types [i]);
				}

				//--- get the select facility types ----

				key = Get_Control_Text (SELECT_FACILITY_TYPES, i);

				if (!key.empty ()) {
					facility_types [i].assign (EXTERNAL+1, 0);

					key.Parse (strings);

					for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
						facility_types [i] [Facility_Code (*str_itr)] = 1;
					}
				} else {
					facility_types [i].assign (EXTERNAL+1, 1);
				}
			}
		}
	}

	//---- link id map file ----

	key = Get_Control_String (LINK_ID_MAP_FILE);

	if (!key.empty ()) {
		Print (1);
		link_map_file.File_Type ("Link ID Map File");
		link_map_flag = true;

		if (Check_Control_Key (LINK_ID_MAP_FORMAT)) {
			link_map_file.Dbase_Format (Get_Control_String (LINK_ID_MAP_FORMAT));
		}
		link_map_file.Open (Project_Filename (key));

		//--- get the link id map field name ----

		key = Get_Control_Text (LINK_ID_FIELD);

		if (key.empty ()) {
			Error ("A Link ID Field is Required for Link ID Map Processing");
		}
		link_id_field = link_map_file.Field_Number (key);

		if (link_id_field < 0) {
			Error (String ("Field %s was Not Found in Link ID Map File") % key);
		}
		Print (0, ", Number = ") << (link_id_field + 1);

		//--- get the link dir field name ----

		key = Get_Control_Text (LINK_DIR_FIELD);

		if (!key.empty ()) {
			link_dir_field = link_map_file.Field_Number (key);

			if (link_dir_field < 0) {
				Error (String ("Field %s was Not Found in Link ID Map File") % key);
			}
			Print (0, ", Number = ") << (link_dir_field + 1);
		}

		//--- get the dir offset field name ----

		key = Get_Control_Text (LINK_OFFSET_FIELD);

		if (!key.empty ()) {
			link_offset_field = link_map_file.Field_Number (key);

			if (link_offset_field < 0) {
				Error (String ("Field %s was Not Found in Link ID Map File") % key);
			}
			Print (0, ", Number = ") << (link_offset_field + 1);
		}
	}

	if (!arc_signal_flag && !signal_map_flag && !arc_station_flag && !link_map_flag) {
		Error ("An ArcGIS File or an ID Map Must be Provided");
	}
	if ((arc_signal_flag || signal_map_flag) && (arc_station_flag || link_map_flag)) {
		Error ("Simultaneous Signal and Link Processing is Not Supported");
	}
	method_flag = (arc_signal_flag || signal_map_flag);

	//---- max search distance ----

	Print (1);
	max_dist = Get_Control_Double (MAX_SEARCH_DISTANCE);

	//---- initialize the file list ----

	num = Highest_Control_Group (DATA_FILE, 0);
	if (num == 0) {
		Error ("No Data Files were Provided");
	}

	if (num > 0) {
		data_id_field = volume_field = speed_field = day_time_field = day_field = time_field = -1;

		//---- open each file ----

		for (i=1; i <= num; i++) {

			//---- open the input file ----

			key = Get_Control_String (DATA_FILE, i);

			if (key.empty ()) continue;

			data_rec.group = i;
			data_group.push_back (data_rec);

			data_ptr = &data_group.back ();
	
			Print (1);
			data_ptr->file = new Db_Header ();

			data_ptr->file->File_Type (String ("Data File #%d") % i);
			data_ptr->file->File_ID (String ("Data%d") % i);

			//---- get the file format ----

			if (Check_Control_Key (DATA_FORMAT, i)) {
				data_ptr->file->Dbase_Format (Get_Control_String (DATA_FORMAT, i));
			}
			data_ptr->file->Open (Project_Filename (key));

			if (data_ptr->file->Nest_Flag ()) {
				if (data_ptr->file->Num_Nest_Field () < 0) {
					Error ("A Nest Count field is Required for Nested Files");
				}
			}

			//---- get the id field -----

			key = Get_Control_Text (DATA_ID_FIELD, i);

			if (!key.empty ()) {
				data_ptr->id_field = data_ptr->file->Field_Number (key);

				if (data_ptr->id_field < 0) {
					Error (String ("Field %s was Not Found in Data File %d") % key % i);
				}
				Print (0, ", Number = ") << (data_ptr->id_field + 1);
				if (i == 1) data_id_field = data_ptr->id_field;
			} else {
				data_ptr->id_field = data_id_field;
			}

			//---- get the volume field -----

			key = Get_Control_Text (DATA_VOLUME_FIELD, i);

			if (!key.empty ()) {
				data_ptr->volume_field = data_ptr->file->Field_Number (key);

				if (data_ptr->volume_field < 0) {
					Error (String ("Field %s was Not Found in Data File %d") % key % i);
				}
				Print (0, ", Number = ") << (data_ptr->volume_field + 1);
				if (i == 1) volume_field = data_ptr->volume_field;
			} else {
				data_ptr->volume_field = volume_field;
			}

			//---- get the speed field -----

			key = Get_Control_Text (DATA_SPEED_FIELD, i);

			if (!key.empty ()) {
				data_ptr->speed_field = data_ptr->file->Field_Number (key);

				if (data_ptr->speed_field < 0) {
					Error (String ("Field %s was Not Found in Data File %d") % key % i);
				}
				Print (0, ", Number = ") << (data_ptr->speed_field + 1);
				if (i == 1) speed_field = data_ptr->speed_field;
			} else {
				data_ptr->speed_field = speed_field;
			}

			//---- get the day time field -----

			key = Get_Control_Text (DATA_DAY_TIME_FIELD, i);

			if (!key.empty ()) {
				data_ptr->day_time_field = data_ptr->file->Field_Number (key);

				if (data_ptr->day_time_field < 0) {
					Error (String ("Field %s was Not Found in Data File %d") % key % i);
				}
				Print (0, ", Number = ") << (data_ptr->day_time_field + 1);
				if (i == 1) day_time_field = data_ptr->day_time_field;
			} else {
				data_ptr->day_time_field = day_time_field;
			}

			//---- get the day field -----

			key = Get_Control_Text (DATA_DAY_FIELD, i);

			if (!key.empty ()) {
				data_ptr->day_field = data_ptr->file->Field_Number (key);

				if (data_ptr->day_field < 0) {
					Error (String ("Field %s was Not Found in Data File %d") % key % i);
				}
				Print (0, ", Number = ") << (data_ptr->day_field + 1);
				if (i == 1) day_field = data_ptr->day_field;
			} else {
				data_ptr->day_field = day_field;
			}

			//---- get the time field -----

			key = Get_Control_Text (DATA_TIME_FIELD, i);

			if (!key.empty ()) {
				data_ptr->time_field = data_ptr->file->Field_Number (key);

				if (data_ptr->time_field < 0) {
					Error (String ("Field %s was Not Found in Data File %d") % key % i);
				}
				Print (0, ", Number = ") << (data_ptr->time_field + 1);
				if (i == 1) time_field = data_ptr->time_field;
			} else {
				data_ptr->time_field = time_field;
			}

			//---- get the time format -----

			key = Get_Control_Text (DATA_TIME_FORMAT, i);
			time_format = Time_Format ();

			if (!key.empty ()) {
				data_ptr->time_format = Time_Code (key);

				if (i == 1) time_format = data_ptr->time_format;
			} else {
				data_ptr->time_format = time_format;
			}

			//--- get the select days ----

			key = Get_Control_Text (SELECT_DAYS, i);

			if (!key.empty ()) {
				key.Parse (data_ptr->select_days, ",");
			}

			//---- get the adjustment factor ----

			data_ptr->factor = Get_Control_Double (ADJUSTMENT_FACTOR, i);
		}
	}

	//---- output day data ----

	Print (1);
	day_flag = Get_Control_Flag (OUTPUT_DAY_DATA);

	if (day_flag && System_File_Flag (NEW_PERFORMANCE)) {
		Performance_File *file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);

		perf_name = file->Filename ();
		perf_name.Split_Last (perf_ext, ".");

		Print (2, String ("Performance Files by Day = %s_day.%s") % perf_name % perf_ext);
	}

	//---- output min-max data ----

	Print (1);
	min_max_flag = Get_Control_Flag (OUTPUT_MIN_MAX_DATA);

	if (min_max_flag && System_File_Flag (NEW_PERFORMANCE)) {
		Performance_File *file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);

		perf_name = file->Filename ();
		perf_name.Split_Last (perf_ext, ".");

		Print (2, String ("Minimum Performance File = %s_MIN.%s") % perf_name % perf_ext);
		Print (1, String ("Maximum Performance File = %s_MAX.%s") % perf_name % perf_ext);
	}

	//---- new performance file ----

	key = Get_Control_String (NEW_LINK_DATA_FILE);

	if (!key.empty ()) {
		Print (1);
		if (sum_periods.Num_Periods () > 0) {
			link_data_file.Copy_Periods (sum_periods);
		} else {
			link_data_file.Copy_Periods (time_periods);
		}
		link_data_file.Num_Decimals (1);
		link_data_file.Create_Fields ();

		if (Check_Control_Key (NEW_LINK_DATA_FORMAT)) {
			link_data_file.Dbase_Format (Get_Control_String (NEW_LINK_DATA_FORMAT));
		}
		link_data_file.Create (Project_Filename (key));
		link_data_flag = true;
		link_data_file.Write_Header ();

		if (day_flag) {
			link_data_name = link_data_file.Filename ();
			link_data_name.Split_Last (link_data_ext, ".");
		
			Print (2, String ("Link Data Files by Day = %s_day.%s") % link_data_name % link_data_ext);
		}
		if (min_max_flag) {
			link_data_name = link_data_file.Filename ();
			link_data_name.Split_Last (link_data_ext, ".");
		
			Print (2, String ("Minimum Link Data File = %s_MIN.%s") % link_data_name % link_data_ext);
			Print (1, String ("Maximum Link Data File = %s_MAX.%s") % link_data_name % link_data_ext);
		}
	} else if (!System_File_Flag (NEW_PERFORMANCE)) {
		Error ("A New Performance or Link Data File is Requried");
	}

	//---- new signal node file ----

	key = Get_Control_String (NEW_SIGNAL_NODE_FILE);

	if (!key.empty ()) {
		Print (1);
		signal_node_file.File_Type ("New Signal Node File");
		signal_node_file.Add_Field ("SIGNAL", DB_INTEGER, 10);
		signal_node_file.Add_Field ("NODE", DB_INTEGER, 10);

		if (Check_Control_Key (NEW_SIGNAL_NODE_FORMAT)) {
			signal_node_file.Dbase_Format (Get_Control_String (NEW_SIGNAL_NODE_FORMAT));
		}
		signal_node_file.Create (Project_Filename (key));
		signal_node_flag = true;
	}

	//---- new link id map file ----

	key = Get_Control_String (NEW_LINK_ID_MAP_FILE);

	if (!key.empty ()) {
		Print (1);
		new_map_file.File_Type ("New Link ID Map File");
		new_map_file.Add_Field ("STATION", DB_INTEGER, 10);
		new_map_file.Add_Field ("LINK", DB_INTEGER, 10);
		new_map_file.Add_Field ("OFFSET", DB_DOUBLE, 10.2);

		if (Check_Control_Key (NEW_LINK_ID_MAP_FORMAT)) {
			new_map_file.Dbase_Format (Get_Control_String (NEW_LINK_ID_MAP_FORMAT));
		}
		new_map_file.Create (Project_Filename (key));
		new_map_flag = true;
	}
}
