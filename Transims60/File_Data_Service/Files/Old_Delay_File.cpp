//********************************************************* 
//	Old_Delay_File.cpp - Link Delay / Performance File
//*********************************************************

#include "Old_Delay_File.hpp"

//-----------------------------------------------------------
//	Old_Delay_File constructors
//-----------------------------------------------------------

Old_Delay_File::Old_Delay_File (Access_Type access, string format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Db_Header (access, format)
{
	Flow_Units (flow);
	Turn_Flag (turn_flag);
	Lane_Use_Flows (flow_flag);
	Setup ();
}

Old_Delay_File::Old_Delay_File (string filename, Access_Type access, string format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Db_Header (access, format)
{
	Flow_Units (flow);
	Turn_Flag (turn_flag);
	Lane_Use_Flows (flow_flag);
	Setup ();

	Open (filename);
}

Old_Delay_File::Old_Delay_File (Access_Type access, Format_Type format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Db_Header (access, format)
{
	Flow_Units (flow);
	Turn_Flag (turn_flag);
	Lane_Use_Flows (flow_flag);
	Setup ();
}

Old_Delay_File::Old_Delay_File (string filename, Access_Type access, Format_Type format, Units_Type flow, bool turn_flag, bool flow_flag) : 
	Db_Header (access, format)
{
	Flow_Units (flow);
	Turn_Flag (turn_flag);
	Lane_Use_Flows (flow_flag);
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Old_Delay_File::Setup (void)
{
	File_Type ("Old Delay File");
	File_ID ("Delay");

	Set_Nesting (turn_flag);

	link = dir = type = start = end = flow = time = -1;
	out_link = out_flow = out_time = -1;

	speed = delay = density = max_den = ratio = queue = max_que = fail = -1;
	tod = volume = enter = exit = vmt = vht = 0;

	increment = 900;
	iteration = 1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Old_Delay_File::Create_Fields (void) 
{
	Add_Field ("LINK", DB_INTEGER, 4, NO_UNITS, true);
	Add_LinkDir_Field ();
	if (type_flag) {
		Add_Field ("TYPE", DB_INTEGER, 1);
	}
	Add_Field ("START", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("END", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
	Add_Field ("FLOW", DB_DOUBLE, 4.2, flow_units, true);
	Add_Field ("TIME", DB_DOUBLE, 4.1, SECONDS, true);

	Add_Field ("AVG_SPEED", DB_DOUBLE, 4.1, MPH, true);
	Add_Field ("AVG_DELAY", DB_DOUBLE, 4.1, SECONDS, true);
	Add_Field ("AVG_DENSITY", DB_DOUBLE, 4.1, LANE_KM, true);
	Add_Field ("MAX_DENSITY", DB_DOUBLE, 4.1, LANE_KM, true);
	Add_Field ("TIME_RATIO", DB_DOUBLE, 4.3, RATIO, true);
	Add_Field ("AVG_QUEUE", DB_DOUBLE, 4.1, STOPPED, true);
	Add_Field ("MAX_QUEUE", DB_INTEGER, 4, STOPPED, true);
	Add_Field ("NUM_FAIL", DB_INTEGER, 4, Flow_Units (), true);

	Add_Field ("AVG_VOLUME", DB_INTEGER, 10);
	Add_Field ("IN_VOLUME", DB_INTEGER, 10);
	Add_Field ("OUT_VOLUME", DB_INTEGER, 10);
	Add_Field ("VMT", DB_DOUBLE, 12.1);
	Add_Field ("VHT", DB_DOUBLE, 12.1);

	if (turn_flag) {
		Nest (NESTED);
		Header_Lines (2);
		Add_Field ("NCONNECT", DB_INTEGER, 2);
		Add_Field ("OUT_LINK", DB_INTEGER, 4, NO_UNITS, true, NESTED);
		Add_Field ("OUT_FLOW", DB_DOUBLE, 4.2, flow_units, true, NESTED);
		Add_Field ("OUT_TIME", DB_DOUBLE, 4.1, SECONDS, true, NESTED);
	} else {
		Nest (NO_NEST);
		Header_Lines (1);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Old_Delay_File::Set_Field_Numbers (void)
{
	bool flag = false;

	//---- required fields ----

	link = Required_Field ("LINK");
	flow = Required_Field (FLOW_RATE_FIELD_NAMES);

	if (Version () <= 40) {
		start = Optional_Field (START_FIELD_NAMES);
		end = Optional_Field (END_FIELD_NAMES);

		if (start < 0 && end < 0) {
			start = end = Required_Field ("PERIOD", "TIME");
			time = Required_Field ("AVG_TIME", "TTIME", "SUM");
			flag = true;
		} else {
			time = Required_Field (TRAVEL_TIME_FIELD_NAMES);
		}
	} else {
		time = Required_Field (TRAVEL_TIME_FIELD_NAMES);
		start = Required_Field (START_FIELD_NAMES);
		end = Required_Field (END_FIELD_NAMES);
	}

	if (link < 0 || flow < 0 || time < 0 || start < 0 || end < 0) return (false);

	//---- optional fields ----

	dir = LinkDir_Type_Field ();
	type = Optional_Field (FLOW_TYPE_FIELD_NAMES);

	speed = Optional_Field ("AVG_SPEED", "SPEED", "VSUM");
	delay = Optional_Field ("AVG_DELAY", "DELAY");
	density = Optional_Field ("AVG_DENSITY", "DENSITY");
	max_den = Optional_Field ("MAX_DENSITY", "MAX_DEN");
	ratio = Optional_Field ("TIME_RATIO", "RATIO");
	queue = Optional_Field ("AVG_QUEUE", "QUEUE");
	max_que = Optional_Field ("MAX_QUEUE", "MAX_QUE");
	fail = Optional_Field ("NUM_FAIL", "CYCLE_FAIL", "FAILURE");

	//---- other Version 4 fields ----

	tod = Optional_Field ("PERIOD", "TOD");
	volume = Optional_Field ("AVG_VOLUME", "VOLUME", "COUNT");
	enter = Optional_Field ("IN_VOLUME", "ENTER");
	exit = Optional_Field ("OUT_VOLUME", "EXIT");
	vmt = Optional_Field ("VMT", "VEH_DIST", "FLOW_DIST");
	vht = Optional_Field ("VHT", "VEH_TIME", "FLOW_TIME");

	//---- nested optional fields ----

	Num_Nest_Field (Optional_Field (CONNECTION_FIELD_NAMES));
	if (Num_Nest_Field () < 0) turn_flag = false;

	out_link = Optional_Field (OUT_LINK_FIELD_NAMES);
	out_flow = Optional_Field (FLOW_OUT_FIELD_NAMES);
	out_time = Optional_Field (TIME_OUT_FIELD_NAMES);

	//---- set default units ----

	if (flag) {
		Set_Units (start, SECONDS);
		Set_Units (end, SECONDS);
	} else {
		Set_Units (start, HOUR_CLOCK);
		Set_Units (end, HOUR_CLOCK);
	}
	Set_Units (flow, flow_units);
	Set_Units (time, SECONDS);
	Set_Units (out_flow, flow_units);
	Set_Units (out_time, SECONDS);

	Units_Type units = Get_Units (flow);
	if (units != NO_UNITS) Flow_Units (units);

	Lane_Use_Flows ((type >= 0));

	//---- set default units ----

	Set_Units (delay, SECONDS);
	Set_Units (ratio, RATIO);
	Set_Units (queue, STOPPED);
	Set_Units (max_que, STOPPED);
	Set_Units (fail, Flow_Units ());
	Set_Units (volume, Flow_Units ());
	Set_Units (enter, Flow_Units ());
	Set_Units (exit, Flow_Units ());

	if (Version () <= 40) {
		Set_Units (speed, MPS);
		Set_Units (density, LANE_KM);
		Set_Units (max_den, LANE_KM);
		Set_Units (vmt, METERS);
		Set_Units (vht, SECONDS);
	} else {
		Set_Units (speed, MPH);
		Set_Units (delay, SECONDS);
		Set_Units (density, LANE_MILE);
		Set_Units (max_den, LANE_MILE);

		if (Flow_Units () == PERSONS) {
			if (Metric_Flag ()) {
				Set_Units (vmt, PKT);
			} else {
				Set_Units (vmt, PMT);
			}
			Set_Units (vht, PHT);
		} else if (Flow_Units () == VEHICLES) {
			if (Metric_Flag ()) {
				Set_Units (vmt, VKT);
			} else {
				Set_Units (vmt, VMT);
			}
			Set_Units (vht, VHT);
		} else {
			if (Metric_Flag ()) {
				Set_Units (vmt, EKT);
			} else {
				Set_Units (vmt, EMT);
			}
			Set_Units (vht, EHT);
		}
	}
	return (true);
}
