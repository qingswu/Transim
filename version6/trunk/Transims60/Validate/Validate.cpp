//*********************************************************
//	Validate.cpp - Network Validation Utility
//*********************************************************

#include "Validate.hpp"

int Validate::volume_level [] = {
	1000, 2500, 5000, 7500, 10000, 25000, 50000, 75000, 100000, 500000, 0
};

//---------------------------------------------------------
//	Validate constructor
//---------------------------------------------------------

Validate::Validate (void) : Data_Service ()
{
	Program ("Validate");
	Version (13);
	Title ("Compare Volumes to Counts");

	System_File_Type required_files [] = {
		NODE, LINK, END_FILE
	};
	System_File_Type optional_files [] = {
		ZONE, SHAPE, POCKET, CONNECTION, ACCESS_LINK, LOCATION, PARKING, LANE_USE, 
		SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, VEHICLE_TYPE,
		TURN_PENALTY, PERFORMANCE, TURN_DELAY, RIDERSHIP, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, LINK_EQUIVALENCE_FILE, ZONE_EQUIVALENCE_FILE, 
		STOP_EQUIVALENCE_FILE, LINE_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, LINK_BEARING_WARNINGS, 0
	};
	Control_Key validate_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ INPUT_VOLUME_FILE, "INPUT_VOLUME_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRAFFIC_COUNT_FILE, "TRAFFIC_COUNT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TURN_VOLUME_FILE, "TURN_VOLUME_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TURN_COUNT_FILE, "TURN_COUNT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINE_GROUP_COUNT_FILE, "LINK_GROUP_COUNT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP},
		{ STOP_GROUP_COUNT_FILE, "STOP_GROUP_COUNT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_VOLUME_FILE, "NEW_VOLUME_FILE", LEVEL0, OPT_KEY,  OUT_KEY, "", FILE_RANGE, NO_HELP},
		{ NEW_VOLUME_COUNT_FILE, "NEW_VOLUME_COUNT_FILE", LEVEL0, OPT_KEY,  OUT_KEY, "", FILE_RANGE, NO_HELP},
		{ ANALYSIS_METHOD, "ANALYSIS_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "VOLUME", "VOLUME, VMT", NO_HELP },
		{ ADJUSTMENT_FACTOR, "ADJUSTMENT_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.05..10.0", NO_HELP },
		{ FACILITY_TYPE_LABELS, "FACILITY_TYPE_LABELS", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP},
		{ AREA_TYPE_LABELS, "AREA_TYPE_LABELS", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ CHECK_NETWORK_FLAG, "CHECK_NETWORK_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_PROBLEM_NODE_FILE, "NEW_PROBLEM_NODE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PROBLEM_LINK_FILE, "NEW_PROBLEM_LINK_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_PROBLEM_COORDINATE_FILE, "NEW_PROBLEM_COORDINATE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"VOLUME_LEVEL_SUMMARY",
		"FACILITY_TYPE_SUMMARY",
		"AREA_TYPE_SUMMARY",
		"ZONE_GROUP_SUMMARY",
		"ZONE_AT_FT_SUMMARY",
		"LINK_GROUP_SUMMARY",
		"LINK_GROUP_DETAILS",
		"TURN_MOVEMENT_SUMMARY",
		"TURN_LEVEL_OF_SERVICE",
		"ZONE_EQUIVALENCE",
		"LINK_EQUIVALENCE",
		"LINE_GROUP_SUMMARY",
		"STOP_GROUP_SUMMARY",
		"BOARDING_GROUP_SUMMARY",
		"ALIGHTING_GROUP_SUMMARY",
		"LINE_EQUIVALENCE",
		"STOP_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	
	Sum_Flow_Flag (true);
	Key_List (validate_keys);
	Report_List (reports);

	Page_Size (120, 47);

	problem_node_flag = problem_link_flag = problem_coord_flag = false;
	delay_flag = link_flag = turn_flag = output_flag = zone_flag = method = line_flag = stop_flag = vc_flag = check_net_flag = false;
	factor = 1.0;
	hours = 1.0;

	compass.Set_Points (8);
	
	System_Read_False (PERFORMANCE);

	stats_format = "\n%25.25s %7d%10.0lf%10.0lf%9.0lf %7.1lf %7.0lf %6.1lf %7.0lf %6.1lf %6.3lf%6.2lf%6.2lf";
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	Validate *program = 0;
	try {
		program = new Validate ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
