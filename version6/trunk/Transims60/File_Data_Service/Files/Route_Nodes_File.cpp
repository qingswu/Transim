//********************************************************* 
//	Route_Nodes_File.cpp - Route_Nodes File Input/Output
//*********************************************************

#include "Route_Nodes_File.hpp"

//-----------------------------------------------------------
//	Route_Nodes_File constructors
//-----------------------------------------------------------

Route_Nodes_File::Route_Nodes_File (Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();
}

Route_Nodes_File::Route_Nodes_File (string filename, Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

Route_Nodes_File::Route_Nodes_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Route_Nodes_File::Route_Nodes_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Route_Nodes_File::Setup (void)
{
	File_Type ("Route Nodes File");
	File_ID ("RouteNodes");
	
	if (Model_Format () == TPPLUS) {
		Header_Lines (0);
		Dbase_Format (UNFORMATED);
	} else {
		Nest (NESTED);
		Header_Lines (2);
	}
	dwell_flag = time_flag = speed_flag = type_flag = ttime_flag = offset_flag = true;
	pattern_flag = false;

	num_periods = 8;
	route = mode = veh_type = oneway = nodes = name = node = type = dwell = time = speed = -1;
	memset (headway, -1, sizeof (headway));
	memset (offset, -1, sizeof (offset));
	memset (ttime, -1, sizeof (ttime));
	memset (pattern, -1, sizeof (pattern));
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Route_Nodes_File::Create_Fields (void) 
{
	if (Model_Format () == TPPLUS) return (true);
	String field;

	Add_Field ("ROUTE", DB_INTEGER, 10);
	if (Code_Flag ()) {
		Add_Field ("MODE", DB_UNSIGNED, 1, TRANSIT_CODE, true);
	} else {
		Add_Field ("MODE", DB_STRING, 16, TRANSIT_CODE);
	}
	Add_Field ("VEH_TYPE", DB_INTEGER, 4, VEH_TYPE);
	Add_Field ("ONEWAY", DB_INTEGER, 2);
	Add_Field ("NODES", DB_INTEGER, 2, NEST_COUNT, true);
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NAME", DB_STRING, 40);
	}
	for (int i=1; i <= num_periods; i++) {
		field ("HEADWAY%d") % i;
		Add_Field (field, DB_TIME, 2, MINUTES, true);

		if (offset_flag) {
			field ("OFFSET%d") % i;
			Add_Field (field, DB_TIME, 2, MINUTES, true);
		}
		if (ttime_flag) {
			field ("TTIME%d") % i;
			Add_Field (field, DB_TIME, 4, MINUTES, true);
		}
		if (pattern_flag) {
			field ("PATTERN%d") % i;
			Add_Field (field, DB_INTEGER, 4, NO_UNITS, true);
		}
	}
	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	Add_Field ("NODE", DB_INTEGER, 4, NO_UNITS, true, NESTED);
	if (Code_Flag ()) {
		if (type_flag) Add_Field ("TYPE", DB_INTEGER, 1, STOP_CODE, true, NESTED);
	} else {
		if (type_flag) Add_Field ("TYPE", DB_STRING, 16, STOP_CODE, false, NESTED);
	}
	if (dwell_flag) Add_Field ("DWELL", DB_TIME, 2, SECONDS, true, NESTED);
	if (time_flag) Add_Field ("TIME", DB_TIME, 2, SECONDS, true, NESTED);
	if (speed_flag) Add_Field ("SPEED", DB_DOUBLE, 4.1, MPH, true, NESTED);

	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Route_Nodes_File::Set_Field_Numbers (void)
{
	if (Model_Format () == TPPLUS) return (true);

	String field1, field2, field3, field4;

	//---- required fields ----

	route = Required_Field (ROUTE_FIELD_NAMES);
	mode = Required_Field (MODE_FIELD_NAMES);
	node = Required_Field ("NODE", "N", "NOD", "STOP");
	nodes = Required_Field ("NODES", "NNODES", "NUM_NODES");

	if (route < 0 || mode < 0 || node < 0 || nodes < 0) return (false);

	//---- optional fields ----

	veh_type = Optional_Field ("VEHICLE", "VEH_TYPE", "VEHTYPE");
	oneway = Optional_Field ("ONEWAY", "DIRECTION" , "DIR");
	Num_Nest_Field (nodes);

	if (exe->Notes_Name_Flag ()) {
		name = Optional_Field (ROUTE_NAME_FIELD_NAMES);
	}
	Notes_Field (Optional_Field (NOTES_FIELD_NAMES));

	ttime_flag = offset_flag = false;

	for (int i=1; i <= 24; i++) {
		field1 ("HEADWAY%d") % i;
		field2 ("HEAD%d") % i;
		field3 ("FREQ%d") % i;
		field4 ("HEADWAY_%d") % i;
		headway [i-1] = Optional_Field (field1.c_str (), field2.c_str (), field3.c_str (), field4.c_str ());

		field1 ("OFFSET%d") % i;
		field2 ("OFFSET_%d") % i;
		offset [i-1] = Optional_Field (field1.c_str (), field2.c_str ());
		if (offset [i-1] >= 0) offset_flag = true;
		
		field1 ("TTIME%d") % i;
		field2 ("TTIME_%d") % i;
		field3 ("TTIME");
		ttime [i-1] = Optional_Field (field1.c_str (), field2.c_str (), field3.c_str ());
		if (ttime [i-1] >= 0) ttime_flag = true;

		field1 ("PATTERN%d") % i;
		field2 ("PATTERN_%d") % i;
		field3 ("PATTERN");
		pattern [i-1] = Optional_Field (field1.c_str (), field2.c_str (), field3.c_str ());
		if (pattern [i-1] >= 0) pattern_flag = true;		

		if (headway [i-1] < 0 && offset [i-1] < 0 && ttime [i-1] < 0 && pattern [i-1] < 0) break;

		num_periods = i;

		if (Version () <= 40) {
			Set_Units (headway [i-1], SECONDS);
			Set_Units (offset [i-1], SECONDS);
			Set_Units (ttime [i-1], SECONDS);
		} else {
			Set_Units (headway [i-1], MINUTES);
			Set_Units (offset [i-1], MINUTES);
			Set_Units (ttime [i-1], MINUTES);
		}
	}
	type = Optional_Field ("TYPE", "STOP_TYPE", "STYLE");
	dwell = Optional_Field ("DWELL", "DELAY");
	time = Optional_Field ("TIME");
	speed = Optional_Field ("SPEED");

	type_flag = (type >= 0);
	dwell_flag = (dwell >= 0);
	time_flag = (time >= 0);
	speed_flag = (speed >= 0);

	//---- set default units ----

	if (Version () <= 40) {
		Set_Units (mode, TRANSIT_CODE);
		Set_Units (veh_type, VEH_TYPE);
		Set_Units (dwell, SECONDS);
		Set_Units (time, SECONDS);
		Set_Units (speed, MPS);
		Set_Units (type, STOP_CODE);
	} else {
		Set_Units (mode, TRANSIT_CODE);
		Set_Units (veh_type, VEH_TYPE);
		Set_Units (dwell, SECONDS);
		Set_Units (time, SECONDS);
		Set_Units (speed, MPH);
		Set_Units (type, STOP_CODE);
	}
	return (true);
}
