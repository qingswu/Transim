//********************************************************* 
//	Performance_File.cpp - Link Performance Input/Output
//*********************************************************

#include "Performance_File.hpp"

//-----------------------------------------------------------
//	Performance_File constructors
//-----------------------------------------------------------

Performance_File::Performance_File (Access_Type access, string format, bool lane_use_flag) : 
	Db_Header (access, format)
{
	Lane_Use_Flag (lane_use_flag);
	Setup ();
}

Performance_File::Performance_File (string filename, Access_Type access, string format, bool lane_use_flag) : 
	Db_Header (access, format)
{
	Lane_Use_Flag (lane_use_flag);
	Setup ();

	Open (filename);
}

Performance_File::Performance_File (Access_Type access, Format_Type format, bool lane_use_flag) : 
	Db_Header (access, format)
{
	Lane_Use_Flag (lane_use_flag);
	Setup ();
}

Performance_File::Performance_File (string filename, Access_Type access, Format_Type format, bool lane_use_flag) : 
	Db_Header (access, format)
{
	Lane_Use_Flag (lane_use_flag);
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Performance_File::Setup (void)
{
	File_Type ("Performance File");
	File_ID ("Performance");

	link = dir = type = start = end = time = persons = volume = enter = exit = flow = -1;
	speed = ratio = delay = density = max_density = queue = max_queue = failure = veh_dist = veh_time = veh_delay = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Performance_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 4, NO_UNITS, true);
	Add_LinkDir_Field ();
	if (lane_use_flag) {
		Add_Field ("TYPE", DB_INTEGER, 1);
	}
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("TTIME", DB_DOUBLE, 4.1, SECONDS, true);
	Add_Field ("PERSONS", DB_DOUBLE, 4.1, PERSONS, true);
	Add_Field ("VOLUME", DB_DOUBLE, 4.1, PCE, true);
	Add_Field ("ENTER", DB_DOUBLE, 4.1, PCE, true);
	Add_Field ("EXIT", DB_DOUBLE, 4.1, PCE, true);
	Add_Field ("FLOW", DB_DOUBLE, 4.2, PCE, true);
	Add_Field ("AVG_SPEED", DB_DOUBLE, 4.1, MPH, true);
	Add_Field ("TIME_RATIO", DB_DOUBLE, 4.3, RATIO, true);
	Add_Field ("AVG_DELAY", DB_DOUBLE, 4.1, SECONDS, true);
	Add_Field ("AVG_DENSITY", DB_DOUBLE, 4.1, LANE_MILE, true);
	Add_Field ("MAX_DENSITY", DB_DOUBLE, 4.1, LANE_MILE, true);
	Add_Field ("AVG_QUEUE", DB_DOUBLE, 4.1, STOPPED, true);
	Add_Field ("MAX_QUEUE", DB_DOUBLE, 4.1, STOPPED, true);
	Add_Field ("NUM_FAIL", DB_DOUBLE, 4.1, PCE, true);
	Add_Field ("VEH_DIST", DB_DOUBLE, 8.3, EMT, true);
	Add_Field ("VEH_TIME", DB_DOUBLE, 8.3, EHT, true);
	Add_Field ("VEH_DELAY", DB_DOUBLE, 4.3, EHD, true);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Performance_File::Set_Field_Numbers (void)
{
	//---- required fields ----

	link = Required_Field ("LINK");

	if (Version () <= 40) {
		start = Optional_Field (START_FIELD_NAMES);
		end = Optional_Field (END_FIELD_NAMES);

		if (start < 0 && end < 0) {
			start = end = Required_Field ("PERIOD", "TIME");
			time = Required_Field ("AVG_TIME", "TTIME", "SUM");

			Set_Units (start, SECONDS);
			Set_Units (end, SECONDS);
		} else {
			time = Required_Field (TRAVEL_TIME_FIELD_NAMES);
			Set_Units (start, HOUR_CLOCK);
			Set_Units (end, HOUR_CLOCK);
		}
	} else {
		time = Required_Field (TRAVEL_TIME_FIELD_NAMES);
		start = Required_Field (START_FIELD_NAMES);
		end = Required_Field (END_FIELD_NAMES);

		Set_Units (start, HOUR_CLOCK);
		Set_Units (end, HOUR_CLOCK);
	}
	volume = Required_Field ("VOLUME", "PCE", "VEHICLES", "FLOW");

	if (link < 0 || volume < 0 || time < 0 || start < 0 || end < 0) return (false);

	//---- optional fields ----

	dir = LinkDir_Type_Field ();

	type = Optional_Field (FLOW_TYPE_FIELD_NAMES);
	Lane_Use_Flag ((type >= 0));
	
	persons = Optional_Field ("PERSONS", "OCCUPANCY", "PEOPLE");
	enter = Optional_Field ("ENTER", "IN_VOLUME", "IN_VOL");
	exit = Optional_Field ("EXIT", "OUT_VOLUME", "OUT_VOL");
	flow = Optional_Field (FLOW_RATE_FIELD_NAMES);
	speed = Optional_Field ("AVG_SPEED", "SPEED", "VSUM");
	ratio = Optional_Field ("TIME_RATIO", "RATIO");
	delay = Optional_Field ("AVG_DELAY", "DELAY");
	density = Optional_Field ("AVG_DENSITY", "DENSITY");
	max_density = Optional_Field ("MAX_DENSITY", "MAX_DEN");
	queue = Optional_Field ("AVG_QUEUE", "QUEUE");
	max_queue = Optional_Field ("MAX_QUEUE", "MAX_QUE");
	failure = Optional_Field ("NUM_FAIL", "CYCLE_FAIL", "FAILURE");
	veh_dist = Optional_Field ("VEH_DIST", "VMT", "VEH_MILES", "VKT");
	veh_time = Optional_Field ("VEH_TIME", "VHT", "VEH_HOURS");
	veh_delay = Optional_Field ("VEH_DELAY", "VHD", "TOT_DELAY");

	//---- set default units ----

	Set_Units (time, SECONDS);
	Set_Units (persons, PERSONS);
	Set_Units (flow, PCE);
	Set_Units (ratio, RATIO);
	Set_Units (delay, SECONDS);

	if (Version () <= 40) {
		Set_Units (volume, VEHICLES);
		Set_Units (enter, VEHICLES);
		Set_Units (exit, VEHICLES);
		Set_Units (speed, MPS);
		Set_Units (density, LANE_KM);
		Set_Units (max_density, LANE_KM);
		Set_Units (queue, STOPPED);
		Set_Units (max_queue, STOPPED);
		Set_Units (failure, PCE);
		Set_Units (veh_dist, VKT);
		Set_Units (veh_time, VHT);
		Set_Units (veh_delay, VHT);
	} else {
		Set_Units (volume, PCE);
		Set_Units (enter, PCE);
		Set_Units (exit, PCE);
		Set_Units (speed, MPH);
		Set_Units (density, LANE_MILE);
		Set_Units (max_density, LANE_MILE);
		Set_Units (queue, PCE);
		Set_Units (max_queue, PCE);
		Set_Units (failure, PCE);
		Set_Units (veh_dist, EMT);
		Set_Units (veh_time, EHT);
		Set_Units (veh_delay, EHD);
	}
	return (true);
}
