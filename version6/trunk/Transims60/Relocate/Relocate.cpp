//*********************************************************
//	Relocate.cpp - move trips and plans to a new network
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Relocate constructor
//---------------------------------------------------------

Relocate::Relocate (void) : Data_Service (), Select_Service () 
{
	Program ("Relocate");
<<<<<<< .working
	Version (7);
=======
	Version (13);
>>>>>>> .merge-right.r1529
	Title ("Move Trips and Plans to a New Network");
	
	System_File_Type required_files [] = {
		NODE, LINK, LOCATION, END_FILE
	};
	System_File_Type optional_files [] = {
<<<<<<< .working
		SHAPE, PARKING, ACCESS_LINK, TRANSIT_STOP, TRANSIT_ROUTE, 
		PLAN, TRIP, PERFORMANCE, TURN_DELAY, 
		NEW_TRIP, NEW_PLAN, NEW_SELECTION, NEW_PERFORMANCE, NEW_TURN_DELAY, END_FILE
=======
		SHAPE, CONNECTION, PARKING, ACCESS_LINK, TRANSIT_STOP, TRANSIT_ROUTE, 
		PLAN, TRIP, PERFORMANCE, TURN_DELAY, 
		NEW_TRIP, NEW_PLAN, NEW_SELECTION, NEW_PERFORMANCE, NEW_TURN_DELAY, END_FILE
>>>>>>> .merge-right.r1529
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ TARGET_DIRECTORY, "TARGET_DIRECTORY", LEVEL0, OPT_KEY, PATH_KEY, "", "", NO_HELP },
		{ TARGET_NODE_FILE, "TARGET_NODE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_SHAPE_FILE, "TARGET_SHAPE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_LINK_FILE, "TARGET_LINK_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_LOCATION_FILE, "TARGET_LOCATION_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_CONNECTION_FILE, "TARGET_CONNECTION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_PARKING_FILE, "TARGET_PARKING_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_ACCESS_FILE, "TARGET_ACCESS_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_STOP_FILE, "TARGET_STOP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_ROUTE_FILE, "TARGET_ROUTE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MAXIMUM_XY_DIFFERENCE, "MAXIMUM_XY_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "100 feet", "0..5000 feet", NO_HELP },
		{ DELETE_PROBLEM_PLANS, "DELETE_PROBLEM_PLANS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_LOCATION_PROBLEM_FILE, "NEW_LOCATION_PROBLEM_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PARKING_PROBLEM_FILE, "NEW_PARKING_PROBLEM_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"LOCATION_MAP_REPORT",
		"PARKING_MAP_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);
	Location_XY_Flag (true);
#ifdef THREADS
	Enable_Threads (true);
#endif

	num_problems = num_perf = num_turn = 0;
	select_flag = new_select_flag = delete_flag = loc_problem_flag = park_problem_flag = false;
	shape_flag = parking_flag = access_flag = stop_flag = line_flag = target_flag = false;
	
	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
<<<<<<< .working

	System_Read_False (PERFORMANCE);
	System_Data_Reserve (PERFORMANCE, 0);

	System_Read_False (TURN_DELAY);
	System_Data_Reserve (TURN_DELAY, 0);
=======

	System_Read_False (PERFORMANCE);
	System_Read_False (TURN_DELAY);
>>>>>>> .merge-right.r1529
}

//---------------------------------------------------------
//	Relocate destructor
//---------------------------------------------------------

Relocate::~Relocate (void)
{
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	Relocate *program = 0;
	try {
		program = new Relocate ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
