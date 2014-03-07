//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "FileFormat.hpp"

#include "TDF_Matrix.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void FileFormat::Program_Control (void)
{
	int i, j, num, field, num_org, num_des, tables;
	String key, name, buf;
	Strings strings;
	Str_Itr str_itr;
	Integers decimals;
	Data_Group data_rec, *data_ptr;
	Matrix_Group matrix_rec, *matrix_ptr;
	Field_Type type = DB_INTEGER;
	Format_Type format;
	Db_Field *fld_ptr;
	Units_Type units;
	double size;
	bool flag;

	data_rec.sort_flag = data_rec.select_flag = data_rec.int_flag = false;
	data_rec.sel_field = -1;

	//---- create the network files ----

	Execution_Service::Program_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- initialize the file list ----

	num = Highest_Control_Group (DATA_FILE, 0);

	if (num > 0) {
		data_flag = true;

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
			data_ptr->file->Scan_Lines (SCAN_ALL_RECORDS);

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

			//---- get the data select field ----

			key = Get_Control_Text (DATA_SELECT_FIELD, i);

			if (!key.empty ()) {
				key.Parse (strings, "=");
				if (strings.size () < 2) {
					Error ("Data Select Field does Not include \"=\" sign");
				} else if (strings.size () > 2) {
					Error ("Data Select Field has too many parameters");
				}
				data_ptr->sel_field = data_ptr->file->Field_Number (strings [0]);
				if (data_ptr->sel_field < 0) {
					Error (String ("Data Select Field \"%s\" was Not Found") % strings [0]);
				}
				data_ptr->select_flag = true;
				fld_ptr = data_ptr->file->Field (data_ptr->sel_field);

				if (fld_ptr->Type () == DB_STRING || fld_ptr->Type () == DB_CHAR) {
					strings [1].Parse (data_ptr->sel_strings, ",");
				} else {
					data_ptr->int_flag = true;
					key = strings [1];
					key.Parse (strings, ",");

					for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
						data_ptr->sel_values.push_back (str_itr->Integer ());
					}
				}
			}

			//---- open the output file ----

			Print (1);
			key = Get_Control_String (NEW_DATA_FILE, i);

			if (key.empty ()) {
				Error (String ("New Data File %d is Required") % i);
			}
			data_ptr->new_file = new Db_Header ();

			data_ptr->new_file->File_Type (String ("New Data File #%d") % i);
			data_ptr->new_file->File_ID (String ("NewData%d") % i);

			//---- get the file format ----

			if (Check_Control_Key (NEW_DATA_FORMAT, i)) {
				data_ptr->new_file->Dbase_Format (Get_Control_String (NEW_DATA_FORMAT, i));
			}
			data_ptr->new_file->Nest (data_ptr->file->Nest ());
			data_ptr->new_file->Create (Project_Filename (key));

			//---- copy existing fields ----

			flag = Get_Control_Flag (COPY_EXISTING_FIELDS, i);

			if (flag) {
				data_ptr->new_file->Replicate_Fields (data_ptr->file);
			}
			if (Get_Control_Flag (NEW_FILE_HEADER, i)) {
				data_ptr->new_file->Header_Lines (data_ptr->file->Header_Lines ());
				data_ptr->new_file->Header_Record (data_ptr->file->Header_Record ());
			} else if (data_ptr->new_file->Dbase_Format () == FIXED_COLUMN || 
				data_ptr->new_file->Dbase_Format () == BINARY) {
				data_ptr->new_file->Header_Lines (1);
			} else {
				data_ptr->new_file->Header_Lines (0);
			}

			//---- get the new data fields ----

			num = Highest_Control_Group (NEW_DATA_FIELD, i, 0);

			if (num == 0) {
				if (!flag) {
					Error ("No New Data Fields");
				}
			} else {
				Print (1);

				for (j=1; j <= num; j++) {
					key = Get_Control_Text (NEW_DATA_FIELD, i, j);
					if (key.empty ()) continue;

					key.Split (name, ",");
					if (name.empty ()) {
						Error (String ("New Data Field %d-%d is Improperly Specified") %i % j);
					}
					field = data_ptr->new_file->Field_Number (name);
					if (field >= 0) {
						Error (String ("New Data Field %s already exists as Field %d") % name % field);
					}
					key.Split (buf, ",");
					units = NO_UNITS;
					if (buf.empty () || buf.Starts_With ("I")) {
						type = DB_INTEGER;
					} else if (buf.Starts_With ("D") || buf.Starts_With ("R")) {
						type = DB_DOUBLE;
					} else if (buf.Starts_With ("S") || buf.Starts_With ("C")) {
						type = DB_STRING;
					} else if (buf.Starts_With ("T")) {
						type = DB_TIME;
						units = Time_Format ();
					} else {
						Error (String ("New Data Field %d-%d is Improperly Specified") %i % j);
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
					data_ptr->new_file->Add_Field (name, type, size, units);
				}
			}
			data_ptr->new_file->Write_Header ();
			
			file_array.push_back (data_ptr->file);

			//---- process the sort option ----

			key = Get_Control_Text (SORT_BY_FIELDS, i);

			if (!key.empty ()) {
				if (data_ptr->new_file->Nest_Flag ()) {
					Error ("Nested Files can not be Sorted");
				}
				data_ptr->sort_flag = true;
				key.Parse (strings);
				data_ptr->sort_string = new Db_Base (MODIFY, FIXED_COLUMN);
				data_ptr->dbase = new Db_Data_Array ();
				data_ptr->dbase->Dbase_Format (FIXED_COLUMN);

				data_ptr->dbase->Replicate_Fields (data_ptr->new_file, true);
				data_ptr->dbase->File_ID (data_ptr->new_file->File_ID ());

				for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
					j = data_ptr->new_file->Required_Field (*str_itr);
					fld_ptr = data_ptr->new_file->Field (j);
					data_ptr->sort_string->Add_Field (fld_ptr->Name (), fld_ptr->Type (), fld_ptr->Size ());
				}
				file_array.push_back (data_ptr->dbase);
			} else {
				data_ptr->sort_flag = false;
				file_array.push_back (data_ptr->new_file);
			}
		}
	}

	//---- process matrix keys ----

	num = Highest_Control_Group (MATRIX_FILE, 0);

	if (num > 0) {
		matrix_flag = true;

		//---- open each matrix file ----

		for (i=1; i <= num; i++) {

			if (!Check_Control_Key (MATRIX_FILE, i)) continue;
			Print (1);

			matrix_rec.group = i;
			matrix_group.push_back (matrix_rec);

			matrix_ptr = &matrix_group.back ();

			//---- open matrix file ----

			key = Project_Filename (Get_Control_String (MATRIX_FILE, i));

			format = Db_Header::Def_Format (key);

			if (format == UNFORMATED) {
				if (Check_Control_Key (MATRIX_FORMAT, i)) {
					name = Get_Control_String (MATRIX_FORMAT, i);
				} else {
					name = Get_Default_Text (MATRIX_FORMAT);
				}
				format = Format_Code (name);
			}
			matrix_ptr->matrix = TDF_Matrix (format);

			if (num > 1) {
				matrix_ptr->matrix->File_Type (String ("Matrix File #%d") % i);
				matrix_ptr->matrix->File_ID (String ("Matrix%d") % i);
			} else {
				matrix_ptr->matrix->File_Type ("Matrix File");
				matrix_ptr->matrix->File_ID ("Matrix");
			}
			matrix_ptr->matrix->Dbase_Format (format);

			matrix_ptr->matrix->Open (key);

			num_org = matrix_ptr->matrix->Num_Org ();
			num_des = matrix_ptr->matrix->Num_Des ();
			tables = matrix_ptr->matrix->Tables ();

			Print (0, " (Zones=") << num_des << " Tables=" << tables << ")";
			Print (1);

			//---- create new matrix file ----

			key = Get_Control_String (NEW_MATRIX_FILE, i);
			if (key.empty ()) {
				Error ("A New Matrix File is Required");
			}

			if (Check_Control_Key (NEW_MATRIX_FORMAT, i)) {
				format = Format_Code (Get_Control_String (NEW_MATRIX_FORMAT, i));
			}
			matrix_ptr->new_matrix = TDF_Matrix (format);

			if (num > 1) {
				matrix_ptr->new_matrix->File_Type (String ("New Matrix File #%d") % i);
				matrix_ptr->new_matrix->File_ID (String ("NewMatrix%d") % i);
			} else {
				matrix_ptr->new_matrix->File_Type ("New Matrix File");
				matrix_ptr->new_matrix->File_ID ("NewMatrix");
			}
			matrix_ptr->new_matrix->Filename (Project_Filename (key));
			matrix_ptr->new_matrix->Dbase_Format (format);

			//---- select tables ----

			key = Get_Control_String (SELECT_TABLES, i);
			if (key.empty () || key.Equals ("ALL")) {

				for (j=0; j < tables; j++) {
					matrix_ptr->table_map.push_back (matrix_ptr->matrix->Table_Field_Number (j));
				}
				matrix_ptr->new_matrix->Replicate_Fields (matrix_ptr->matrix, false, false);
			} else {
				key.Parse (strings);
				matrix_ptr->new_matrix->Clear_Fields ();

				matrix_ptr->new_matrix->Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
				matrix_ptr->new_matrix->Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);

				for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
					j = matrix_ptr->matrix->Required_Field (*str_itr);
					matrix_ptr->table_map.push_back (j);
					fld_ptr = matrix_ptr->matrix->Field (j);
					size = (double) sizeof (double) + (fld_ptr->Decimal () / 10.0);
					matrix_ptr->new_matrix->Add_Field (fld_ptr->Name (), DB_DOUBLE, size, NO_UNITS, true);
				}
			}
			matrix_ptr->new_matrix->Num_Org (num_org);
			matrix_ptr->new_matrix->Num_Des (num_des);

			matrix_ptr->new_matrix->Create ();

			if (!matrix_ptr->new_matrix->Allocate_Data ()) {
				Error (String ("Insufficient Memory for Matrix %s") % matrix_ptr->new_matrix->File_ID ());
			}
			Print (0, " (Zones=") << matrix_ptr->new_matrix->Num_Des () << " Tables=" << matrix_ptr->new_matrix->Tables () << ")";
			Print (1);

			file_array.push_back (matrix_ptr->matrix);
			file_array.push_back (matrix_ptr->new_matrix);

			Get_Control_Text (SELECT_TABLES, i);
		}
	}

	if (!data_flag && !matrix_flag) {
		Error ("No Input File Keys were Found");
	}

	//---- read conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (!key.empty ()) {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (key));
	}

	//---- read report types ----

	List_Reports ();

	stats_flag = Report_Flag (STATS_REPORT);

	if (script_flag) {
		Show_Message ("Compiling Conversion Script");

		if (Report_Flag (PRINT_SCRIPT)) {
			Header_Number (PRINT_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		program.Initialize (file_array, random.Seed () + 1);

		if (!program.Compile (program_file, Report_Flag (PRINT_SCRIPT))) {
			Error ("Compiling Conversion Script");
		}
		if (Report_Flag (PRINT_STACK)) {
			Header_Number (PRINT_STACK);

			program.Print_Commands (false);
		}
		Header_Number (0);
		Write (1);
	}
}

