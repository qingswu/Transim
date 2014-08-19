//*********************************************************
//	ArcPerf.cpp - ArcView Performance Files
//*********************************************************

#include "ArcPerf.hpp"

//---------------------------------------------------------
//	ArcDelay constructor
//---------------------------------------------------------

ArcPerf::ArcPerf (void) : Data_Service (), Select_Service (), Draw_Service ()
{
	Program ("ArcPerf");
	Version (7);
	Title ("ArcView Performance Files");

	System_File_Type required_files [] = {
		NODE, LINK, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, CONNECTION, POCKET, ACCESS_LINK,
		VEHICLE_TYPE, PERFORMANCE, TURN_DELAY, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, CONGESTED_TIME_RATIO, 0
	};
	int select_service_keys [] = {
		SELECT_FACILITY_TYPES, SELECT_LINKS, SELECT_SUBAREA_POLYGON, 0
	};
	int draw_service_keys [] = {
		DRAW_NETWORK_LANES, DRAW_VEHICLE_SHAPES, LANE_WIDTH, CENTER_ONEWAY_LINKS, LINK_DIRECTION_OFFSET, DRAW_AB_DIRECTION, 
		MAXIMUM_SHAPE_ANGLE, MINIMUM_SHAPE_LENGTH, DRAW_ONEWAY_ARROWS, ONEWAY_ARROW_LENGTH, ONEWAY_ARROW_SIDE_OFFSET, 
		BANDWIDTH_FIELD, BANDWIDTH_SCALING_FACTOR, MINIMUM_BANDWIDTH_VALUE, MINIMUM_BANDWIDTH_SIZE, MAXIMUM_BANDWIDTH_SIZE, 0
	};

	Control_Key arcperf_keys [] = {  //--- code, key, level, status, type, default, range, help ----
		{ LINK_DATA_FILE, "LINK_DATA_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_DATA_FORMAT, "LINK_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ LINK_DIRECTION_FILE, "LINK_DIRECTION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_DIRECTION_FORMAT, "LINK_DIRECTION_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ SYSTEM_EVENT_FILE, "SYSTEM_EVENT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SYSTEM_EVENT_FORMAT, "SYSTEM_EVENT_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_ARC_LINK_DATA_FILE, "NEW_ARC_LINK_DATA_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_DATA_BY_PERIOD, "LINK_DATA_BY_PERIOD", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_ARC_LINK_DIR_FILE, "NEW_ARC_LINK_DIR_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
//		{ LINK_DIR_BY_PERIOD, "LINK_DIR_BY_PERIOD", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_ARC_PERFORMANCE_FILE, "NEW_ARC_PERFORMANCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_INTERSECTION_FILE, "NEW_ARC_INTERSECTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TURN_DELAY_FILE, "NEW_ARC_TURN_DELAY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_TRAFFIC_IMAGE, "NEW_ARC_TRAFFIC_IMAGE", LEVEL1, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ TRAFFIC_IMAGE_ATTRIBUTE, "TRAFFIC_IMAGE_ATTRIBUTE", LEVEL1, OPT_KEY, TEXT_KEY, "VOLUME", "VOLUME, DENSITY, MAX_DENSITY, QUEUE, MAX_QUEUE, FAILURE", NO_HELP },
		{ VEHICLE_TYPE_DISTRIBUTION, "VEHICLE_TYPE_DISTRIBUTION", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ADD_LINK_DIRECTION_INDEX, "ADD_LINK_DIRECTION_INDEX", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ IGNORE_TIME_RANGE_FIELDS, "IGNORE_TIME_RANGE_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PERIOD_MAXIMUM_FIELD, "PERIOD_MAXIMUM_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", PERF_FIELD_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);
	Draw_Service_Keys (draw_service_keys);

	Key_List (arcperf_keys);
	Report_List (reports);

	Enable_Partitions (true);

	projection.Add_Keys ();

	AB_Map_Flag (true);
	Sum_Flow_Flag (true);

	vc_field = cong_time_field = cong_vmt_field = cong_vht_field = max_period_field = -1;
	center_flag = lanes_flag = turn_flag = subarea_flag = index_flag = image_flag = shape_flag = max_period_flag = false;
	perf_flag = link_dir_flag = link_data_flag = intersection_flag =  event_flag = link_period_flag = dir_period_flag = false;
	width_field = green_field = yellow_field = red_field = split_field = 0;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	ArcPerf *program = 0;
	try {
		program = new ArcPerf ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
