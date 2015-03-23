//*********************************************************
//	Router.cpp - Network Path Building
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Router constructor
//---------------------------------------------------------

Router::Router (void) : Converge_Service ()
{
	Program ("Router");
	Version (85);
	Title ("Network Path Building");

	System_File_Type optional_files [] = {
		TRIP, PLAN, SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR, NEW_TURN_DELAY, NEW_RIDERSHIP, END_FILE
	};
	Control_Key keys [] = { //--- code, key, level, status, type, help ----
		{ UPDATE_PLAN_RECORDS, "UPDATE_PLAN_RECORDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ REROUTE_FROM_TIME_POINT, "REROUTE_FROM_TIME_POINT", LEVEL0, OPT_KEY, TIME_KEY, "0:00", "", NO_HELP },
		END_CONTROL
	};
	Optional_System_Files (optional_files);

	Converge_Service_Keys ();
	Key_List (keys);
	Report_List (reports);

	reroute_time = 0;
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	Router *program = 0;
	try {
		program = new Router ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif

