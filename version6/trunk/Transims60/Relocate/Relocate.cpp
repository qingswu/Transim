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
	Version (3);
	Title ("Move Trips and Plans to a New Network");
	
	System_File_Type required_files [] = {
		NODE, LINK, LOCATION, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, PARKING, ACCESS_LINK, TRANSIT_STOP, TRANSIT_ROUTE, 
		PLAN, TRIP, NEW_TRIP, NEW_PLAN, NEW_SELECTION, END_FILE
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
		{ TARGET_PARKING_FILE, "TARGET_PARKING_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_ACCESS_FILE, "TARGET_ACCESS_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_STOP_FILE, "TARGET_STOP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_ROUTE_FILE, "TARGET_ROUTE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MAXIMUM_XY_DIFFERENCE, "MAXIMUM_XY_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "100 feet", "0..1000 feet", NO_HELP },
		{ DELETE_PROBLEM_PLANS, "DELETE_PROBLEM_PLANS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Enable_Partitions (true);
	Location_XY_Flag (true);
#ifdef THREADS
	Enable_Threads (true);
#endif

	num_problems = 0;
	select_flag = new_select_flag = delete_flag = false;
	shape_flag = parking_flag = access_flag = stop_flag = line_flag = target_flag = false;
	
	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
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
