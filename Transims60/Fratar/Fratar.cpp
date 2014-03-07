//*********************************************************
//	Fratar.cpp - Factor a tables based on new marginals
//*********************************************************

#include "Fratar.hpp"

//---------------------------------------------------------
//	Fratar constructor
//---------------------------------------------------------

Fratar::Fratar (void) : Execution_Service ()
{
	Program ("Fratar");
	Version (0);
	Title ("File Format Conversion");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ TRIP_TABLE_FILE, "TRIP_TABLE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_TABLE_FORMAT, "TRIP_TABLE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ NEW_TRIP_TABLE_FILE, "NEW_TRIP_TABLE_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_TABLE_FORMAT, "NEW_TRIP_TABLE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ BASE_MARGIN_FILE, "BASE_MARGIN_FILE", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ BASE_MARGIN_FORMAT, "BASE_MARGIN_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TRIP_MARGIN_FILE, "TRIP_MARGIN_FILE", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_MARGIN_FORMAT, "TRIP_MARGIN_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MARGIN_TYPE_FIELD, "MARGIN_TYPE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "TYPE", "", NO_HELP },
		{ MARGIN_TRIP_FIELD, "MARGIN_TRIP_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "TOTAL", "", NO_HELP },
		{ MARGIN_TYPE_MAP_FILE, "MARGIN_TYPE_MAP_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MARGIN_TYPE_MAP_FORMAT, "MARGIN_TYPE_MAP_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ MAXIMUM_ITERATIONS, "MAXIMUM_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "10", "1..100", NO_HELP },
		{ PERCENT_TRIP_DIFFERENCE, "PERCENT_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 percent", "0.0..100.0 percent", NO_HELP },
		{ MINIMUM_TRIP_DIFFERENCE, "MINIMUM_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "0", "0..500", NO_HELP },
		{ MAXIMUM_TRIP_DIFFERENCE, "MAXIMUM_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "1000", "0..10000", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		""
	};
	Key_List (keys);
	Report_List (reports);

	in_flag = script_flag = period_flag = false;
	max_iterations = 10;
	min_diff = 0.0;
	max_diff = 1000.0;
	percent_diff = 10.0;

	int ignore_keys [] = {
		TIME_OF_DAY_FORMAT, MODEL_START_TIME, MODEL_END_TIME, MODEL_TIME_INCREMENT,  
		MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, NUMBER_OF_THREADS, 0
	};
	Ignore_Keys (ignore_keys);
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	Fratar *program = 0;
	try {
		program = new Fratar ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
