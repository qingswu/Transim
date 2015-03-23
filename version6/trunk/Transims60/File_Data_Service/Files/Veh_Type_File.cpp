//********************************************************* 
//	Veh_Type_File.cpp - Vehicle Type File Input/Output
//*********************************************************

#include "Veh_Type_File.hpp"

//-----------------------------------------------------------
//	Veh_Type_File constructors
//-----------------------------------------------------------

Veh_Type_File::Veh_Type_File (Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();
}

Veh_Type_File::Veh_Type_File (string filename, Access_Type access, string format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

Veh_Type_File::Veh_Type_File (Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();
}

Veh_Type_File::Veh_Type_File (string filename, Access_Type access, Format_Type format) : 
	Db_Header (access, format)
{
	Setup ();

	Open (filename);
}

//-----------------------------------------------------------
//	Setup
//-----------------------------------------------------------

void Veh_Type_File::Setup (void)
{
	File_Type ("Vehicle Type File");
	File_ID ("VehType");

	type = length = max_speed = max_accel = max_decel = op_cost = use = capacity = max_load = -1;
	occupancy = load = unload = method = min_dwell = max_dwell = subtype = -1;
	grade = fuel = fuel_cap = -1;
}

//---------------------------------------------------------
//	Create_Fields
//---------------------------------------------------------

bool Veh_Type_File::Create_Fields (void) 
{
	Add_Field ("TYPE", DB_INTEGER, 4, VEH_TYPE);
	Add_Field ("LENGTH", DB_DOUBLE, 5.1, FEET);
	Add_Field ("MAX_SPEED", DB_DOUBLE, 5.1, MPH);
	Add_Field ("MAX_ACCEL", DB_DOUBLE, 5.1, FPS2);
	Add_Field ("MAX_DECEL", DB_DOUBLE, 5.1, FPS2);
	Add_Field ("OP_COST", DB_DOUBLE, 5.1, CENT_MI);

	if (Code_Flag ()) {
		Add_Field ("USE", DB_UNSIGNED, 2, VEH_USE_CODE, true);
	} else {
		Add_Field ("USE", DB_STRING, 20, VEH_USE_CODE);
	}
	Add_Field ("CAPACITY", DB_UNSIGNED, 4, PPV);
	Add_Field ("MAX_LOAD", DB_UNSIGNED, 4, PPV);
	Add_Field ("OCCUPANCY", DB_DOUBLE, 8.2, PPV);
	Add_Field ("LOADING", DB_DOUBLE, 6.2, SPP);
	Add_Field ("UNLOADING", DB_DOUBLE, 6.2, SPP);
	if (Code_Flag ()) {
		Add_Field ("METHOD", DB_UNSIGNED, 1, LOADING_CODE, true);
	} else {
		Add_Field ("METHOD", DB_STRING, 20, LOADING_CODE);
	}
	Add_Field ("MIN_DWELL", DB_TIME, 6.1, SECONDS);
	Add_Field ("MAX_DWELL", DB_TIME, 6.1, SECONDS);
	Add_Field ("GRADE_FUNC", DB_INTEGER, 3);
	Add_Field ("FUEL_FUNC", DB_INTEGER, 3);
	Add_Field ("FUEL_CAP", DB_INTEGER, 5.1, GALLONS);

	if (exe->Notes_Name_Flag ()) {
		Add_Field ("NOTES", DB_STRING, STRING_FIELD_SIZE);
	}
	return (Set_Field_Numbers ());
}

//-----------------------------------------------------------
//	Set_Field_Numbers
//-----------------------------------------------------------

bool Veh_Type_File::Set_Field_Numbers (void)
{

	//---- required fields ----

	type = Required_Field ("TYPE", "VEHTYPE", "VEH_TYPE");
	length = Required_Field ("LENGTH", "SIZE");
	max_speed = Required_Field ("MAX_SPEED", "MAXVEL", "MAXSPEED", "MAXSPD");
	max_accel = Required_Field ("MAX_ACCEL", "MAXACCEL");
	capacity = Required_Field ("CAPACITY", "PASSENGERS");
	
	if (type < 0 || length < 0 || max_speed < 0 || max_accel < 0 || capacity < 0) return (false);

	//---- optional fields ----

	occupancy = Optional_Field ("OCCUPANCY", "PASSENGERS", "AVG_OCC");
	max_decel = Optional_Field ("MAX_DECEL", "MAXDECEL");
	op_cost = Optional_Field ("OP_COST", "OPCOST", "COST");
	use = Optional_Field ("USE", "ACCESS");
	max_load = Optional_Field ("MAX_LOAD", "MAXLOAD", "MAX_CAP");
	load = Optional_Field ("LOADING", "LOAD", "BOARD", "BOARDING", "ON_RATE");
	unload = Optional_Field ("UNLOADING", "UNLOAD", "ALIGHT", "ALIGHTING", "OFF_RATE");
	method = Optional_Field ("METHOD", "LOAD_TYPE", "LOAD_METHOD");
	min_dwell = Optional_Field ("MIN_DWELL", "MINDWELL", "DWELL");
	max_dwell = Optional_Field ("MAX_DWELL", "MAXDWELL", "DWELL");

	grade = Optional_Field ("GRADE", "GRADE_FUNC", "GRADE_FUNCTION", "GRADE_NUM");
	fuel = Optional_Field ("FUEL", "FUEL_FUNC", "FUEL_FUNCTION", "FUEL_NUM");
	fuel_cap = Optional_Field ("FUEL_CAP", "MAX_FUEL", "TANK_SIZE", "FUELCAP");

	//---- Version 4.0 compatibility ----

	subtype = Optional_Field ("SUBTYPE", "SUB_TYPE", "VSUBTYPE");
	Notes_Field (Optional_Field (NOTES_FIELD_NAMES));

	//---- set default units ----

	if (Version () <= 40) {
		Set_Units (type, VEH_TYPE);
		Set_Units (length, METERS);
		Set_Units (max_speed, MPS);
		Set_Units (max_accel, MPS2);
		Set_Units (max_decel, MPS2);
		Set_Units (op_cost, CENT_KM);
		Set_Units (use, VEH_USE_CODE);
		Set_Units (capacity, PPV);
		Set_Units (max_load, PPV);
		Set_Units (occupancy, PPV);
		Set_Units (load, SPP);
		Set_Units (unload, SPP);
		Set_Units (method, LOADING_CODE);
		Set_Units (min_dwell, SECONDS);
		Set_Units (max_dwell, SECONDS);
		Set_Units (fuel_cap, LITERS);
	} else {
		Set_Units (type, VEH_TYPE);
		Set_Units (length, FEET);
		Set_Units (max_speed, MPH);
		Set_Units (max_accel, FPS2);
		Set_Units (max_decel, FPS2);
		Set_Units (op_cost, CENT_MI);
		Set_Units (use, VEH_USE_CODE);
		Set_Units (capacity, PPV);
		Set_Units (max_load, PPV);
		Set_Units (occupancy, PPV);
		Set_Units (load, SPP);
		Set_Units (unload, SPP);
		Set_Units (method, LOADING_CODE);
		Set_Units (min_dwell, SECONDS);
		Set_Units (max_dwell, SECONDS);
		Set_Units (fuel_cap, GALLONS);
	}

	return (true);
}
