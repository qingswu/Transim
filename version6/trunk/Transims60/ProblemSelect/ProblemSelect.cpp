//*********************************************************
//	ProblemSelect.cpp - Select Households for Re-Routing
//*********************************************************

#include "ProblemSelect.hpp"

//---------------------------------------------------------
//	ProblemSelect constructor
//---------------------------------------------------------

ProblemSelect::ProblemSelect (void) : Data_Service (), Select_Service ()
{
	Program ("ProblemSelect");
	Version (0);
	Title ("Travel Problem Selection Utility");

	System_File_Type required_files [] = {
		NODE, LINK, LOCATION, PROBLEM, NEW_SELECTION, END_FILE
	};
	System_File_Type optional_files [] = {
		SELECTION, END_FILE
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_FACILITY_TYPES, 
		SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, SELECT_PROBLEM_TYPES, 
		SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		//{ SELECT_TIME_RATIOS, "SELECT_TIME_RATIOS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, >1.0", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		//"FIRST_REPORT",
		//"SECOND_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

	max_problem = num_problem = 0;
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	ProblemSelect *program = 0;
	try {
		program = new ProblemSelect ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
