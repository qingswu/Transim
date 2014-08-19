//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void NewLandUse::Program_Control (void)
{
	int i, j, num;
	String key;
	Zone_File *zone_file, *new_file;
	Dbls_Array dbls_array;
	Str_Itr str_itr;
	Processing_Data step_data;

	//---- create the network files ----

	Data_Service::Program_Control ();

	zone_file = (Zone_File *) System_File_Handle (ZONE);
	new_file = (Zone_File *) System_File_Handle (NEW_ZONE);

	new_file->Clear_Fields ();
	new_file->Replicate_Fields (zone_file);
	new_file->Write_Header ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- zone data ----

	num = Highest_Control_Group (ZONE_DATA_FIELD, 0);
	if (num == 0) {
		Error ("Zone Data Fields are Required");
	}

	for (i=1; i <= num; i++) {
		key = Get_Control_Text (ZONE_DATA_FIELD, i);

		if (!key.empty ()) {
			j = zone_file->Required_Field (key);
			Print (0, ", Number = ") << (j + 1);

			data_fields.push_back (j);
			data_names.push_back (key);
		}
	}
	Print (1);

	//---- target field ----

	key = Get_Control_Text (ZONE_TARGET_FIELD);

	if (!key.empty ()) {
		target_field = zone_file->Required_Field (key);
		Print (0, ", Number = ") << (target_field + 1);
	}

	//---- area field ----

	key = Get_Control_Text (ZONE_AREA_FIELD);

	if (!key.empty ()) {
		area_field = zone_file->Required_Field (key);
		Print (0, ", Number = ") << (area_field + 1);
	}

	//---- group field ----

	num_group = Highest_Control_Group (ZONE_GROUP_FIELD, 0);
	if (num_group > 0) {
		group_index.assign (num_group + 1, -1);

		for (i=1; i <= num_group; i++) {
			key = Get_Control_Text (ZONE_GROUP_FIELD, i);

			if (!key.empty ()) {
				j = zone_file->Required_Field (key);
				Print (0, ", Number = ") << (j + 1);

				group_index [i] = (int) group_fields.size ();
				group_fields.push_back (j);
			}
		}
	}

	//---- type field ----

	num_type = Highest_Control_Group (ZONE_TYPE_FIELD, 0);
	if (num_type > 0) {
		type_index.assign (num_type + 1, -1);

		for (i=1; i <= num_type; i++) {
			key = Get_Control_Text (ZONE_TYPE_FIELD, i);

			if (!key.empty ()) {
				j = zone_file->Required_Field (key);
				Print (0, ", Number = ") << (j + 1);

				type_index [i] = (int) type_fields.size ();
				type_fields.push_back (j);
			}
		}
	}
	//---- coverage field ----

	num_cover = Highest_Control_Group (ZONE_COVERAGE_FIELD, 0);
	if (num_cover > 0) {
		cover_index.assign (num_cover + 1, -1);

		for (i=1; i <= num_cover; i++) {
			key = Get_Control_Text (ZONE_COVERAGE_FIELD, i);

			if (!key.empty ()) {
				j = zone_file->Required_Field (key);
				Print (0, ", Number = ") << (j + 1);

				cover_index [i] = (int) cover_fields.size ();
				cover_fields.push_back (j);
			}
		}
	}

	//---- zone year files ----

	num_years = Highest_Control_Group (ZONE_YEAR_FILE, 0);
	if (num_years > 0) {
		year_index.assign (num_years + 1, -1);
		year_index [0] = 0;

		for (i=1; i <= num_years; i++) {
			key = Get_Control_String (ZONE_YEAR_FILE, i);

			if (!key.empty ()) {
				Print (1);
				Zone_File *file = new Zone_File ();
				file->File_Type (String ("Zone Year File #%d") % i);
				file->File_ID (String ("Zone%d") % i);

				if (Check_Control_Key (ZONE_YEAR_FORMAT, i)) {
					file->Dbase_Format (Get_Control_String (ZONE_YEAR_FORMAT, i));
				}
				file->Open (Project_Filename (key));

				year_index [i] = (int) zone_year_array.size () + 1;
				zone_year_array.push_back (file);
				
				//---- check for data fields ----

				for (str_itr = data_names.begin (); str_itr != data_names.end (); str_itr++) {
					file->Required_Field (*str_itr);
				}
			}
		}
	}

	//---- target density files ----

	key = Get_Control_String (TARGET_DENSITY_FILE);

	if (!key.empty ()) {
		Print (1);
		target_file.File_Type ("Target Density File");
		target_file.File_ID ("Target");

		if (Check_Control_Key (TARGET_DENSITY_FORMAT)) {
			target_file.Dbase_Format (Get_Control_String (TARGET_DENSITY_FORMAT));
		}
		target_file.Open (Project_Filename (key));

		//---- check for data fields ----

		index_field = target_file.Required_Field ("TARGET", "DISTRICT", "GROUP", "STATION", "ZONE");

		for (str_itr = data_names.begin (); str_itr != data_names.end (); str_itr++) {
			target_file.Required_Field (*str_itr);
		}
	}

	//---- processing steps ----

	num = Highest_Control_Group (PROCESSING_STEP, 0);
	if (num > 0) {
		Print (1);

		for (i=1; i <= num; i++) {
			key = Get_Control_Text (PROCESSING_STEP, i);

			if (!key.empty ()) {
				memset (&step_data, '\0', sizeof (step_data));

				step_data.step = i;

				Parse_Step (key, step_data);

				//---- parse and process the data ----

				processing_steps.push_back (step_data);
			}
		}
	}

	//---- check for friction curves ----

	num = Highest_Control_Group (FUNCTION_PARAMETERS, 0);
	Print (1);
		
	if (num == 0) {
		Error ("At least one Friction Function is Required");
	}

	//---- friction curve ----

	for (i=1; i <= num; i++) {
		key = Get_Control_String (FUNCTION_PARAMETERS, i);

		if (key.empty ()) continue;
				
		function.Add_Function (i, key);
	}
	Print (1);

	//---- maximum iterations ----

	max_iterations = Get_Control_Integer (MAXIMUM_ITERATIONS);

	//---- maximum percent difference ----

	max_percent = Get_Control_Double (MAXIMUM_PERCENT_DIFFERENCE) / 100.0;

	//---- maximum trip difference ----

	max_diff = Get_Control_Double (MAXIMUM_TRIP_DIFFERENCE);

	//---- read report types ----

	List_Reports ();
}

