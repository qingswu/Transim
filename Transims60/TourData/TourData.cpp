//*********************************************************
//	TourData.cpp - Tour Data Processing
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	ZoneData constructor
//---------------------------------------------------------

TourData::TourData (void) : Data_Service ()
{
	Program ("TourData");
	Version (1);
	Title ("Tour Data Processor");

	System_File_Type required_files [] = {
		ZONE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key tourdata_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ ZONE_GROUP_FIELD, "ZONE_GROUP_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "GROUP", "", NO_HELP },
		{ TOUR_FILE, "TOUR_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TOUR_FORMAT, "TOUR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMTED", FORMAT_RANGE, FORMAT_HELP },
		{ COMPARE_TOUR_FILE, "COMPARE_TOUR_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ COMPARE_TOUR_FORMAT, "COMPARE_TOUR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMTED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_TOUR_FILE, "NEW_TOUR_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TOUR_FORMAT, "NEW_TOUR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ TOUR_PURPOSE_FIELD, "TOUR_PURPOSE_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "PURPOSE", "", NO_HELP },
		{ TOUR_TYPE_FIELD, "TOUR_TYPE_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "TYPE", "", NO_HELP },
		{ TOUR_INCOME_FIELD, "TOUR_INCOME_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "INCOME", "", NO_HELP },
		{ TOUR_ORIGIN_FIELD, "TOUR_ORIGIN_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "ORIGIN", "", NO_HELP },
		{ TOUR_DESTINATION_FIELD, "TOUR_DESTINATION_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "DESTINATION", "", NO_HELP },
		{ TOUR_POINT_FIELD, "TOUR_POINT_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "POINT", "", NO_HELP },

		{ TARGET_TOUR_MATRIX, "TARGET_TOUR_MATRIX", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TARGET_TOUR_FORMAT, "TARGET_TOUR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ BASE_TOUR_MATRIX, "BASE_TOUR_MATRIX", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ BASE_TOUR_FORMAT, "BASE_TOUR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },
		{ NEW_RESULT_MATRIX, "NEW_RESULT_MATRIX", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_RESULT_FORMAT, "NEW_RESULT_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", MATRIX_RANGE, FORMAT_HELP },

		{ TABLE_PURPOSE_RANGE, "TABLE_PURPOSE_RANGE", LEVEL1, REQ_KEY, TEXT_KEY, "", RANGE_RANGE, NO_HELP },
		{ TABLE_TYPE_RANGE, "TABLE_TYPE_RANGE", LEVEL1, OPT_KEY, TEXT_KEY, "All", RANGE_RANGE, NO_HELP },
		{ TABLE_INCOME_RANGE, "TABLE_INCOME_RANGE", LEVEL1, OPT_KEY, TEXT_KEY, "All", RANGE_RANGE, NO_HELP },
		{ TABLE_TARGET_FIELD, "TABLE_TARGET_FIELD", LEVEL1, REQ_KEY, TEXT_KEY, "TARGET", "", NO_HELP },
		{ TABLE_BASE_FIELD, "TABLE_BASE_FIELD", LEVEL1, REQ_KEY, TEXT_KEY, "BASE", "", NO_HELP },
		{ SPECIAL_ATTRACTIONS, "SPECIAL_ATTRACTIONS", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },

		{ MIN_ABSOLUTE_DIFFERENCE, "MIN_ABSOLUTE_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "1000.0", ">=0", NO_HELP },
		{ MIN_PERCENT_DIFFERENCE, "MIN_PERCENT_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0", ">=0", NO_HELP },
		{ DISTANCE_FACTOR, "DISTANCE_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1000.0", ">=1.0", NO_HELP },
		{ ATTRACTION_FACTOR, "ATTRACTION_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "100.0", ">=1.0", NO_HELP },

		{ NEW_ATTRACTION_FILE, "NEW_ATTRACTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ATTRACTION_FORMAT, "NEW_ATTRACTION_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMTED", FORMAT_RANGE, FORMAT_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		"ZONE_ATTRACTIONS",
		"ZONE_GROUP_STATISTICS",
		""
	};
	Required_System_Files (required_files);
	File_Service_Keys (file_service_keys);
	Key_List (tourdata_keys);
	Report_List (reports);

	result_flag = stats_flag = attr_flag = attr_file_flag = compare_flag = group_flag = income_flag = false;

	group_field = purpose_field = type_field = org_field = des_field = income_field = point_field = -1;
	max_zone = max_group = num_group = 0;

	min_difference = 1000.0;
	min_percent_diff = 0.1;
	distance_factor = 1000.0;
	attraction_factor = 100.0;
}

bool operator < (Stats_Index left, Stats_Index right)
{
	if (left.origin < right.origin) return (true);
	if (left.origin == right.origin) {
		if (left.destination < right.destination) return (true);
		if (left.destination == right.destination) {
			if (left.type < right.type) return (true);
		}
	}
	return (false); 
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	TourData *program = 0;
	try {
		program = new TourData ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
