//*********************************************************
//	Read_Square.cpp - Read a Square Table File
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Read_Square_Table
//---------------------------------------------------------

bool MatrixData::Read_Square_Table (void)
{
	int zone, period, org, des, tab;
	double trips;
	bool tab_flag;
	String record;
	Strings fields;
	Str_Itr str_itr;

	//---- scan for table names ----

	input_matrix->Add_Field ("ORG", DB_INTEGER, 4, NO_UNITS, true);
	input_matrix->Add_Field ("DES", DB_INTEGER, 4, NO_UNITS, true);

	while (input_file.Read ()) {
		record = input_file.Record_String ();
		record.Parse (fields, "\t");

		if (fields [0].Equals ("Period=")) {
			input_matrix->Add_Ranges (fields [1]);
		}
	}
	if (input_matrix->Num_Periods () > 1) {
		input_matrix->Add_Field ("PERIOD", DB_INTEGER, 2, NO_UNITS, true);
	}
	input_file.Rewind ();
	input_matrix->Type (DB_DOUBLE);
	tab = input_matrix->Num_Fields ();

	while (input_file.Read ()) {
		record = input_file.Record_String ();
		record.Parse (fields, "\t");

		if (fields [0].Equals ("Table=")) {
			input_matrix->Add_Field (fields [1], DB_DOUBLE, 10.2);
		} else if (fields [0].Equals ("Zone") || fields [0].Equals ("Group") || fields [0].Equals ("Station")) {
			for (des=0, str_itr = fields.begin (); str_itr != fields.end (); str_itr++, des++) {
				if (*str_itr == "Zone" || *str_itr == "Group" || *str_itr == "Station" || *str_itr == "Total") continue;
				zone = str_itr->Integer ();
				if (zone == 0) zone = des;
				input_matrix->Add_Org (zone);
				input_matrix->Add_Des (zone);
			}
		}
	}
	if (input_matrix->Num_Fields () == tab) {
		tab_flag = false;
		input_matrix->Add_Field ("TRIPS", DB_DOUBLE, 10.2);
	} else {
		tab_flag = true;
	}

	input_matrix->Num_Org ((int) input_matrix->Org_Map ()->size ());
	input_matrix->Num_Des ((int) input_matrix->Des_Map ()->size ());

	input_file.Rewind ();
	input_matrix->Set_Field_Numbers ();

	input_matrix->Allocate_Data (true);

	//---- read into memory ----

	tab = (tab_flag) ? -1 : 0;
	org = 0;
	period = 0;

	while (input_file.Read ()) {
		record = input_file.Record_String ();
		record.Parse (fields, "\t");
		if (fields.size () < 2) continue;

		if (fields [0].Equals ("Table=")) {
			tab++;
		} else if (fields [0].Equals ("Period=")) {
			Dtime low, high;
			String low_str, high_str;
			fields [1].Range (low_str, high_str);
			low.Time_String (low_str);
			high.Time_String (high_str);
			period = input_matrix->Period (low, high);
		} else if (fields [0].Equals ("Zone") || fields [0].Equals ("Group") || fields [0].Equals ("Station")) {
			org = 0;
		} else if (fields [0].Equals ("Total")) {
		} else {
			for (des = -1, str_itr = fields.begin (); str_itr != fields.end (); str_itr++, des++) {
				if (des < 0 || *str_itr == "Total") continue;

				trips = str_itr->Double ();
				input_matrix->Set_Cell_Index (period, org, des, tab, trips);
			}
			org++;
		}
	}	
	input_file.Close ();
	return (true);
}
