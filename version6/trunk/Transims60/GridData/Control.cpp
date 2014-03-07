//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "GridData.hpp"

#include "Subzone_File.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void GridData::Program_Control (void)
{
	int i, num, field;
	double size;
	bool copy_flag, z_flag, m_flag;

	Field_Type type;
	String key, name, buf;
	Db_Header *file_ptr;
	Data_Group data_rec;
	Data_Itr data_itr;
	Field_Ptr fld_ptr;
	Polygon_Group poly_rec;
	Polygon_Itr poly_itr;
	Data_Field data_field, *data_ptr;
	Summary_Group group_data, *group_ptr;
	Summary_Itr group_itr;
	Dbls_Itr dbls_itr;
	Doubles doubles;
	
	field = 0;
	group_data.all_flag = true;

	//---- open network files ----

	Data_Service::Program_Control ();
	
	//---- read the projection information ----

	projection.Read_Control ();

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- arcview grid file ----

	key = Get_Control_String (ARC_GRID_FILE);
	copy_flag = false;
	
	if (!key.empty ()) {
		grid_flag = true;

		Print (1);
		in_file.File_Type ("Arc Grid File");
		in_file.File_ID ("Grid");

		if (!in_file.Open (Project_Filename (key))) {
			File_Error ("Opening Arc Grid File", in_file.Shape_Filename ());
		}

		//---- get the zone field numbers ----

		field = in_file.Field_Number ("ZONE");
		if (field > 0) {
			in_zone_fld.push_back (field);
		}
		for (i=1; ; i++) {
			name ("ZONE%d") % i;
			field = in_file.Field_Number (name);
			if (field < 0) break;
			in_zone_fld.push_back (field);
		}

		//---- grid data file ----

		num = Highest_Control_Group (GRID_DATA_FILE, 0);

		//---- open each file ----

		for (i=1; i <= num; i++) {
			key = Get_Control_String (GRID_DATA_FILE, i);
			if (key.empty ()) continue;

			Print (1);
			file_ptr = new Db_Header ();
			data_files.push_back (file_ptr);

			file_ptr->File_Type (String ("Grid Data File #%d") % i);
			file_ptr->File_ID (String ("Data%d") % i);

			if (Check_Control_Key (GRID_DATA_FORMAT, i)) {
				file_ptr->Dbase_Format (Get_Control_String (GRID_DATA_FORMAT, i));
			}
			file_ptr->Open (Project_Filename (key));
		}

		//---- copy existing fields ----

		copy_flag = Get_Control_Flag (COPY_EXISTING_FIELDS);
	}

	//---- create base fields ----

	out_file.File_Type ("New Arc Grid File");
	out_file.File_ID ("NewGrid");
	out_file.File_Access (CREATE);
	out_file.Shape_Type (POLYGON);

	if (copy_flag) {
		out_file.Replicate_Fields (&in_file);
		z_flag = in_file.Z_Flag ();
		m_flag = in_file.M_Flag ();
	} else {
		out_file.Add_Field ("ID", DB_INTEGER, 10);
		out_file.Add_Field ("X_COORD", DB_DOUBLE, 14.1, METERS);
		out_file.Add_Field ("Y_COORD", DB_DOUBLE, 14.1, METERS);
		out_file.Add_Field ("ZONE", DB_INTEGER, 6);
	}
	out_file.Z_Flag (z_flag);
	out_file.M_Flag (m_flag);

	//---- create new arc grid file ---

	Print (1);
	key = Get_Control_String (NEW_ARC_GRID_FILE);

	if (!out_file.Open (Project_Filename (key))) {
		File_Error ("Opening New Arc Grid File", out_file.Shape_Filename ());
	}

	//---- add zone fields ----

	if (Check_Control_Key (GRID_ZONE_FIELDS)) {
		num_zone_fields = Get_Control_Integer (GRID_ZONE_FIELDS);

		for (i=1; i < num_zone_fields; i++) {
			name ("ZONE%d") % i;
			if (out_file.Field_Number (name) < 0) {
				out_file.Add_Field (name, DB_INTEGER, 6);
			}
		}
	}

	//---- get the new grid fields ----

	num = Highest_Control_Group (NEW_GRID_FIELD, 0);

	for (i=1; i <= num; i++) {
		if (i == 1) Print (1);
		key = Get_Control_Text (NEW_GRID_FIELD, i);
		if (key.empty ()) continue;

		key.Split (name, ",");
		if (name.empty ()) goto data_error;

		field = out_file.Field_Number (name);
		if (field >= 0) goto field_error;

		key.Split (buf, ",");
		if (buf.empty () || buf.Starts_With ("I")) {
			type = DB_INTEGER;
		} else if (buf.Starts_With ("D") || buf.Starts_With ("R")) {
			type = DB_DOUBLE;
		} else if (buf.Starts_With ("S") || buf.Starts_With ("C")) {
			type = DB_STRING;
		} else {
			goto data_error;
		}

		key.Split (buf, ",");
		if (buf.empty ()) {
			if (type == DB_DOUBLE) {
				size = 10.2;
			} else {
				size = 10.0;
			}
		} else {
			size = buf.Double ();
		}
		out_file.Add_Field (name, type, size);
	}

	//---- read conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (!key.empty ()) {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (key));
	}

	//---- open the zone boundary file ----

	if (!grid_flag) {
		key = Get_Control_String (ZONE_BOUNDARY_FILE);

		if (!key.empty ()) {
			Print (1);
			boundary_file.File_Type ("Zone Boundary File");
			boundary_file.File_ID ("Boundary");

			boundary_file.Open (Project_Filename (key));

			boundary_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

			if (Check_Control_Key (BOUNDARY_ZONE_FIELD)) {
				key = Get_Control_Text (BOUNDARY_ZONE_FIELD);
				zone_field = boundary_file.Required_Field (key);
			} else {
				zone_field = boundary_file.Required_Field (ZONE_FIELD_NAMES);
			}

			//---- grid size ----

			grid_size = Get_Control_Double (GRID_SIZE);
		} else {
			Error ("A Zone Boundary File is Required to Create Grids");
		}
	}
	
	//---- check for zone data files ----

	num = Highest_Control_Group (ZONE_DATA_FILE, 0);

	if (num > 0) {
		if (!script_flag && !grid_flag) {
			Error ("Zone Data Processing Requires an Input Grid File and Conversion Script");
		}
		zone_grids.File_ID ("ZONE_GRID");
		zone_grids.File_Format (BINARY);
		zone_grids.Add_Field ("COUNT", DB_INTEGER, 10);
	}

	//---- open each file ----

	for (i=1; i <= num; i++) {
		key = Get_Control_String (ZONE_DATA_FILE, i);
		if (key.empty ()) continue;
		zone_flag = true;

		//---- create a header file and file labels ----

		Print (1);
		data_group.push_back (data_rec);
		data_itr = --data_group.end ();

		data_itr->group = i;
		data_itr->file = new Db_Header ();
		data_itr->data_db = new Db_Sort_Array ();

		data_itr->file->File_Type (String ("Zone Data File #%d") % i);
		data_itr->file->File_ID (String ("Zone%d") % i);

		if (Check_Control_Key (ZONE_DATA_FORMAT, i)) {
			data_itr->file->Dbase_Format (Get_Control_String (ZONE_DATA_FORMAT, i));
		}
		data_itr->file->Open (Project_Filename (key));

		//---- find the data join field ----

		key = Get_Control_Text (ZONE_ID_FIELD, i);

		data_itr->zone_field = data_itr->file->Required_Field (key);

		Print (0, ", Number = ") << (data_itr->zone_field + 1);
	}

	//---- read the boundary polygons ----

	num = Highest_Control_Group (BOUNDARY_POLYGON_FILE, 0);

	if (num > 0) {
		if (!script_flag && !grid_flag) {
			Error ("Boundary Polygon Processing Requires an Input Grid File and Conversion Script");
		}

		//---- open each file ----

		for (i=1; i <= num; i++) {
			key = Get_Control_String (BOUNDARY_POLYGON_FILE, i);
			if (key.empty ()) continue;

			polygons.push_back (poly_rec);

			poly_itr = --polygons.end ();

			poly_itr->group = i;
			poly_itr->file = new Arcview_File ();
			poly_itr->data_db = new Db_Sort_Array ();
			poly_itr->file->Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

			Print (1);
			poly_itr->file->File_Type (String ("Boundary Polygon File #%d") % i);
			poly_itr->file->File_ID (String ("Polygon%d") % i);

			poly_itr->file->Open (Project_Filename (key));
			polygon_flag = true;
		}
	}

	//---- process the point data files ----

	key = Get_Control_String (POINT_DATA_FILE);

	if (!key.empty ()) {
		point_flag = true;
		Print (1);
		point_file.File_Type ("Point Data File");
		point_file.File_ID ("Point");

		if (Check_Control_Key (POINT_DATA_FORMAT)) {
			point_file.Dbase_Format (Get_Control_String (POINT_DATA_FORMAT));
		}
		point_file.Open (Project_Filename (key));

		//---- identify the id and xy coordinate fields ----

		key = Get_Control_Text (POINT_ID_FIELD);
		if (key.empty ()) {
			Error ("Point ID Field Name is Required");
		}
		field = point_file.Required_Field (key);
		Print (0, ", Number = ") << (field + 1);
			
		fld_ptr = point_file.Field (field);
		point_data.Add_Field (fld_ptr->Name (), fld_ptr->Type (), fld_ptr->Size (), fld_ptr->Units ());

		key = Get_Control_Text (POINT_X_FIELD);
		if (key.empty ()) {
			Error ("Point X Field Name is Required");
		}
		field = point_file.Required_Field (key);
		Print (0, ", Number = ") << (field + 1);

		fld_ptr = point_file.Field (field);
		point_data.Add_Field (fld_ptr->Name (), fld_ptr->Type (), fld_ptr->Size (), fld_ptr->Units ());
			
		key = Get_Control_Text (POINT_Y_FIELD);
		if (key.empty ()) {
			Error ("Point Y Field Name is Required");
		}
		field = point_file.Required_Field (key);
		Print (0, ", Number = ") << (field + 1);

		fld_ptr = point_file.Field (field);
		point_data.Add_Field (fld_ptr->Name (), fld_ptr->Type (), fld_ptr->Size (), fld_ptr->Units ());
				
		out_fields.push_back (out_file.Add_Field ("SHARE", DB_DOUBLE, 14.6));

		//---- read the data fields ----

		num = Highest_Control_Group (POINT_DATA_FIELD, 0);

		if (num == 0) {
			Error ("No Point Data Fields");
		}
		for (i=1; i <= num; i++) {
			key = Get_Control_Text (POINT_DATA_FIELD, i);
			if (key.empty ()) continue;

			field = point_file.Field_Number (key);
			if (field < 0) {
				Error (String ("Point Data Field %s was Not Found") % key);
			}
			Print (0, ", Number = ") << (field + 1);

			fld_ptr = point_file.Field (field);
			point_data.Add_Field (key, fld_ptr->Type (), fld_ptr->Size (), fld_ptr->Units ());

			out_fields.push_back (out_file.Add_Field (key, DB_DOUBLE, 16.6));
		}
		max_distance = Get_Control_Double (MAX_POINT_DISTANCE);
	}
	out_file.Write_Header ();

	out_file.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());

	//---- identify the zone field numbers ----

	field = out_file.Field_Number ("ZONE");
	if (field > 0) {
		out_zone_fld.push_back (field);
	}
	for (i=1; ; i++) {
		name ("ZONE%d") % i;
		field = out_file.Field_Number (name);
		if (field < 0) break;
		out_zone_fld.push_back (field);
	}
	num_zone_fields = (int) out_zone_fld.size ();

	//---- process the summary file keys ----

	key = Get_Control_String (NEW_SUMMARY_FILE);

	if (!key.empty ()) {
		summary_flag = true;
		Print (1);
		summary_file.File_Type ("New Summary File");

		if (Check_Control_Key (NEW_SUMMARY_FORMAT)) {
			summary_file.Dbase_Format (Get_Control_String (NEW_SUMMARY_FORMAT));
		}
		summary_file.Add_Field ("GROUP", DB_INTEGER, 4);
		summary_file.Add_Field ("SELECT", DB_STRING, 16);
		summary_file.Add_Field ("LOW", DB_INTEGER, 6);
		summary_file.Add_Field ("HIGH", DB_INTEGER, 10);
		summary_file.Add_Field ("COUNT", DB_INTEGER, 10);

		data_offset = summary_file.Num_Fields ();

		summary_file.Create (Project_Filename (key));

		//---- identify the select field ----

		num = Highest_Control_Group (SUMMARY_SELECT_FIELD, 0);

		if (num == 0) {
			Error ("Summary Select Fields are Required");
		}

		//---- process each select field ----

		for (i=1; i <= num; i++) {
			if (!Check_Control_Key (SUMMARY_SELECT_FIELD, i)) continue;
			Print (1);

			key = Get_Control_Text (SUMMARY_SELECT_FIELD, i);
			if (key.empty ()) continue;

			summary_array.push_back (group_data);
			group_ptr = &summary_array.back ();

			group_ptr->group = i;
			group_ptr->field_name = key;

			group_ptr->select_field = out_file.Required_Field (key);
			Print (0, ", Number = ") << (group_ptr->select_field + 1);

			//---- summary select values ----
	
			key = exe->Get_Control_Text (SUMMARY_SELECT_VALUES, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				if (!group_ptr->select_values.Add_Ranges (key)) {
					Error ("Adding Summary Select Values");
				}
				group_ptr->all_flag = false;
				field = group_ptr->select_values.Num_Ranges ();
				group_ptr->grid_counts.assign (field, 0);
				group_ptr->data_array.assign (field, doubles);
			}
		}

		//---- identify the data fields ----

		num = Highest_Control_Group (SUMMARY_DATA_FIELD, 0);

		if (num == 0) {
			Error ("Summary Data Fields are Required");
		}
		for (i=1; i <= num; i++) {
			if (!Check_Control_Key (SUMMARY_DATA_FIELD, i)) continue;
			Print (1);

			key = Get_Control_Text (SUMMARY_DATA_FIELD, i);
			if (key.empty ()) continue;

			data_field_array.push_back (data_field);
			data_ptr = &data_field_array.back ();

			key.Split (name, ",");
			if (name.empty ()) goto data_error;

			field = summary_file.Field_Number (name);
			if (field >= 0) goto field_error;

			key.Split (buf, ",");
			if (buf.empty () || buf.Starts_With ("I")) {
				type = DB_INTEGER;
			} else if (buf.Starts_With ("D") || buf.Starts_With ("R")) {
				type = DB_DOUBLE;
			} else if (buf.Starts_With ("S") || buf.Starts_With ("C")) {
				type = DB_STRING;
			} else {
				goto data_error;
			}

			key.Split (buf, ",");
			if (buf.empty ()) {
				if (type == DB_DOUBLE) {
					size = 10.2;
				} else {
					size = 10.0;
				}
			} else {
				size = buf.Double ();
			}
			data_ptr->data_field = summary_file.Add_Field (name, type, size);

			//---- data value ----
	
			key = exe->Get_Control_Text (SUMMARY_DATA_VALUE, i);

			if (!key.empty ()) {
				data_ptr->data_value = out_file.Field_Number (key);
				if (data_ptr->data_value >= 0) Print (0, ", Number = ") << (data_ptr->data_value + 1);
			} else {
				data_ptr->data_value = -1;
			}

			//---- data filter ----
	
			key = exe->Get_Control_Text (SUMMARY_DATA_FILTER, i);

			if (!key.empty () && !key.Equals ("ALL")) {
				key.Split (name, " ");
				if (name.empty ()) goto data_error;

				field = out_file.Field_Number (name);
				if (field < 0) {
					Error (String ("Grid Data Field %s was Not Found") % name);;
				}

				data_ptr->filter_field = field;

				key.Split (buf, " ");
				if (buf.empty ()) goto data_error;

				if (buf.Starts_With ("=") || buf.Equals ("EQ")) {
					data_ptr->filter_test = 0;
				} else if (buf.Equals (">") || buf.Equals ("GT")) {
					data_ptr->filter_test = 1;
				} else if (buf.Equals (">=") || buf.Equals ("GE")) {
					data_ptr->filter_test = 2;
				} else if (buf.Equals ("!=") || buf.Equals ("NE")) {
					data_ptr->filter_test = 3;
				} else if (buf.Equals ("<") || buf.Equals ("LT")) {
					data_ptr->filter_test = 4;
				} else if (buf.Equals ("<=") || buf.Equals ("LE")) {
					data_ptr->filter_test = 5;
				} else {
					goto data_error;
				}
				key.Split (buf, " ");
				data_ptr->filter_value = buf.Double ();

				//---- and condition ----

				data_ptr->filter2_field = -1;

				key.Split (buf, " ");
				if (buf.empty ()) continue;

				if (!buf.Equals ("AND") && !buf.Equals ("&&")) goto data_error;

				key.Split (name, " ");
				if (name.empty ()) goto data_error;

				field = out_file.Field_Number (name);
				if (field < 0) {
					Error (String ("Grid Data Field %s was Not Found") % name);;
				}
				data_ptr->filter2_field = field;

				key.Split (buf, " ");
				if (buf.empty ()) goto data_error;

				if (buf.Starts_With ("=") || buf.Equals ("EQ")) {
					data_ptr->filter2_test = 0;
				} else if (buf.Equals (">") || buf.Equals ("GT")) {
					data_ptr->filter2_test = 1;
				} else if (buf.Equals (">=") || buf.Equals ("GE")) {
					data_ptr->filter2_test = 2;
				} else if (buf.Equals ("!=") || buf.Equals ("NE")) {
					data_ptr->filter2_test = 3;
				} else if (buf.Equals ("<") || buf.Equals ("LT")) {
					data_ptr->filter2_test = 4;
				} else if (buf.Equals ("<=") || buf.Equals ("LE")) {
					data_ptr->filter2_test = 5;
				} else {
					goto data_error;
				}
				key.Split (buf, " ");
				data_ptr->filter2_value = buf.Double ();

			} else {
				data_ptr->filter_field = -1;
			}
		}
		summary_file.Write_Header ();

		//---- initialize the summary data ----

		num = (int) data_field_array.size ();

		for (group_itr = summary_array.begin (); group_itr != summary_array.end (); group_itr++) {
			if (group_itr->all_flag) continue;
			
			for (dbls_itr = group_itr->data_array.begin (); dbls_itr != group_itr->data_array.end (); dbls_itr++) {
				dbls_itr->assign (num, 0.0);
			}
		}
	}
	return;

data_error:
	Error (String ("New Grid Field %d is Improperly Specified") % i);

field_error:
	Error (String ("New Grid Field %s already exists as Field %d") % name % field);
}
