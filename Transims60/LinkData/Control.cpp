//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void LinkData::Program_Control (void)
{
	String key;
	String_List string_list;
	Str_Itr str_itr;

	//---- open control file ----

	Data_Service::Program_Control ();
		
	Print (2, String ("%s Control Keys:") % Program ());

	//---- open the link node equivalence ----

	key = Get_Control_String (LINK_NODE_LIST_FILE);

	if (!key.empty ()) {
		link_node_flag = true;
		Print (1);

		link_node.File_Type ("Link Node List File");
		link_node.Open (Project_Filename (key));
	}

	//---- open the input node file ----

	key = Get_Control_String (INPUT_NODE_FILE);

	if (!key.empty ()) {
		input_flag = true;
		Print (1);
		input_node_file.File_Type ("Input Node File");

		//---- get the file format ----

		if (Check_Control_Key (INPUT_NODE_FORMAT)) {
			input_node_file.Dbase_Format (Get_Control_String (INPUT_NODE_FORMAT));
		}
		input_node_file.Open (Project_Filename (key));

		//---- node map file ----

		key = Get_Control_String (NODE_MAP_FILE);

		if (!key.empty ()) {
			node_map_flag = true;
			node_map_file.File_Type ("Node Map File");
			node_map_file.Open (Project_Filename (key));
		}

		//---- coordinate resolution ----

		resolution = Scale (Get_Control_Double (COORDINATE_RESOLUTION));
	}
	if (!input_flag && !link_node_flag) {
		Error ("A Link Node List or Input Node File are Required");
	}

	//---- open the directional data file ----

	key = Get_Control_String (DIRECTIONAL_DATA_FILE);

	if (key.empty ()) goto control_error;

	Print (1);
	dir_file.File_Type ("Directional Data File");
	dir_file.Dbase_Format (Get_Control_String (DIRECTIONAL_DATA_FORMAT));

	dir_file.Open (Project_Filename (key));

	//---- set the data flag ----

	key = Get_Control_String (NEW_LINK_DATA_FILE);
	if (!key.empty ()) {
		data_flag = true;
	}

	//---- get the from node field name ----

	key = Get_Control_Text (FROM_NODE_FIELD_NAME);

	if (key.empty ()) {
		from_field = dir_file.Required_Field (ANODE_FIELD_NAMES);
	} else {
		from_field = dir_file.Required_Field (key);
	}
	Print (0, ", Number = ") << (from_field + 1);

	//---- get the to node field name ----

	key = Get_Control_Text (TO_NODE_FIELD_NAME);

	if (key.empty ()) {
		to_field = dir_file.Required_Field (BNODE_FIELD_NAMES);
	} else {
		to_field = dir_file.Required_Field (key);
	}
	Print (0, ", Number = ") << (to_field + 1);

	if (data_flag) {

		//---- get the volume field name ----

		if (Get_Control_List_Groups (VOLUME_DATA_FIELD_NAME, string_list)) {
			volume_flag = true;

			for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
				vol_fields.push_back (dir_file.Required_Field (*str_itr));
			}
			num_fields = (int) vol_fields.size ();
			if (num_fields == 1) {
				Print (0, ", Number = ") << (vol_fields [0] + 1);
			}
		}

		//---- get the speed field name ----

		if (Get_Control_List_Groups (SPEED_DATA_FIELD_NAME, string_list)) {
			speed_flag = true;

			for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
				spd_fields.push_back (dir_file.Required_Field (*str_itr));

				Field_Ptr fld = dir_file.Field (*str_itr);

				if (fld->Units () == NO_UNITS) {
					fld->Units ((Metric_Flag () ? KPH : MPH));
				}
			}
			if (spd_fields.size () == 1) {
				Print (0, ", Number = ") << (spd_fields [0] + 1);
			}	
		}

		//---- get the ab volume field name ----

		if (Get_Control_List_Groups (AB_VOLUME_FIELD_NAME, string_list)) {
			if (volume_flag) {
				Error ("Volume Data and AB Volume Fields are Mutually Exclusive");
			}
			if (speed_flag) {
				Error ("Speed Data and AB Volume Fields are Mutually Exclusive");
			}
			for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
				vol_ab_fields.push_back (dir_file.Required_Field (*str_itr));
			}
			num_fields = (int) vol_ab_fields.size ();
			if (num_fields == 1) {
				Print (0, ", Number = ") << (vol_ab_fields [0] + 1);
			}	

			//---- get the ba volume field name ----

			if (!Get_Control_List_Groups (BA_VOLUME_FIELD_NAME, string_list)) {
				Error ("AB and BA Volume Fields are Both Needed");
			}
			for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
				vol_ba_fields.push_back (dir_file.Required_Field (*str_itr));
			}
			if ((int) vol_ba_fields.size () != num_fields) {
				Error (String ("Number of AB/BA Fields Differ (%d vs %d)") % num_fields % vol_ba_fields.size ());
			}
			if (num_fields == 1) {
				Print (0, ", Number = ") << (vol_ba_fields [0] + 1);
			}	
			two_way_flag = true;
			volume_flag = true;
		}

		//---- get the ab speed field name ----

		if (Get_Control_List_Groups (AB_SPEED_FIELD_NAME, string_list)) {
			if (speed_flag) {
				Error ("Speed Data and AB Speed Fields are Mutually Exclusive");
			}
			for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
				spd_ab_fields.push_back (dir_file.Required_Field (*str_itr));
			}
			if (spd_ab_fields.size () == 1) {
				Print (0, ", Number = ") << (spd_ab_fields [0] + 1);
			}	

			//---- get the ba speed field name ----

			if (!Get_Control_List_Groups (BA_SPEED_FIELD_NAME, string_list)) {
				Error ("AB and BA Speed Fields are Both Needed");
			}
			for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
				spd_ba_fields.push_back (dir_file.Required_Field (*str_itr));
			}
			if ((int) spd_ba_fields.size () != num_fields) {
				Error (String ("Number of AB/BA Fields Differ (%d vs %d)") % num_fields % spd_ba_fields.size ());
			}
			if (spd_ba_fields.size () == 1) {
				Print (0, ", Number = ") << (spd_ba_fields [0] + 1);
			}	
			two_way_flag = true;
			speed_flag = true;
		}

		if (!volume_flag && !speed_flag) {
			Error ("Input Volume and/or Speed Field Names are Required");
		}

		//---- open the new link data file ----

		key = Get_Control_String (NEW_LINK_DATA_FILE);
		Print (1);

		if (Check_Control_Key (NEW_LINK_DATA_FORMAT)) {
			data_file.Dbase_Format (Get_Control_String (NEW_LINK_DATA_FORMAT));
		}
		if (Check_Control_Key (SUMMARY_TIME_RANGES)) {
			if (Check_Control_Key (NEW_AB_VOLUME_FIELD_NAME) || Check_Control_Key (NEW_AB_SPEED_FIELD_NAME) ||
				Check_Control_Key (NEW_AB_VOLUME_FIELD_NAME) || Check_Control_Key (NEW_AB_SPEED_FIELD_NAME) ) {

				Warning ("Output Field Names are Ignored when Summary Time Ranges are Provided");
			}
			data_file.Num_Decimals (1);
			data_file.Data_Units (Performance_Units_Map ((volume_flag) ? FLOW_DATA : SPEED_DATA));
			data_file.Copy_Periods (sum_periods);

			data_file.Create (Project_Filename (key));
	
			data_file.Create_Fields ();
			data_file.Write_Header ();
		} else {
			data_file.Create (Project_Filename (key));
			custom_flag = true;
			data_file.Set_Flag (false);

			data_file.Add_Field ("LINK", DB_INTEGER, 10);
			data_file.Add_Field ("ANODE", DB_INTEGER, 10);
			data_file.Add_Field ("BNODE", DB_INTEGER, 10);

			//---- get the volume field names ----

			if (volume_flag) {
				if (!Get_Control_List_Groups (NEW_AB_VOLUME_FIELD_NAME, string_list)) goto control_error;
				
				for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
					data_file.Add_Field (*str_itr, DB_INTEGER, 10, VEHICLES);
					ab_vol_fields.push_back (data_file.Required_Field (*str_itr));
				}
				if ((int) ab_vol_fields.size () != num_fields) {
					Error (String ("Number of Input/Output Fields Differ (%d vs %d)") % num_fields % ab_vol_fields.size ());
				}
				if (!Get_Control_List_Groups (NEW_BA_VOLUME_FIELD_NAME, string_list)) goto control_error;
				
				for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
					data_file.Add_Field (*str_itr, DB_INTEGER, 10, VEHICLES);
					ba_vol_fields.push_back (data_file.Required_Field (*str_itr));
				}
				if ((int) ba_vol_fields.size () != num_fields) {
					Error (String ("Number of Input/Output Fields Differ (%d vs %d)") % num_fields % ba_vol_fields.size ());
				}
			}

			//---- get the speed field names ----

			if (speed_flag) {
				if (!Get_Control_List_Groups (NEW_AB_SPEED_FIELD_NAME, string_list)) goto control_error;
				
				for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
					data_file.Add_Field (*str_itr, DB_DOUBLE, 6.1, (Metric_Flag () ? KPH : MPH));
					ab_spd_fields.push_back (data_file.Required_Field (*str_itr));
				}
				if ((int) ab_spd_fields.size () != num_fields) {
					Error (String ("Number of Input/Output Fields Differ (%d vs %d)") % num_fields % ab_spd_fields.size ());
				}				
				if (!Get_Control_List_Groups (NEW_BA_SPEED_FIELD_NAME, string_list)) goto control_error;
				
				for (str_itr = string_list.begin () + 1; str_itr != string_list.end (); str_itr++) {
					data_file.Add_Field (*str_itr, DB_DOUBLE, 6.1, (Metric_Flag () ? KPH : MPH));
					ba_spd_fields.push_back (data_file.Required_Field (*str_itr));
				}
				if ((int) ba_spd_fields.size () != num_fields) {
					Error (String ("Number of Input/Output Fields Differ (%d vs %d)") % num_fields % ba_spd_fields.size ());
				}
			}
			data_file.Write_Header ();
		}
	}

	//---- open the new directional data file ----

	key = Get_Control_String (NEW_DIRECTIONAL_DATA_FILE);

	if (!key.empty ()) {
		Print (1);
		new_dir_file.File_Type ("New Directional Data File");

		new_dir_file.Add_Field ("LINK", DB_INTEGER, 10);
		new_dir_file.Add_Field ("DIR", DB_INTEGER, 1);

		new_dir_file.Replicate_Fields (&dir_file);

		if (Check_Control_Key (NEW_DIRECTIONAL_DATA_FORMAT)) {
			data_file.Dbase_Format (Get_Control_String (NEW_DIRECTIONAL_DATA_FORMAT));
		}
		new_dir_file.Create (Project_Filename (key));
		output_flag = true;
	}
	if (!data_flag && !output_flag) {
		Error ("Not Output Files have been Specified");
	}

	//---- new node map file ----

	key = Get_Control_String (NEW_NODE_MAP_FILE);

	if (!key.empty ()) {
		new_map_flag = true;
		new_map_file.File_Type ("New Node Map File");
		new_map_file.Create (Project_Filename (key));
	}

	//---- new link node file ----

	key = Get_Control_String (NEW_LINK_NODE_FILE);

	if (!key.empty ()) {
		new_link_node_flag = true;
		new_link_node.File_Type ("New Link Node File");
		new_link_node.Create (Project_Filename (key));
	}
	return;
	
control_error:
	Error (String ("Missing Control Key = %s") % Current_Key ());
}
