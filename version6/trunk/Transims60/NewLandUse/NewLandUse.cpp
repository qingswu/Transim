//*********************************************************
//	NewLandUse.cpp - redistribute land use data
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	NewLandUse constructor
//---------------------------------------------------------

NewLandUse::NewLandUse (void) : Data_Service ()
{
	Program ("NewLandUse");
	Version (4);
	Title ("Redistribute Land Use Data");
	
	System_File_Type required_files [] = {
		ZONE, NEW_ZONE, END_FILE
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ ZONE_DATA_FIELD, "ZONE_DATA_FIELD", LEVEL1, REQ_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ZONE_TARGET_FIELD, "ZONE_TARGET_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "TARGET", "", NO_HELP },
		{ ZONE_AREA_FIELD, "ZONE_AREA_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "AREA", "", NO_HELP },
		{ ZONE_GROUP_FIELD, "ZONE_GROUP_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "GROUP", "", NO_HELP },
		{ ZONE_TYPE_FIELD, "ZONE_TYPE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "TYPE", "", NO_HELP },
		{ ZONE_COVERAGE_FIELD, "ZONE_COVERAGE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "COVERAGE", "", NO_HELP },
		{ ZONE_YEAR_FILE, "ZONE_YEAR_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_YEAR_FORMAT, "ZONE_YEAR_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TARGET_DENSITY_FILE, "TARGET_DENSITY_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_DENSITY_FORMAT, "TARGET_DENSITY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ PROCESSING_STEP, "PROCESSING_STEP", LEVEL1, REQ_KEY, TEXT_KEY, "", "", NO_HELP },

		{ FUNCTION_PARAMETERS, "FUNCTION_PARAMETERS", LEVEL1, OPT_KEY, TEXT_KEY, "", FUNCTION_RANGE, NO_HELP },
		{ MAXIMUM_ITERATIONS, "MAXIMUM_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "10", "1..100", NO_HELP },
		{ MAXIMUM_PERCENT_DIFFERENCE, "MAXIMUM_PERCENT_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 percent", "0.0..100.0 percent", NO_HELP },
		{ MAXIMUM_TRIP_DIFFERENCE, "MAXIMUM_TRIP_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "1000", "0..10000", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"GROUP_SUMMARY_REPORT_*",
		""
	};
	Required_System_Files (required_files);
	Key_List (keys);
	Report_List (reports);

	year_number = num_years = num_cover = num_type = num_group = report_code = 0;
	area_field = target_field = index_field = -1;
	max_iterations = 10;
	max_diff = 1000.0;
	max_percent = 0.1;
	best_function = 1;
	total_in = 0.0;

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
	NewLandUse *program = 0;
	try {
		program = new NewLandUse ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
