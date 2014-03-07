//*********************************************************
//	GridData.cpp - Zone Data Processing
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	GridData constructor
//---------------------------------------------------------

GridData::GridData (void) : Data_Service ()
{
	Program ("GridData");
	Version (1);
	Title ("Grid Data Processor");

	System_File_Type optional_files [] = {
		ZONE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key griddata_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ ARC_GRID_FILE, "ARC_GRID_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ GRID_DATA_FILE, "GRID_DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ GRID_DATA_FORMAT, "GRID_DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ NEW_ARC_GRID_FILE, "NEW_ARC_GRID_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },

		{ COPY_EXISTING_FIELDS, "COPY_EXISTING_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ GRID_ZONE_FIELDS, "GRID_ZONE_FIELDS", LEVEL0, OPT_KEY, INT_KEY, "1", "1..10", NO_HELP },
		{ NEW_GRID_FIELD, "NEW_GRID_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },

		{ ZONE_BOUNDARY_FILE, "ZONE_BOUNDARY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ BOUNDARY_ZONE_FIELD, "BOUNDARY_ZONE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "ZONE", "", NO_HELP },
		{ GRID_SIZE, "GRID_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "1320 feet", "264..31680 feet", NO_HELP },
		
		{ ZONE_DATA_FILE, "ZONE_DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_DATA_FORMAT, "ZONE_DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ ZONE_ID_FIELD, "ZONE_ID_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "ZONE", "", NO_HELP },
		
		{ BOUNDARY_POLYGON_FILE, "BOUNDARY_POLYGON_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },

		{ POINT_DATA_FILE, "POINT_DATA_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ POINT_DATA_FORMAT, "POINT_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ POINT_ID_FIELD, "POINT_ID_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ID", "", NO_HELP },
		{ POINT_X_FIELD, "POINT_X_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "X", "", NO_HELP },
		{ POINT_Y_FIELD, "POINT_Y_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "Y", "", NO_HELP },
		{ POINT_DATA_FIELD, "POINT_DATA_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ MAX_POINT_DISTANCE, "MAX_POINT_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "1320 feet", "264..31280 feet", NO_HELP },

		{ NEW_SUMMARY_FILE, "NEW_SUMMARY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SUMMARY_FORMAT, "NEW_SUMMARY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ SUMMARY_SELECT_FIELD, "SUMMARY_SELECT_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "ID", "", NO_HELP },
		{ SUMMARY_SELECT_VALUES, "SUMMARY_SELECT_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "ALL", "", NO_HELP },
		{ SUMMARY_DATA_FIELD, "SUMMARY_DATA_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SUMMARY_DATA_VALUE, "SUMMARY_DATA_VALUE", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SUMMARY_DATA_FILTER, "SUMMARY_DATA_FILTER", LEVEL1, OPT_KEY, TEXT_KEY, "ALL", RANGE_RANGE, NO_HELP },

		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		""
	};
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Key_List (griddata_keys);
	Report_List (reports);

	projection.Add_Keys ();

	num_zone_fields = 1;
	zone_field = -1;
	max_distance = 8000;
	data_offset = 4;
	script_flag = grid_flag = point_flag = zone_flag = polygon_flag = summary_flag = false;

	int ignore_keys [] = {
		TIME_OF_DAY_FORMAT, MODEL_START_TIME, MODEL_END_TIME, MODEL_TIME_INCREMENT,  
		RANDOM_NUMBER_SEED,	MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, NUMBER_OF_THREADS, 0
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
	GridData *program = 0;
	try {
		program = new GridData ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
