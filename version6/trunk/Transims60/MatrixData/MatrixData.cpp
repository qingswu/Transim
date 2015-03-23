//*********************************************************
//	MatrixData.cpp - convert files to a new format
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	MatrixData constructor
//---------------------------------------------------------

MatrixData::MatrixData (void) : Data_Service (), Select_Service ()
{
	Program ("MatrixData");
	Version (5);
	Title ("Matrix Data Processing");

	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, MERGE_TIME_PERIODS, ZONE_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, HIGHEST_ZONE_NUMBER, 0
	};
	int select_service_keys [] = {
		SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, SELECTION_PERCENTAGE, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DATA_FILE, "DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FORMAT, "DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ ORIGIN_FIELD, "ORIGIN_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "ORG", "NAME", NO_HELP },
		{ DESTINATION_FIELD, "DESTINATION_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "DES", "NAME", NO_HELP },
		{ PERIOD_FIELD, "PERIOD_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "PERIOD", "NAME", NO_HELP },
		{ LOW_TIME_FIELD, "LOW_TIME_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "LOW", "NAME", NO_HELP },
		{ HIGH_TIME_FIELD, "HIGH_TIME_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "HIGH", "NAME", NO_HELP },
		{ DATA_FACTOR_FIELD, "DATA_FACTOR_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "DATE", "NAME", NO_HELP },
		{ TABLE_DATA_FIELD, "TABLE_DATA_FIELD", LEVEL2, OPT_KEY, TEXT_KEY, "TRIPS", "NAME", NO_HELP },

		{ MATRIX_FILE, "MATRIX_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MATRIX_FORMAT, "MATRIX_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ GROWTH_FACTOR, "GROWTH_FACTOR", LEVEL1, OPT_KEY, FLOAT_KEY, "1.0", "0.01..100.0", NO_HELP },
		{ TRANSPOSE_OD, "TRANSPOSE_OD", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SELECT_TABLES, "SELECT_TABLES", LEVEL1, OPT_KEY, TEXT_KEY, "ALL", "", NO_HELP },
		{ MATRIX_PERIOD_BREAKS, "MATRIX_PERIOD_BREAKS", LEVEL1, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ MATRIX_PERIOD_FIELDS, "MATRIX_PERIOD_FIELDS", LEVEL1, OPT_KEY, TEXT_KEY, "TRIPS", "NAME", NO_HELP },

		{ SQUARE_TABLE_FILE, "SQUARE_TABLE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },

		{ GROWTH_FACTOR_FILE, "GROWTH_FACTOR_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ GROWTH_FACTOR_FORMAT, "GROWTH_FACTOR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ FACTOR_TABLES, "FACTOR_TABLES", LEVEL0, OPT_KEY, TEXT_KEY, "ALL", "", NO_HELP },
		{ FACTOR_PERIOD_BREAKS, "FACTOR_PERIOD_BREAKS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ FACTOR_PERIOD_FIELDS, "FACTOR_PERIOD_FIELDS", LEVEL0, OPT_KEY, TEXT_KEY, "FACTOR", "NAME", NO_HELP },

		{ NEW_MATRIX_FILE, "NEW_MATRIX_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_MATRIX_FORMAT, "NEW_MATRIX_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ NEW_MATRIX_TABLE, "NEW_MATRIX_TABLE", LEVEL1, OPT_KEY, TEXT_KEY, "", "NAME, INTEGER, 10", NO_HELP },
		{ NEW_MATRIX_FACTOR, "NEW_MATRIX_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "> 0.1", NO_HELP },
		{ NEW_MATRIX_METHOD, "NEW_MATRIX_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "ADD", "ADD, RATIO, EXPAND, SELECT", NO_HELP },
		{ PERCENT_MISSING_DATA, "PERCENT_MISSING_DATA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..100.0", NO_HELP },
		{ PROBLEM_DISTRIBUTION, "PROBLEM_DISTRIBUTION", LEVEL0, OPT_KEY, LIST_KEY, "100.0", "0.0..100.0", NO_HELP },
		{ MINIMUM_RATIO_VALUE, "MINIMUM_RATIO_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.1", "0.001..1.0", NO_HELP },
		{ MAXIMUM_RATIO_VALUE, "MAXIMUM_RATIO_VALUE", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0", "1.0..1000.0", NO_HELP },
		{ ZONE_LABEL_FILE, "ZONE_LABEL_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ CONVERSION_EQUIV_FILE, "CONVERSION_EQUIV_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_MARGIN_TOTAL_FILE, "NEW_MARGIN_TOTAL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SQUARE_TABLE_FILE, "NEW_SQUARE_TABLE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_START_TIME_FILE, "NEW_START_TIME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_END_TIME_FILE, "NEW_END_TIME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"ZONE_EQUIVALENCE",
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		"CONVERSION_EQUIV", 
		"FIELD_STATISTICS", 
		"MARGIN_TOTALS",
		""
	};
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Report_List (reports);

	script_flag = stats_flag = data_flag = matrix_flag = label_flag = margin_flag = square_flag = factor_flag = equiv_flag = false;
	new_flag = input_flag = missing_flag = problem_flag = user_equiv_flag = fac_table_flag = false;
	num_records = group = 0;
	method = ADD_METHOD;
	new_matrix = 0;
	factor = 1.0;

	int ignore_keys [] = {
		MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, NUMBER_OF_THREADS, 0
	};
	Ignore_Keys (ignore_keys);
}

//---------------------------------------------------------
//	MatrixData destructor
//---------------------------------------------------------

MatrixData::~MatrixData (void)
{
	if (new_matrix != 0) {
		delete new_matrix;
	}
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	MatrixData *program = 0;
	try {
		program = new MatrixData ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
