//*********************************************************
//	Gravity.cpp - Gravity-based distribution model
//*********************************************************

#include "Gravity.hpp"

//---------------------------------------------------------
//	Fratar constructor
//---------------------------------------------------------

Gravity::Gravity (void) : Data_Service ()
{
	Program ("Gravity");
	Version (4);
	Title ("File Format Conversion");
	
	System_File_Type required_files [] = {
		ZONE, END_FILE
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ ZONE_PRODUCTION_FIELD, "ZONE_PRODUCTION_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "PRODUCTION", "", NO_HELP },
		{ ZONE_ATTRACTION_FIELD, "ZONE_ATTRACTION_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ATTRACTION", "", NO_HELP },

		{ SKIM_FILE, "SKIM_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SKIM_FORMAT, "SKIM_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "CUBE", MATRIX_RANGE, FORMAT_HELP },
		{ SKIM_TABLE_NAME, "SKIM_TABLE_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "TIME", "", NO_HELP },
	
		{ TRIP_TABLE_FILE, "TRIP_TABLE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_TABLE_FORMAT, "TRIP_TABLE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "CUBE", MATRIX_RANGE, FORMAT_HELP },
		{ TRIP_TABLE_NAME, "TRIP_TABLE_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "TRIPS", "", NO_HELP },
		
		{ NEW_TRIP_TABLE_FILE, "NEW_TRIP_TABLE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_TABLE_FORMAT, "NEW_TRIP_TABLE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "CUBE", MATRIX_RANGE, FORMAT_HELP },

		{ FUNCTION_INPUT_UNITS, "FUNCTION_INPUT_UNITS", LEVEL0, OPT_KEY, TEXT_KEY, "NO_UNITS", "MINUTES, MILES, IMPEDANCE", NO_HELP },
		{ FUNCTION_INPUT_FACTOR, "FUNCTION_INPUT_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.001..1000.0", NO_HELP },
		{ FUNCTION_PARAMETERS, "FUNCTION_PARAMETERS", LEVEL1, OPT_KEY, TEXT_KEY, "", FUNCTION_RANGE, NO_HELP },

		{ CALIBRATE_FUNCTIONS, "CALIBRATE_FUNCTIONS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PARAMETER_CONSTRAINTS, "PARAMETER_CONSTRAINTS", LEVEL2, OPT_KEY, TEXT_KEY, "0", "", NO_HELP },

		{ BALANCING_METHOD, "BALANCING_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "AVERAGE", "PRODUCTION, ATTRACTION, AVERAGE", NO_HELP },
		{ MAXIMUM_ITERATIONS, "MAXIMUM_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "10", "1..100", NO_HELP },
		{ MAXIMUM_PERCENT_DIFFERENCE, "MAXIMUM_PERCENT_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 percent", "0.0..100.0 percent", NO_HELP },
		{ MAXIMUM_TRIP_DIFFERENCE, "MAXIMUM_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "1000", "0..10000", NO_HELP },

		{ NEW_INPUT_LENGTH_FILE, "NEW_INPUT_LENGTH_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_INPUT_LENGTH_FORMAT, "NEW_INPUT_LENGTH_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_OUTPUT_LENGTH_FILE, "NEW_OUTPUT_LENGTH_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_OUTPUT_LENGTH_FORMAT, "NEW_OUTPUT_LENGTH_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TRIP_LENGTH_INCREMENT, "TRIP_LENGTH_INCREMENT", LEVEL0, OPT_KEY, FLOAT_KEY, "100", ">0", NO_HELP },
		{ MAXIMUM_TRIP_LENGTH, "MAXIMUM_TRIP_LENGTH", LEVEL0, OPT_KEY, FLOAT_KEY, "20000", ">0", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		""
	};
	Required_System_Files (required_files);
	Key_List (keys);
	Report_List (reports);

	prod_field = attr_field = skim_field = -1;
	in_flag = script_flag = period_flag = trip_flag = in_len_flag = out_len_flag = new_flag = calib_flag = false;
	max_iterations = 10;
	max_diff = 1000.0;
	max_percent = 0.1;
	best_function = 1;
	total_in = 0.0;

	input_units = NO_UNITS;
	value_factor = 1.0;
	units_factor = 1.0;

	increment = 100.0;
	max_value = 20000.0;
		
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
	Gravity *program = 0;
	try {
		program = new Gravity ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
