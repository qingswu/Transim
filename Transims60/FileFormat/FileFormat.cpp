//*********************************************************
//	FileFormat.cpp - convert files to a new format
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	FileFormat constructor
//---------------------------------------------------------

FileFormat::FileFormat (void) : Execution_Service ()
{
	Program ("FileFormat");
	Version (5);
	Title ("File Format Conversion");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DATA_FILE, "DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FORMAT, "DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ DATA_SELECT_FIELD, "DATA_SELECT_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "NAME = VALUE", NO_HELP },
		{ NEW_DATA_FILE, "NEW_DATA_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DATA_FORMAT, "NEW_DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ COPY_EXISTING_FIELDS, "COPY_EXISTING_FIELDS", LEVEL1, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ NEW_FILE_HEADER, "NEW_FILE_HEADER", LEVEL1, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ NEW_DATA_FIELD, "NEW_DATA_FIELD", LEVEL2, OPT_KEY, TEXT_KEY, "", "NAME, INTEGER, 10", NO_HELP },
		{ SORT_BY_FIELDS, "SORT_BY_FIELDS", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DATA_FIELD_MAP, "DATA_FIELD_MAP", LEVEL2, OPT_KEY, TEXT_KEY, "", "DATA_FIELD = COMBINE_FIELD", NO_HELP },	
		{ DATA_INDEX_FIELD, "DATA_INDEX_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },

		{ NEW_COMBINE_FIELDS_FILE, "NEW_COMBINE_FIELDS_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_COMBINE_FIELDS_FORMAT, "NEW_COMBINE_FIELDS_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },

		{ MATRIX_FILE, "MATRIX_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MATRIX_FORMAT, "MATRIX_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ NEW_MATRIX_FILE, "NEW_MATRIX_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_MATRIX_FORMAT, "NEW_MATRIX_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ SELECT_TABLES, "SELECT_TABLES", LEVEL1, OPT_KEY, TEXT_KEY, "ALL", "", NO_HELP },

		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		"FIELD_STATISTICS", 
		""
	};
	Key_List (keys);
	Report_List (reports);

	script_flag = data_flag = matrix_flag = stats_flag = combine_flag = index_flag = false;
	num_records = 0;

	int ignore_keys [] = {
		MODEL_START_TIME, MODEL_END_TIME, MODEL_TIME_INCREMENT,  
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
	FileFormat *program = 0;
	try {
		program = new FileFormat ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
