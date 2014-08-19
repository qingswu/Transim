//********************************************************* 
//	Turn_Delay_File.cpp - Turn Movement File Input/Output
//*********************************************************

#include "Turn_Delay_File.hpp"

//-----------------------------------------------------------
//	Turn_Delay_File constructors
//-----------------------------------------------------------

Turn_Delay_File::Turn_Delay_File (Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();
}

Turn_Delay_File::Turn_Delay_File (string filename, Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

Turn_Delay_File::Turn_Delay_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Turn_Delay_File::Turn_Delay_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Turn_Delay_File::Setup (void)
{
	File_Type ("Turn Delay File");
	File_ID ("Turn");

	node = link = to_link = start = end = turn = time = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Turn_Delay_File::Create_Fields (void) 
{
	Add_Field ("NODE", DB_INTEGER, 10);
	Add_Field ("LINK", DB_INTEGER, 10);
	Add_Field ("TO_LINK", DB_INTEGER, 10);
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TURN", DB_DOUBLE, 10.1, PCE);
	Add_Field ("TIME", DB_DOUBLE, 10.1, SECONDS);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Turn_Delay_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field (IN_LINK_FIELD_NAMES);
	to_link = Required_Field (OUT_LINK_FIELD_NAMES);
	turn = Required_Field ("TURN", "VOLUME", "TURN_VOL", "COUNT");

	if (link < 0 || to_link < 0 || turn < 0) return (false);

	node = Optional_Field ("NODE");
	start = Optional_Field (START_FIELD_NAMES);
	end = Optional_Field (END_FIELD_NAMES);
	time = Optional_Field (TRAVEL_TIME_FIELD_NAMES);

	//---- set default units ----

	Set_Units (time, SECONDS);

	if (Version () <= 40) {
		Set_Units (start, SECONDS);
		Set_Units (end, SECONDS);
		Set_Units (turn, VEHICLES);
	} else {
		Set_Units (start, HOUR_CLOCK);
		Set_Units (end, HOUR_CLOCK);
		Set_Units (turn, PCE);
	}
	return (true);
}
