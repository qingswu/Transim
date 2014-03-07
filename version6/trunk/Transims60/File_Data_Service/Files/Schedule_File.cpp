//********************************************************* 
//	Schedule_File.cpp - Transit Schedule File Input/Output
//*********************************************************

#include "Schedule_File.hpp"

//-----------------------------------------------------------
//	Schedule_File constructors
//-----------------------------------------------------------

Schedule_File::Schedule_File (Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();
}

Schedule_File::Schedule_File (string filename, Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

Schedule_File::Schedule_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Schedule_File::Schedule_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Schedule_File::Setup (void)
{
	File_Type ("Transit Schedule File");
	File_ID ("Schedule");

	Nest (NESTED);
	Header_Lines (2);

	route = stops = stop = name = label = -1;
	memset (run, '\0', sizeof (run));
	memset (time, '\0', sizeof (time));
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Schedule_File::Create_Fields (void) 
{
	int i;
	String field;

	Add_Field ("ROUTE", DB_INTEGER, 10);

	for (i=1; i <= NUM_SCHEDULE_COLUMNS; i++) {
		field ("RUN%d") % i;
		Add_Field (field, DB_INTEGER, 4);
	}
	Add_Field ("STOPS", DB_INTEGER, 4, NEST_COUNT);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("LABEL", DB_STRING, 40);
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	Add_Field ("STOP", DB_INTEGER, 4, NO_UNITS, true, NESTED);

	for (i=1; i <= NUM_SCHEDULE_COLUMNS; i++) {
		field ("TIME%d") % i;
		Add_Field (field, DB_TIME, TIME_FIELD_SIZE, Time_Format (), false, NESTED);
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NAME", DB_STRING, 50, NO_UNITS, false, NESTED);
	}
	return (Set_Field_Numbers ());
}

//---------------------------------------------------------
//	Clear_Columns
//---------------------------------------------------------

void Schedule_File::Clear_Columns (void) 
{
	for (int i=0; i < NUM_SCHEDULE_COLUMNS; i++) {
		Blank_Field (run [i]);
		Blank_Field (time [i]);
	}
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Schedule_File::Set_Field_Numbers (void)
{
	int i;
	String option1, option2;

	//---- required fields ----

	route = Required_Field (ROUTE_FIELD_NAMES);
	stop = Required_Field ("STOP", "STOPID");

	if (route < 0 || stop < 0) return (false);

	//---- optional fields ----

	stops = Optional_Field ("STOPS", "NSTOPS", "NUM_STOPS", "NUM_NEST");
	Num_Nest_Field (stops);

	Notes_Field (Optional_Field (NOTES_FIELD_NAMES));
	if (exe->Notes_Name_Flag ()) {
		label = Optional_Field ("LABEL", "ROUTE_NAME", "RTNAME", "CODE");
		name = Optional_Field ("NAME", "STOP_NAME", "STNAME", "DESCRIPTION");
	}

	for (i=1; i <= NUM_SCHEDULE_COLUMNS; i++) {
		option1 ("RUN%d") % i;
		option2 ("RUN_NUM%d") % i;

		if (i == 1) {
			run [0] = Optional_Field (option1.c_str (), option2.c_str (), "RUN", "RUN_NUM");
		} else {
			run [i-1] = Optional_Field (option1.c_str (), option2.c_str ());
		}
		option1 ("TIME%d") % i;
		option2 ("DEPART%d") % i;

		if (i == 1) {
			time [0] = Required_Field (option1.c_str (), option2.c_str (), "TIME", "DEPART");
		} else {
			time [i-1] = Optional_Field (option1.c_str (), option2.c_str ());
		}

		//---- set default units ----

		if (Version () <= 40) {
			Set_Units (time [i-1], SECONDS);
		} else {
			Set_Units (time [i-1], HOUR_CLOCK);
		}
	}
	return (true);
}

//-----------------------------------------------------------
//	Run_Type
//-----------------------------------------------------------

void Schedule_File::Run_Type (int num, int value)
{
	if (Check (num)) {
		int fld = time [num];
		Field_Ptr ptr = Field (fld);

		Units_Type units = ptr->Units ();
		Field_Type type = ptr->Type ();

		ptr->Units (NO_UNITS);
		ptr->Type (DB_INTEGER);

		Put_Field (fld, value);

		ptr->Units (units);
		ptr->Type (type);
	}
}
