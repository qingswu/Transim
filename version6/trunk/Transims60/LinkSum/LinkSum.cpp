//*********************************************************
//	LinkSum.cpp - Summarize Link Performance Data
//*********************************************************

#include "LinkSum.hpp"

int	LinkSum::percent_break [NUM_PERCENTILES] = {50, 65, 70, 75, 80, 85, 90, 95, 99};

//---------------------------------------------------------
//	LinkSum constructor
//---------------------------------------------------------

LinkSum::LinkSum (void) : Data_Service (), Select_Service ()
{
	Program ("LinkSum");
<<<<<<< .working
	Version (17);
=======
	Version (24);
>>>>>>> .merge-right.r1529
	Title ("Summarize Link Performance Data");

	System_File_Type required_files [] = {
		NODE, LINK, PERFORMANCE, END_FILE
	};
	System_File_Type optional_files [] = {
		CONNECTION, LANE_USE, LOCATION, TURN_DELAY, NEW_PERFORMANCE, NEW_TURN_DELAY, END_FILE
	};
	int file_service_keys [] = {
		SAVE_LANE_USE_FLOWS, LINK_EQUIVALENCE_FILE, ZONE_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 
		CONGESTED_TIME_RATIO, MAXIMUM_TIME_RATIO, EXCLUDE_TIME_RATIO, 0
	};
	int select_service_keys [] = {
		SELECT_FACILITY_TYPES, SELECT_VC_RATIOS, SELECT_TIME_RATIOS, SELECT_SUBAREAS, SELECTION_POLYGON, 0
	};
	Control_Key linksum_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ COMPARE_PERFORMANCE_FILE, "COMPARE_PERFORMANCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ COMPARE_PERFORMANCE_FORMAT, "COMPARE_PERFORMANCE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ COMPARE_LINK_MAP_FILE, "COMPARE_LINK_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ COMPARE_LINK_MAP_FORMAT, "COMPARE_LINK_MAP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ MINIMUM_LINK_VOLUME, "MINIMUM_LINK_VOLUME", LEVEL0, OPT_KEY, FLOAT_KEY, "2.0", ">= 0", NO_HELP },
		{ PERSON_BASED_STATISTICS, "PERSON_BASED_STATISTICS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SELECT_BY_LINK_GROUP, "SELECT_BY_LINK_GROUP", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ COMPARE_TURN_DELAY_FILE, "COMPARE_TURN_DELAY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ COMPARE_TURN_DELAY_FORMAT, "COMPARE_TURN_DELAY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ TURN_NODE_RANGE, "TURN_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },	
		{ NEW_LINK_ACTIVITY_FILE, "NEW_LINK_ACTIVITY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_ACTIVITY_FORMAT, "NEW_LINK_ACTIVITY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ COPY_LOCATION_FIELDS, "COPY_LOCATION_FIELDS", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NEW_ZONE_TRAVEL_FILE, "NEW_ZONE_TRAVEL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ZONE_TRAVEL_FORMAT, "NEW_ZONE_TRAVEL_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_GROUP_TRAVEL_FILE, "NEW_GROUP_TRAVEL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_GROUP_TRAVEL_FORMAT, "NEW_GROUP_TRAVEL_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMTED", FORMAT_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_FILE, "NEW_LINK_DIRECTION_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_FORMAT, "NEW_LINK_DIRECTION_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_FIELD, "NEW_LINK_DIRECTION_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", PERF_FIELD_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_INDEX, "NEW_LINK_DIRECTION_INDEX", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_LINK_DIRECTION_FLIP, "NEW_LINK_DIRECTION_FLIP", LEVEL1, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FILE, "NEW_LINK_DATA_FILE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FORMAT, "NEW_LINK_DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FIELD, "NEW_LINK_DATA_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", PERF_FIELD_RANGE, NO_HELP },
		{ NEW_DATA_SUMMARY_FILE, "NEW_DATA_SUMMARY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DATA_SUMMARY_FORMAT, "NEW_DATA_SUMMARY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_DATA_SUMMARY_PERIODS, "NEW_DATA_SUMMARY_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, " ", TIME_RANGE, NO_HELP },
		{ NEW_DATA_SUMMARY_RATIOS, "NEW_DATA_SUMMARY_RATIOS", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, 1.0..5.0", NO_HELP },
		{ NEW_GROUP_SUMMARY_FILE, "NEW_GROUP_SUMMARY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_GROUP_SUMMARY_FORMAT, "NEW_GROUP_SUMMARY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TOP_100_LINK_VOLUME",
		"TOP_100_LANE_VOLUME",
		"TOP_100_PERIOD_VOLUME",
		"TOP_100_SPEED_REDUCTIONS",
		"TOP_100_TRAVEL_TIME_RATIOS",
		"TOP_100_VOLUME_CAPACITY_RATIOS",
		"TOP_100_TRAVEL_TIME_CHANGES",
		"TOP_100_VOLUME_CHANGES",
		"LINK_VOLUME_GREATER_THAN_*",
		"GROUP_VOLUME_GREATER_THAN_*",
		"LINK_EQUIVALENCE",
		"ZONE_EQUIVALENCE",
		"TRAVEL_TIME_DISTRIBUTION",
		"VOLUME_CAPACITY_RATIOS",
		"TRAVEL_TIME_CHANGES",
		"VOLUME_CHANGES",
		"LINK_GROUP_TRAVEL_TIME",
		"NETWORK_PERFORMANCE_SUMMARY",
		"NETWORK_PERFORMANCE_DETAILS",
		"GROUP_PERFORMANCE_SUMMARY",
		"GROUP_PERFORMANCE_DETAILS",
		"RELATIVE_GAP_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Sum_Flow_Flag (true);
	Key_List (linksum_keys);
	Report_List (reports);

	compare_flag = group_select = summary_flag = group_sum_flag = turn_flag = turn_compare_flag = false;
<<<<<<< .working
	select_flag = activity_flag = zone_flag = group_flag = person_flag = periods_flag = ratios_flag = false;
=======
	select_flag = activity_flag = zone_flag = group_flag = person_flag = periods_flag = ratios_flag = false;
	link_map_flag = compare_perf_flag = false;
>>>>>>> .merge-right.r1529

	nerror = 0;
	minimum_volume = 2.0;
	increment = 900;
	num_inc = 96;
	cap_factor = 0.25;
	congested_ratio = 3.0;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	LinkSum *program = 0;
	try {
		program = new LinkSum ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif

