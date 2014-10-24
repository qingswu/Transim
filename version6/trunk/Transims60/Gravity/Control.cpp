//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Gravity.hpp"

#include "TDF_Matrix.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Gravity::Program_Control (void)
{
	int i, j, num, num2;
	double size, low, high;
	String key, range, format;
	Zone_File *zone_file;
	Dbls_Array dbls_array, *dbls_ptr;
	Doubles range_rec, *range_ptr; 

	//---- create the network files ----

	Data_Service::Program_Control ();

	zone_file = System_Zone_File ();

	Print (2, String ("%s Control Keys:") % Program ());

	key = Get_Control_Text (ZONE_PRODUCTION_FIELD);

	prod_field = zone_file->Required_Field (key);

	Print (0, ", Number = ") << (prod_field + 1);

	key = Get_Control_Text (ZONE_ATTRACTION_FIELD);

	attr_field = zone_file->Required_Field (key);

	Print (0, ", Number = ") << (attr_field + 1);

	//---- get the skim file ----

	Print (1);
	key = Get_Control_String (SKIM_FILE);

	key = Project_Filename (key);

	format = Db_Header::Def_Format (key);

	if (format.empty ()) {
		if (Check_Control_Key (SKIM_FORMAT)) {
			format = Get_Control_String (SKIM_FORMAT);
		}
	}
	skim_file = TDF_Matrix (READ, format);

	skim_file->File_Type ("Skim File");
	skim_file->File_ID ("Skim");

	skim_file->Open (key);

	Print (0, " (Zones=") << skim_file->Num_Des () << " Tables=" << skim_file->Tables () << ")";

	//---- skim table name ----
	
	key = Get_Control_Text (SKIM_TABLE_NAME);

	skim_field = skim_file->Table_Number (key);

	Print (0, ", Table = ") << (skim_field + 1);

	skim_units = skim_file->Table_Field (skim_field)->Units ();
		
	if (skim_units != NO_UNITS) {
		Print (0, ", Units = ") << Units_Code (skim_units);
	}
	Print (1);

	//---- get the trip table file ----

	key = Get_Control_String (TRIP_TABLE_FILE);

	if (!key.empty ()) {
		key = Project_Filename (key);

		format = Db_Header::Def_Format (key);

		if (format.empty ()) {
			if (Check_Control_Key (TRIP_TABLE_FORMAT)) {
				format = Get_Control_String (TRIP_TABLE_FORMAT);
			}
		}
		trip_file = TDF_Matrix (READ, format);

		trip_file->File_Type ("Trip Table_File");
		trip_file->File_ID ("Trip");
		trip_flag = true;

		trip_file->Open (key);

		Print (0, " (Zones=") << trip_file->Num_Des () << " Tables=" << trip_file->Tables () << ")";

		//---- trip table name ----
	
		key = Get_Control_Text (TRIP_TABLE_NAME);

		trip_field = trip_file->Table_Number (key);

		Print (0, ", Table = ") << (trip_field + 1);
		Print (1);
	}

	//---- create new trip table files ----

	key = Get_Control_String (NEW_TRIP_TABLE_FILE);

	if (!key.empty ()) {

		if (Check_Control_Key (NEW_TRIP_TABLE_FORMAT)) {
			format = Get_Control_String (NEW_TRIP_TABLE_FORMAT);
		}
		new_file = TDF_Matrix (CREATE, format);

		new_file->File_Type ("New Trip Table File");
		new_file->File_ID ("NewTrip");
		new_file->Filename (Project_Filename (key));
		new_file->Num_Org (skim_file->Num_Org ());
		new_file->Num_Des (skim_file->Num_Des ());
		new_file->Copy_Periods (*skim_file);
		new_file->Tables (1);

		size = sizeof (double) + 0.2;

		new_file->Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
		new_file->Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);
		new_file->Add_Field ("TRIPS", DB_DOUBLE, size, NO_UNITS, true);
		new_flag = true;

		new_file->Create ();

		if (!new_file->Allocate_Data ()) {
			Error (String ("Insufficient Memory for Matrix %s") % new_file->File_ID ());
		}
		Print (0, " (Zones=") << new_file->Num_Des () << " Tables=" << new_file->Tables () << ")";
	}

	//---- check for friction curves ----

	num = Highest_Control_Group (FUNCTION_PARAMETERS, 0);
	Print (1);
		
	if (num == 0) {
		Error ("At least one Friction Function is Required");
	}

	//---- input value units ----

	input_units = Units_Code (Get_Control_Text (FUNCTION_INPUT_UNITS));

	//---- input value factor ----

	value_factor = Get_Control_Double (FUNCTION_INPUT_FACTOR);

	//---- calculate the units factor ----

	units_factor = value_factor;

	if (skim_units == SECONDS || skim_units == MINUTES) {
		if (input_units != skim_units) {
			if (skim_units == SECONDS && input_units == MINUTES) {
				units_factor = 60.0;
			}
		}
		units_factor = value_factor / Scale (units_factor);
	}

	//---- calibration keys ----

	if (Check_Control_Key (CALIBRATE_FUNCTIONS)) {
		calib_flag = Get_Control_Flag (CALIBRATE_FUNCTIONS);

		if (calib_flag && !trip_flag) {
			Error ("Function Calibration Requires a Trip File");
		}
		constraints.assign (num, dbls_array);
		Print (1);
	}

	//---- friction curve ----

	for (i=1; i <= num; i++) {
		key = Get_Control_String (FUNCTION_PARAMETERS, i);

		if (key.empty ()) continue;
				
		function.Add_Function (i, key);
			
		if (calib_flag) {
			num2 = Highest_Control_Group (PARAMETER_CONSTRAINTS, i, 0);
			if (num2 < 1) continue;

			dbls_ptr = &constraints [i - 1];
			dbls_ptr->assign (num2, range_rec);

			if (num > 1 && num2 > 1) Print (1);

			for (j=1; j <= num2; j++) {
				key = Get_Control_Text (PARAMETER_CONSTRAINTS, i, j);

				range_ptr = &dbls_ptr->at (j - 1);

				key.Split (range);
				range.Range (low, high);

				range_ptr->push_back (low);
				range_ptr->push_back (high);
				if (!key.empty ()) {
					range_ptr->push_back (key.Double ());
				}
			}
		}
	}
	Print (1);

	//---- balancing method ----
	
	key = Get_Control_Text (BALANCING_METHOD);
	if (key.Equals ("PRODUCTION")) {
		balance = PROD_BAL;
	} else if (key.Equals ("ATTTACTION")) {
		balance = ATTR_BAL;
	} else if (key.Equals ("AVERAGE")) {
		balance = AVERAGE_BAL;
	} else {
		Error (String ("Unrecognized Balancing Method = %s") % key);
	}
	
	//---- maximum iterations ----

	max_iterations = Get_Control_Integer (MAXIMUM_ITERATIONS);

	//---- maximum percent difference ----

	max_percent = Get_Control_Double (MAXIMUM_PERCENT_DIFFERENCE) / 100.0;

	//---- maximum trip difference ----

	max_diff = Get_Control_Double (MAXIMUM_TRIP_DIFFERENCE);

	//---- new input length file ----

	key = Get_Control_String (NEW_INPUT_LENGTH_FILE);

	if (!key.empty ()) {
		Print (1);
		in_len_file.File_Type ("New Input Length File");
		in_len_file.File_ID ("OutLen");

		if (Check_Control_Key (NEW_INPUT_LENGTH_FORMAT)) {
			in_len_file.Dbase_Format (Get_Control_String (NEW_INPUT_LENGTH_FORMAT));
		}
		in_len_file.Add_Field ("LENGTH", DB_DOUBLE, 10.2);
		in_len_file.Add_Field ("TRIPS", DB_DOUBLE, 10.2);

		in_len_file.Create (Project_Filename (key));
		in_len_flag = true;
	}

	//---- new output length file ----

	key = Get_Control_String (NEW_OUTPUT_LENGTH_FILE);

	if (!key.empty ()) {
		Print (1);
		out_len_file.File_Type ("New Output Length File");
		out_len_file.File_ID ("OutLen");

		if (Check_Control_Key (NEW_OUTPUT_LENGTH_FORMAT)) {
			out_len_file.Dbase_Format (Get_Control_String (NEW_OUTPUT_LENGTH_FORMAT));
		}
		out_len_file.Add_Field ("LENGTH", DB_DOUBLE, 10.2);
		out_len_file.Add_Field ("TRIPS", DB_DOUBLE, 10.2);

		out_len_file.Create (Project_Filename (key));
		out_len_flag = true;
	}

	if (in_len_flag || out_len_flag || calib_flag) {
		Print (1);

		//---- trip length increment ----

		increment = Get_Control_Double (TRIP_LENGTH_INCREMENT);

		if (increment <= 0) Error ("Trip Length Increment is Out of Range");

		key (" (%s * %.3lf)") % Units_Code (input_units) % value_factor;

		Print (0, key);

		//---- maximum trip length ----

		max_value = Get_Control_Double (MAXIMUM_TRIP_LENGTH);

		if (max_value <= 0) Error ("Maximum Trip Length is Out of Range");
		
		Print (0, key);

		//---- allocate memory -----

		if (trip_flag || calib_flag) {
			in_distb.assign ((int) (max_value / increment + 1.5), 0.0);
		}
		if (new_flag) {
			out_distb.assign ((int) (max_value / increment + 1.5), 0.0);
		}
	}
}

