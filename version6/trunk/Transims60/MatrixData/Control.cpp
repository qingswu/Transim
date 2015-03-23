//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "MatrixData.hpp"

#include "TDF_Matrix.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void MatrixData::Program_Control (void)
{
	int i, j, num, tabs, field, zone, num_org, num_des;
	String key, name, buf, format;
	Strings strings, labels;
	Str_Itr str_itr;
	Data_Group data_rec, *data_ptr;
	Data_Itr data_itr;
	Int_Itr int_itr;
	Matrix_Group matrix_rec, *matrix_ptr;
	Matrix_Itr matrix_itr;
	Db_Field *fld_ptr;
	Field_Type type = DB_INTEGER;
	Units_Type units;
	double size;
	bool flag;

	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	Print (2, String ("%s Control Keys:") % Program ());

	factor = 0.0;
	script_flag = !Get_Control_String (CONVERSION_SCRIPT).empty ();

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
			factor += 1.0;

			data_ptr = &data_group.back ();
	
			Print (1);
			data_ptr->file = new Db_Header ();
			data_ptr->file->Scan_Lines (SCAN_ALL_RECORDS);

			if (num > 1) {
				data_ptr->file->File_Type (String ("Data File #%d") % i);
				data_ptr->file->File_ID (String ("Data%d") % i);
			} else {
				data_ptr->file->File_Type ("Data File");
				data_ptr->file->File_ID ("Data");
			}

			//---- get the file format ----

			if (Check_Control_Key (DATA_FORMAT, i)) {
				data_ptr->file->Dbase_Format (Get_Control_String (DATA_FORMAT, i));
			}
			data_ptr->file->Open (Project_Filename (key));
			file_array.push_back (data_ptr->file);

			//---- origin field ----

			if (Check_Control_Key (ORIGIN_FIELD, i)) {
				key = Get_Control_Text (ORIGIN_FIELD, i);
				data_ptr->org_field = data_ptr->file->Required_Field (key);
				Print (0, ", Number = ") << (data_ptr->org_field + 1);
			} else {
				data_ptr->org_field = data_ptr->file->Required_Field (ORIGIN_FIELD_NAMES);
			}

			//---- destination field ----

			if (Check_Control_Key (DESTINATION_FIELD, i)) {
				key = Get_Control_Text (DESTINATION_FIELD, i);
				data_ptr->des_field = data_ptr->file->Required_Field (key);
				Print (0, ", Number = ") << (data_ptr->des_field + 1);
			} else {
				data_ptr->des_field = data_ptr->file->Required_Field (DESTINATION_FIELD_NAMES);
			}

			//---- period processing options ----

			data_ptr->period_flag = data_ptr->time_flag = false;
			data_ptr->period = 0;

			if (Check_Control_Key (PERIOD_FIELD, i)) {

				//---- period field ----

				key = Get_Control_Text (PERIOD_FIELD, i);
				data_ptr->period_field = data_ptr->file->Required_Field (key);
				Print (0, ", Number = ") << (data_ptr->period_field + 1);
				data_ptr->period_flag =true;

			} else if (Check_Control_Key (LOW_TIME_FIELD, i)) {

				//---- low time field ----

				key = Get_Control_Text (LOW_TIME_FIELD, i);
				data_ptr->low_field = data_ptr->file->Required_Field (key);
				Print (0, ", Number = ") << (data_ptr->low_field + 1);

				//---- high time field ----

				key = Get_Control_Text (HIGH_TIME_FIELD, i);
				data_ptr->high_field = data_ptr->file->Required_Field (key);
				Print (0, ", Number = ") << (data_ptr->high_field + 1);
				data_ptr->time_flag = true;

			} else if (Check_Control_Key (PERIOD_NUMBER, i)) {

				//---- period number ----

				data_ptr->period = Get_Control_Integer (PERIOD_NUMBER, i);

			} else {

				//---- check for default field names ----

				data_ptr->period_field = data_ptr->file->Optional_Field (PERIOD_FIELD_NAMES);

				if (data_ptr->period_field < 0) {
					data_ptr->low_field = data_ptr->file->Optional_Field ("LOW", "START", "TIME1", "T1");
					data_ptr->high_field = data_ptr->file->Optional_Field ("HIGH", "END", "TIME2", "T2");

					if (data_ptr->low_field >= 0) {
						data_ptr->time_flag = true;
					}
				} else {
					data_ptr->period_flag = true;
				}
			}

			//---- data factor field ----

			if (Check_Control_Key (DATA_FACTOR_FIELD, i)) {
				key = Get_Control_Text (DATA_FACTOR_FIELD, i);
				data_ptr->factor_field = data_ptr->file->Required_Field (key);
				Print (0, ", Number = ") << (data_ptr->factor_field + 1);
			} else {
				data_ptr->factor_field = -1;
			}

			//---- table data fields ----

			tabs = Highest_Control_Group (TABLE_DATA_FIELD, i, 0);

			if (tabs == 0) {
				field = data_ptr->file->Required_Field ("TRIPS", "TOTAL", "SUM", "RIDERS", "TABLE");
				data_ptr->table_fields.push_back (field);
			} else {
				Print (1);

				for (j=1; j <= tabs; j++) {
					key = Get_Control_Text (TABLE_DATA_FIELD, i, j);
					if (key.empty ()) continue;
				
					field = data_ptr->file->Required_Field (key);
					Print (0, ", Number = ") << (field + 1);
					data_ptr->table_fields.push_back (field);
				}
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
			factor += 1.0;

			matrix_ptr = &matrix_group.back ();

			//---- open matrix file ----

			key = Project_Filename (Get_Control_String (MATRIX_FILE, i));

			format = Db_Header::Def_Format (key);

			if (format.empty ()) {
				if (Check_Control_Key (MATRIX_FORMAT, i)) {
					format = Get_Control_String (MATRIX_FORMAT, i);
				} else {
					format = Get_Default_Text (MATRIX_FORMAT);
				}
			}
			matrix_ptr->matrix = TDF_Matrix (READ, format);

			if (num > 1) {
				matrix_ptr->matrix->File_Type (String ("Matrix File #%d") % i);
				matrix_ptr->matrix->File_ID (String ("Matrix%d") % i);
			} else {
				matrix_ptr->matrix->File_Type ("Matrix File");
				matrix_ptr->matrix->File_ID ("Matrix");
			}
			matrix_ptr->matrix->Open (key);

			if (matrix_ptr->matrix->Num_Periods () > 1) {
				Print (0, " (Periods=") << matrix_ptr->matrix->Num_Periods () << " Zones=" << matrix_ptr->matrix->Num_Zones () << " Tables=" << matrix_ptr->matrix->Tables () << ")";
			} else {
				Print (0, " (Zones=") << matrix_ptr->matrix->Num_Zones () << " Tables=" << matrix_ptr->matrix->Tables () << ")";
			}

			//---- growth factor ----

			matrix_ptr->factor = Get_Control_Double (GROWTH_FACTOR, i);

			//---- transpose OD ----

			matrix_ptr->transpose = Get_Control_Flag (TRANSPOSE_OD, i);

			//---- select tables ----

			key = Get_Control_Text (SELECT_TABLES, i);
			if (key.empty () || key.Equals ("ALL")) {

				for (j=0; j < matrix_ptr->matrix->Tables (); j++) {
					matrix_ptr->table_map.push_back (matrix_ptr->matrix->Table_Field_Number (j));
				}
			} else {
				key.Parse (strings);

				for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
					matrix_ptr->table_map.push_back (matrix_ptr->matrix->Required_Field (*str_itr));
				}
			}

			//---- matrix period breaks ----

			key = Get_Control_Text (MATRIX_PERIOD_BREAKS, i);

			if (!key.empty () && !key.Equals ("NONE")) {
				matrix_ptr->break_flag = true;

				matrix_ptr->breaks.Add_Breaks (key);

				//---- matrix period fields ----

				key = Get_Control_Text (MATRIX_PERIOD_FIELDS, i);
				key.Parse (strings);

				for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
					j = matrix_ptr->matrix->Table_Number (*str_itr);
					if (j < 0) {
						Error (String ("Matrix Table %s was Not Found") % *str_itr);
					}
					matrix_ptr->fields.push_back (j);
				}
			} else {
				matrix_ptr->break_flag = false;
			}

			if (script_flag) {
				matrix_ptr->record = new Db_Matrix ();
				matrix_ptr->record->Replicate_Fields (matrix_ptr->matrix);
				matrix_ptr->record->File_ID (matrix_ptr->matrix->File_ID ());
				file_array.push_back (matrix_ptr->record);
			}
		}
	}

	if (!data_flag && !matrix_flag) {
		key = Get_Control_String (SQUARE_TABLE_FILE);
		
		if (key.empty ()) {
			Error ("Input Data or Matrix Files are Required");
		}
		Print (1);

		input_file.File_Type ("Square Table File");
		input_file.File_ID ("Square");

		input_file.Open (Project_Filename (key));
		input_flag = true;

		input_matrix = 	TDF_Matrix (READ, TAB_DELIMITED);

		Read_Square_Table ();
	}

	//---- read new matrix method ----

	key = Get_Control_String (NEW_MATRIX_METHOD);

	if (!key.empty ()) {
		if (key.Equals ("ADD")) {
			method = ADD_METHOD;
		} else if (key.Equals ("RATIO")) {
			method = RATIO_METHOD;
		} else if (key.Equals ("EXPAND")) {
			method = EXPAND_METHOD;
		} else if (key.Equals ("SELECT")) {
			method = SELECT_METHOD;
		} else {
			Error (String ("Unrecognized Matrix Method = %s") % key);
		}
	}

	//---- growth factor file ----

	key = Get_Control_String (GROWTH_FACTOR_FILE);

	if (!key.empty ()) {
		factor_flag = true;

		key = Project_Filename (key);

		format = Db_Header::Def_Format (key);

		if (format.empty ()) {
			if (Check_Control_Key (GROWTH_FACTOR_FORMAT)) {
				format = Get_Control_String (GROWTH_FACTOR_FORMAT);
			} else {
				format = Get_Default_Text (GROWTH_FACTOR_FORMAT);
			}
		}
		factor_file = TDF_Matrix (READ, format);

		factor_file->File_Type ("Growth Factor File");
		factor_file->File_ID ("Factor");

		Print (1);
		factor_file->Open (key);

		if (factor_file->Num_Periods () > 1) {
			Print (0, " (Periods=") << factor_file->Num_Periods () << " Zones=" << factor_file->Num_Des () << " Tables=" << factor_file->Tables () << ")";
		} else {
			Print (0, " (Zones=") << factor_file->Num_Des () << " Tables=" << factor_file->Tables () << ")";
		}
		if (!factor_file->Allocate_Data (true)) {
			Error ("Insufficient Memory for Factor Matrix");
		}

		//---- factor period breaks ----

		key = Get_Control_Text (FACTOR_PERIOD_BREAKS);

		if (!key.empty () && !key.Equals ("NONE")) {
			factor_periods.Add_Breaks (key);

			//---- factor period fields ----

			key = Get_Control_Text (FACTOR_PERIOD_FIELDS);
			key.Parse (strings);

			for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
				num = factor_file->Table_Number (*str_itr);
				if (num < 0) {
					Error (String ("Factor Table %s was Not Found") % *str_itr);
				}
				factor_fields.push_back (num);
			}
		} else {
			key = Get_Control_Text (FACTOR_TABLES);
			fac_table_flag = true;

			if (key.empty () || key.Equals ("ALL")) {

				for (i=0; i < factor_file->Tables (); i++) {
					factor_fields.push_back (factor_file->Table_Field_Number (i));
				}
			} else {
				key.Parse (strings);

				for (str_itr = strings.begin (); str_itr != strings.end (); str_itr++) {
					num = factor_file->Table_Number (*str_itr);
					if (num < 0) {
						Error (String ("Factor Table %s was Not Found") % *str_itr);
					}
					factor_fields.push_back (num);
				}
			}
		}
	}

	//---- create new matrix file ----

	key = Get_Control_String (NEW_MATRIX_FILE);

	if (!key.empty ()) {
		new_flag = true;

		if (Check_Control_Key (NEW_MATRIX_FORMAT)) {
			format = Get_Control_String (NEW_MATRIX_FORMAT);
		} else {
			format = Get_Default_Text (NEW_MATRIX_FORMAT);
		}
		new_matrix = TDF_Matrix (CREATE, format);
	} else {
		new_matrix = TDF_Matrix (CREATE, TAB_DELIMITED);
	}

	new_matrix->File_Type ("New Matrix File");
	new_matrix->File_ID ("NewMatrix");

	if (new_flag) {
		new_matrix->Filename (Project_Filename (key));
	}

	//---- build matrix fields ----

	new_matrix->Copy_Periods (sum_periods);
	if (new_matrix->Num_Periods () > 1) {
		period_flag = true;
	} else if (new_matrix->Num_Periods () == 0) {
		period_flag = false;
	} else {
		Dtime low, high;
		new_matrix->Period_Range (0, low, high);
		period_flag = (low != Model_Start_Time () || high != Model_End_Time ());
	}
	new_matrix->Clear_Fields ();

	new_matrix->Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
	new_matrix->Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);

	if (period_flag) {
		new_matrix->Add_Field ("PERIOD", DB_INTEGER, 2, NO_UNITS, true);
	}

	//---- create table fields ----

	num = Highest_Control_Group (NEW_MATRIX_TABLE, 0);

	if (num > 0) {
		Print (1);

		for (i=1; i <= num; i++) {
			key = Get_Control_Text (NEW_MATRIX_TABLE, i);
			if (key.empty ()) continue;

			key.Split (name, ",");
			if (name.empty ()) {
				Error (String ("New Matrix Table %d is Improperly Specified") %i);
			}
			field = new_matrix->Field_Number (name);
			if (field >= 0) {
				Error (String ("New Matrix Table %s already exists as Field %d") % name % field);
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
				Error (String ("New Matrix Table %d is Improperly Specified") %i);
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
			new_matrix->Add_Field (name, type, size, units);
		}

	} else if (data_flag) {
		for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
			for (int_itr = data_itr->table_fields.begin (); int_itr != data_itr->table_fields.end (); int_itr++) {
				fld_ptr = data_itr->file->Field (*int_itr);
				if (fld_ptr->Type () == DB_STRING || fld_ptr->Type () == DB_CHAR) continue;

				if (new_matrix->Field_Number (fld_ptr->Name ()) < 0) {
					size = (double) sizeof (double) + (fld_ptr->Decimal () / 10.0);
					new_matrix->Add_Field (fld_ptr->Name (), DB_DOUBLE, size, fld_ptr->Units (), true);
				}
			}
		}

	} else if (input_flag) {
		new_matrix->Replicate_Tables (input_matrix);
		factor = 1.0;
	} else {

		for (matrix_itr = matrix_group.begin (); matrix_itr != matrix_group.end (); matrix_itr++) {
			if (method == EXPAND_METHOD && matrix_itr->break_flag && matrix_itr != matrix_group.begin ()) continue;

			for (int_itr = matrix_itr->table_map.begin (); int_itr != matrix_itr->table_map.end (); int_itr++) {
				fld_ptr = matrix_itr->matrix->Field (*int_itr);
				if (fld_ptr->Type () == DB_STRING || fld_ptr->Type () == DB_CHAR) continue;

				if (new_matrix->Field_Number (fld_ptr->Name ()) < 0) {
					size = (double) sizeof (double) + (fld_ptr->Decimal () / 10.0);
					new_matrix->Add_Field (fld_ptr->Name (), DB_DOUBLE, size, fld_ptr->Units (), true);
				}
			}
		}
	}

	//---- determine the number of zones ----

	equiv_flag = Zone_Equiv_Flag () && !factor_flag && method != EXPAND_METHOD;

	if (equiv_flag) {
		num_org = num_des = zone_equiv.Num_Groups ();
	} else if (Max_Zone_Number () == 0) {
		if (data_flag) {
			Show_Message ("Scanning Data Files for Maximum Zone Number -- Record");
			Set_Progress ();
			Int_Set org_set, des_set;

			for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
				flag = (data_itr->factor_field >= 0);
				Str_ID factor_map;

				while (data_itr->file->Read ()) {
					Show_Progress ();
					zone = data_itr->file->Get_Integer (data_itr->org_field);
					org_set.insert (zone);

					zone = data_itr->file->Get_Integer (data_itr->des_field);
					des_set.insert (zone);

					if (flag) {
						key = data_itr->file->Get_String (data_itr->factor_field);
						key.Clean ();

						if (!key.empty ()) {
							num = (int) factor_map.size ();
							factor_map.insert (Str_ID_Data (key, num));
						}
					}
				}
				if (flag) {
					factor = (double) factor_map.size ();
				}
				data_itr->file->Rewind ();
			}
			End_Progress ();
			num_org = (int) org_set.size ();
			num_des = (int) des_set.size ();
		} else if (input_flag) {
			num_org = input_matrix->Num_Org ();
			num_des = input_matrix->Num_Des ();
		} else {
			num_org = num_des = 0;

			for (matrix_itr = matrix_group.begin (); matrix_itr != matrix_group.end (); matrix_itr++) {
				num = matrix_itr->matrix->Num_Org ();
				if (num > num_org) num_org = num;
				num = matrix_itr->matrix->Num_Des ();
				if (num > num_des) num_des = num;
			}
		}
	} else {
		num_org = num_des = Max_Zone_Number ();
	}

	//---- create the matrix and allocation memory ----
	
	new_matrix->Num_Org (num_org);
	new_matrix->Num_Des (num_des);

	if (new_flag) {
		Print (1);
		new_matrix->Create ();
	} else {
		Print (2, "Matrix Data in Memory");
		new_matrix->Set_Field_Numbers ();
	}

	if (new_matrix->Num_Periods () > 1) {
		Print (0, " (Periods=") << new_matrix->Num_Periods () << " Zones=" << new_matrix->Num_Zones () << " Tables=" << new_matrix->Tables () << ")";
	} else {
		Print (0, " (Zones=") << new_matrix->Num_Zones () << " Tables=" << new_matrix->Tables () << ")";
	}

	file_array.push_back (new_matrix);

	if (!new_matrix->Allocate_Data (true)) {
		Error ("Insufficient Memory for New Matrix");
	}
	
	//---- read new matrix factor ----

	if (Check_Control_Key (NEW_MATRIX_FACTOR)) {
		factor = Get_Control_Double (NEW_MATRIX_FACTOR);
	} else if (factor > 1.0) {
		Print (1, "New Matrix Factor = ") << factor;
	}
	if (factor == 0.0) {
		Error (String ("New Matrix Factor %lf is Out of Range (>0.0)") % factor);
	}

	//---- read new matrix method ----

	key = Get_Control_Text (NEW_MATRIX_METHOD);

	if (!key.empty ()) {
		if (method == RATIO_METHOD) {
			if (matrix_group.size () != 2) {
				Error ("Matrix Ratios Require Two Matrix Files");
			}
			min_ratio = Get_Control_Double (MINIMUM_RATIO_VALUE);
			max_ratio = Get_Control_Double (MAXIMUM_RATIO_VALUE);

			alt_matrix = new Db_Matrix ();
			alt_matrix->Copy_Periods (*new_matrix);
			alt_matrix->Num_Org (new_matrix->Num_Org ());
			alt_matrix->Num_Des (new_matrix->Num_Des ());
			alt_matrix->Tables (new_matrix->Tables ());
			alt_matrix->Type (new_matrix->Type ());
			alt_matrix->Allocate_Data (true);
			factor = 1.0;

		} else if (method == EXPAND_METHOD) {
			if (matrix_group.size () != 2) {
				Error ("Matrix Expansion Requires Two Matrix Files");
			}
		} else if (method == SELECT_METHOD) {
			if (Check_Control_Key (PERCENT_MISSING_DATA)) {
				share_missing = Get_Control_Double (PERCENT_MISSING_DATA) / 100.0;
				if (share_missing > 0.0) {
					missing_flag = true;
					random_missing.Seed (random.Seed ()+1);
				}
				if (Check_Control_Key (PROBLEM_DISTRIBUTION)) {
					Get_Control_List (PROBLEM_DISTRIBUTION, problem_distribution);

					for (Dbl_Itr itr = problem_distribution.begin (); itr != problem_distribution.end (); itr++) {
						if (*itr > 0.0 && *itr < 100.0) problem_flag = true;
						*itr /= 100.0;
					}
					random_problem.Seed (random.Seed ()+2);
				}
			}
			if (!missing_flag) {
				if (!percent_flag || select_percent == 0.0 || select_percent == 1.0) {
					Error (String ("Selection Method and Percentage %.1lf%% are incompatible") % (select_percent * 100.0) % FINISH);
				}
			}
		}
	}

	//---- read zone label file ----

	key = Get_Control_String (ZONE_LABEL_FILE);

	if (!key.empty ()) {
		Print (1);
		label_flag = true;
		label_file.File_Type ("Zone Label File");

		label_file.Open (Project_Filename (key));
	}
	Print (1);

	//---- read conversion script ----

	key = Get_Control_String (CONVERSION_SCRIPT);

	if (!key.empty ()) {
		script_flag = true;
		Print (1);
		program_file.File_Type ("Conversion Script");
		program_file.Open (Project_Filename (key));

		key = Get_Control_String (CONVERSION_EQUIV_FILE);

		if (!key.empty ()) {
			user_equiv_flag = true;
			user_equiv.File_Type ("Conversion Equiv File");
			user_equiv.Open (Project_Filename (key));
		}
	}
	
	//---- create new margin total file ----

	key = Get_Control_String (NEW_MARGIN_TOTAL_FILE);

	if (!key.empty ()) {
		margin_flag = true;
		margin_file.File_Type ("New Margin Total File");

		margin_file.Create (Project_Filename (key));
	}

	//---- create new square table file ----

	key = Get_Control_String (NEW_SQUARE_TABLE_FILE);

	if (!key.empty ()) {
		square_flag = true;
		square_file.File_Type ("New Square Table File");

		square_file.Create (Project_Filename (key));
	}

	//---- create new start time file ----

	key = Get_Control_String (NEW_START_TIME_FILE);

	if (!key.empty ()) {
		start_flag = true;
		start_file.File_Type ("New Start Time File");

		start_file.Create (Project_Filename (key));
	}

	//---- create new end time file ----

	key = Get_Control_String (NEW_END_TIME_FILE);

	if (!key.empty ()) {
		end_flag = true;
		end_file.File_Type ("New End Time File");

		end_file.Create (Project_Filename (key));
	}

	//---- read report types ----

	List_Reports ();

	if (Zone_Equiv_Flag ()) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
		Print (1);

		if (method == EXPAND_METHOD) {
			if ((int) zone_equiv.Zone_Map ()->size () > new_matrix->Num_Des ()) {
				Warning (String ("Zone=%d Equivalance is Greater than New Matrix Zones=%d") % 
					zone_equiv.Zone_Map ()->size () % new_matrix->Num_Des ());
				Show_Message (1);
			}
		}
	}

	stats_flag = Report_Flag (STATS_REPORT);

	if (script_flag) {
		if (user_equiv_flag) {
			user_equiv.Read (Report_Flag (USER_EQUIV));
		}
		Show_Message ("Compiling Conversion Script");

		if (Report_Flag (PRINT_SCRIPT)) {
			Header_Number (PRINT_SCRIPT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		program.Initialize (file_array, random.Seed () + 1);

		if (user_equiv_flag) {
			program.Set_Equiv (&user_equiv);
		}
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

