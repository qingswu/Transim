//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "TripData.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void TripData::Program_Control (void)
{
	int i, num, field;
	double size;
	bool flag;
	String key, name, buf;
	Field_Type type;

	Data_Group data_rec;
	Data_Itr data_itr;
	
	field = 0;

	//---- open network files ----

	Data_Service::Program_Control ();

	input_file = (Zone_File *) System_File_Handle (ZONE);

	output_file = (Zone_File *) System_File_Handle (NEW_ZONE);

	flag = exe->Notes_Name_Flag ();
	exe->Notes_Name_Flag (false);

	Print (2, String ("%s Control Keys:") % Program ());	

	//---- open the trip file ----

	key = Get_Control_String (TRIP_FILE);

	trip_file.File_Type ("Trip File");
	trip_file.File_ID ("Trip");

	if (Check_Control_Key (TRIP_FORMAT)) {
		trip_file.Dbase_Format (Get_Control_String (TRIP_FORMAT));
	}
	trip_file.Open (Project_Filename (key));

	//---- open the new trip file ----

	key = Get_Control_String (NEW_TRIP_FILE);

	new_trip_file.File_Type ("New Trip File");
	new_trip_file.File_ID ("NewTrip");

	if (Check_Control_Key (NEW_TRIP_FORMAT)) {
		new_trip_file.Dbase_Format (Get_Control_String (NEW_TRIP_FORMAT));
	}
	new_trip_file.Open (Project_Filename (key));

	//---- copy existing fields ----

	copy_flag = Get_Control_Flag (COPY_EXISTING_FIELDS);

	if (copy_flag) {
		output_file->Replicate_Fields (input_file, false);
	} else if (Get_Control_Flag (CLEAR_ALL_FIELDS)) {
		output_file->Clear_Fields ();
	} else {
		output_file->Create_Fields ();
	}
	exe->Notes_Name_Flag (flag);

	if (Check_Control_Key (ZONE_FILE_HEADER)) {
		if (Get_Control_Flag (ZONE_FILE_HEADER)) {
			output_file->Header_Lines (1);
		} else {
			output_file->Header_Lines (0);
		}
	}

	//---- get the new zone fields ----

	num = Highest_Control_Group (NEW_ZONE_FIELD, 0);

	if (num == 0) {
		if (!copy_flag) {
			Warning ("No New Zone Fields");
		}
	} else {
		Print (1);

		for (i=1; i <= num; i++) {
			key = Get_Control_Text (NEW_ZONE_FIELD, i);
			if (key.empty ()) continue;

			key.Split (name, ",");
			if (name.empty ()) goto data_error;

			field = output_file->Field_Number (name);
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
			output_file->Add_Field (name, type, size);
		}
	}
	if (exe->Notes_Name_Flag ()) {
		if (output_file->Field_Number ("NOTES") < 0) {
			output_file->Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
		}
	}
	output_file->Write_Header ();
	
	//---- check for data files ----

	num_data_files = Highest_Control_Group (DATA_FILE, 0);

	if (num_data_files > 0) {
		data_flag = true;
	}

	//---- read conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (key.empty ()) {
		if (data_flag) {
			Error ("A Convertion Script is needed for Data Processing");
		}
	} else {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (key));
		Print (1);
	}

	//---- read data files ----

	if (data_flag) {

		//---- open each file ----

		for (i=1; i <= num_data_files; i++) {
			key = Get_Control_String (DATA_FILE, i);
			if (key.empty ()) continue;

			//---- create a header file and file labels ----

			Print (1);

			data_group.push_back (data_rec);
			data_itr = --data_group.end ();

			data_itr->group = i;
			data_itr->file = new Db_Header ();
			data_itr->data_db = new Db_Sort_Array ();

			data_itr->file->File_Type (String ("Data File #%d") % i);
			data_itr->file->File_ID (String ("Data%d") % i);

			if (Check_Control_Key (DATA_FORMAT, i)) {
				data_itr->file->Dbase_Format (Get_Control_String (DATA_FORMAT, i));
			}
			data_itr->file->Open (Project_Filename (key));

			//---- find the data join field ----

			key = Get_Control_Text (DATA_JOIN_FIELD, i);
			if (key.empty ()) goto control_error;

			field = data_itr->file->Field_Number (key);

			if (field < 0) {
				Error (String ("Data Join Field %s was Not Found") % key);
			}
			data_itr->join_field = field;
			Print (0, ", Number = ") << (field + 1);

			//---- find the zone join field ----

			key = Get_Control_Text (ZONE_JOIN_FIELD, i);
			if (key.empty ()) goto control_error;

			field = input_file->Field_Number (key);
			if (field < 0) {
				Error (String ("Zone Join Field %s was Not Found") % key);
			}
			data_itr->zone_field = field;
			Print (0, ", Number = ") << (field + 1);
		}
	}
	Show_Message (1);
	return;

control_error:
	Error (String ("Missing Control Key = %s") % Current_Key ());

data_error:
	Error (String ("New Zone Field %d is Improperly Specified") % i);

field_error:
	Error (String ("New Zone Field %s already exists as Field %d") % name % field);
}
