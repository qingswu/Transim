//*********************************************************
//	Matrix_File.cpp - Matrix File Input/Output
//*********************************************************

#include "Matrix_File.hpp"

//---------------------------------------------------------
//	Matrix_File constructor
//---------------------------------------------------------

Matrix_File::Matrix_File (Access_Type access, string format, Matrix_Type type, Units_Type od) : 
	Db_Matrix (access, format)
{
	Data_Type (type);
	OD_Units (od);
	Setup ();
}

Matrix_File::Matrix_File (string filename, Access_Type access, string format, Matrix_Type type, Units_Type od) : 
	Db_Matrix (access, format)
{
	Data_Type (type);
	OD_Units (od);
	Setup ();

	Open (filename);
}

Matrix_File::Matrix_File (Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) :
	Db_Matrix (access, format)
{
	Data_Type (type);
	OD_Units (od);
	Setup ();
}

Matrix_File::Matrix_File (string filename, Access_Type access, Format_Type format, Matrix_Type type, Units_Type od) : 
	Db_Matrix (access, format)
{
	Data_Type (type);
	OD_Units (od);
	Setup ();

	Open (filename);
}

//---------------------------------------------------------
//	Setup
//---------------------------------------------------------

void Matrix_File::Setup (void)
{
	File_Type ("Matrix File");
	File_ID ("Matrix");

	if (Data_Type () == TRIP_TABLE) {
		Type (DB_INTEGER);
	} else if (Data_Type () == DATA_TABLE) {
		Type (DB_DOUBLE);
	} else if (Data_Type () == TIME_TABLE) {
		Type (DB_INTEGER);
	} else {	//---- SKIM_TABLE ----
		Type (DB_DOUBLE);
	}
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Matrix_File::Create_Fields (void) 
{
	if (od_units != LOCATION_OD) {
		Add_Field ("ORIGIN", DB_UNSIGNED, 2, od_units, true);
		Add_Field ("DESTINATION", DB_UNSIGNED, 2, od_units, true);
	} else {
		Add_Field ("ORIGIN", DB_INTEGER, 10, od_units);
		Add_Field ("DESTINATION", DB_INTEGER, 10, od_units);
	}
	Add_Field ("PERIOD", DB_UNSIGNED, 1, NO_UNITS, true);

	if (Data_Type () == TRIP_TABLE) {
		Add_Field ("TRIPS", DB_INTEGER, 4, NO_UNITS, true);
	} else if (Data_Type () == DATA_TABLE) {
		Add_Field ("FACTOR", DB_DOUBLE, 2.6, NO_UNITS, true);
	} else {
		Add_Field ("COUNT", DB_INTEGER, 2, NO_UNITS, true);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Matrix_File::Set_Field_Numbers (void)
{
	//data = Optional_Field ("DATA", "TRIPS", "COUNT", "FACTOR", "TIME");

	if (Optional_Field ("TRIPS") >= 0) {
		Data_Type (TRIP_TABLE);
	} else if (Optional_Field ("DATA", "FACTOR") >= 0) {
		Data_Type (DATA_TABLE);
	} else if (Optional_Field ("TIME") >= 0) {
		Data_Type (TIME_TABLE);
	} else {
		Data_Type (SKIM_TABLE);
	}
	return (Db_Matrix::Set_Field_Numbers ());
}

