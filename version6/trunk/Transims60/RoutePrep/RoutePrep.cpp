//*********************************************************
//	RoutePrep.cpp - Transit Route Preparation Utility
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	RoutePrep constructor
//---------------------------------------------------------

RoutePrep::RoutePrep (void) : Data_Service ()
{
	Program ("RoutePrep");
	Version (10);
	Title ("Transit Route Preparation Utility");

	System_File_Type optional_files [] = {
		LINK, NODE, SHAPE, ROUTE_NODES, NEW_NODE, NEW_LINK, NEW_SHAPE, NEW_ROUTE_NODES, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};

	Control_Key routeprep_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ FIRST_NODE_NUMBER, "FIRST_NODE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..1000000000", NO_HELP },
		{ FIRST_LINK_NUMBER, "FIRST_LINK_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..1000000000", NO_HELP },
		{ FIRST_ROUTE_NUMBER, "FIRST_ROUTE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "1", "1..100000", NO_HELP },
		{ ROUTE_MODE_MAP, "ROUTE_MODE_MAP", LEVEL1, OPT_KEY, LIST_KEY, "BUS", "BUS, EXPRESS, TROLLEY, STREETCAR, LIGHTRAIL, RAPIDRAIL, REGIONRAIL", NO_HELP },
		{ MODE_VEH_TYPE_MAP, "MODE_VEH_TYPE_MAP", LEVEL1, OPT_KEY, LIST_KEY, "1", "1..100", NO_HELP },

		{ CONVERT_NODE_NUMBERS, "CONVERT_NODE_NUMBERS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },		
		{ INPUT_NODE_FILE, "INPUT_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_NODE_FORMAT, "INPUT_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NODE_MAP_FILE, "NODE_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_NODE_MAP_FILE, "NEW_NODE_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },

		{ TRANSIT_TIME_PERIODS, "TRANSIT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ TRANSIT_PERIOD_OFFSETS, "TRANSIT_PERIOD_OFFSETS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PERIOD_TRAVEL_TIMES, "PERIOD_TRAVEL_TIMES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ TRANSIT_NODE_TYPES, "TRANSIT_NODE_TYPES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ COLLAPSE_ROUTE_DATA, "COLLAPSE_ROUTE_DATA", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ COORDINATE_RESOLUTION, "COORDINATE_RESOLUTION", LEVEL0, OPT_KEY, FLOAT_KEY, "3.0 feet", "0..100 feet", NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },

		{ ROUTE_SHAPE_FILE, "ROUTE_SHAPE_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ROUTE_NAME_FIELD, "ROUTE_NAME_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ROUTE_LINK_FIELD, "ROUTE_LINK_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ROUTE_ORDER_FIELD, "ROUTE_ORDER_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ROUTE_HEADWAY_FIELD, "ROUTE_HEADWAY_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ROUTE_FREQUENCY_FIELD, "ROUTE_FREQUENCY_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ROUTE_LINK_LENGTH, "ROUTE_LINK_LENGTH", LEVEL0, OPT_KEY, FLOAT_KEY, "1000.0 feet", "300..3000 feet", NO_HELP }, 
		
		{ ROUTE_STOP_FILE, "ROUTE_STOP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ROUTE_STOP_FORMAT, "ROUTE_STOP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ ROUTE_STOP_ROUTE_FIELD, "ROUTE_STOP_ROUTE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ROUTE", "", NO_HELP },
		{ ROUTE_STOP_STOP_FIELD, "ROUTE_STOP_STOP_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "STOP", "", NO_HELP },
		{ ROUTE_STOP_OFFSET_FIELD, "ROUTE_STOP_OFFSET_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ROUTE_STOP_ORDER_FIELD, "ROUTE_STOP_ORDER_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },

		{ STATION_NODE_FILE, "STATION_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ STATION_NODE_FORMAT, "STATION_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ STATION_NODE_FIELD, "STATION_NODE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ STATION_NAME_FIELD, "STATION_NAME_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ STATION_X_FIELD, "STATION_X_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ STATION_Y_FIELD, "STATION_Y_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ STATION_LINK_LENGTH, "STATION_LINK_LENGTH", LEVEL0, OPT_KEY, FLOAT_KEY, "300.0 feet", "0..1000 feet", NO_HELP },
		{ STATION_PLATFORM_TIME, "STATION_PLATFORM_TIME", LEVEL0, OPT_KEY, TIME_KEY, "0 seconds", "0..1000 seconds", NO_HELP },
		{ STATION_PLATFORM_FLAG, "STATION_PLATFORM_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ STATION_PLATFORM_OFFSET, "STATION_PLATFORM_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "15.0 feet", "0..160 feet", NO_HELP },

		{ INPUT_ROUTE_FILE, "INPUT_ROUTE_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_ROUTE_FORMAT, "INPUT_ROUTE_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TPPLUS", FORMAT_RANGE, NO_HELP },
		{ ROUTE_PERIOD_MAP, "ROUTE_PERIOD_MAP", LEVEL1, OPT_KEY, LIST_KEY, "1", "0..24", NO_HELP },
		{ ROUTE_VEHICLE_TYPE, "ROUTE_VEHICLE_TYPE", LEVEL1, OPT_KEY, INT_KEY, "0", ">=0", NO_HELP },
		{ NEW_ROUTE_FILE, "NEW_ROUTE_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ROUTE_FORMAT, "NEW_ROUTE_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TPPLUS", FORMAT_RANGE, NO_HELP },

		{ ROUTE_SPEED_FILE, "ROUTE_SPEED_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ROUTE_SPEED_FORMAT, "ROUTE_SPEED_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ SPEED_ROUTE_FIELD, "SPEED_ROUTE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SPEED_PEAK_FIELD, "SPEED_PEAK_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SPEED_OFFPEAK_FIELD, "SPEED_OFFPEAK_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },

		{ SEGMENT_SPEED_FACTOR_FILE, "SEGMENT_SPEED_FACTOR_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SEGMENT_SPEED_FACTOR_FORMAT, "SEGMENT_SPEED_FACTOR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ SEGMENT_FIELD, "SEGMENT_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SPEED_FACTOR_FIELD, "SPEED_FACTOR_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ IGNORE_EXISTING_SPEEDS, "IGNORE_EXISTING_SPEEDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		"ROUTE_SEGMENT_NODES",
		""
	};
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (routeprep_keys);
	Report_List (reports);

	proj_service.Add_Keys ();
	
	AB_Map_Flag (true);

	route_name_field = route_link_field = route_order_field = -1;
	rstop_route_field = rstop_stop_field = rstop_offset_field = rstop_order_field = -1; 
	station_node_field = station_name_field = -1;
	speed_route_field = speed_peak_field = speed_offpeak_field = segment_field = speed_fac_field = -1;
	convert_flag = shape_flag = offset_flag = time_flag = collapse_routes = in_route_flag = out_route_flag = segment_report = seg_fac_flag = false;
	route_freq_flag = route_shape_flag = route_stop_flag = station_flag = platform_flag = new_route_flag = route_speed_flag = false;
	convert_node_flag = input_node_flag = node_map_flag = new_map_flag = ignore_speeds = false;
	route_shape_file = 0;
	new_route_nodes = 0;

	new_link = new_node = new_route = 1;
	num_periods = num_match = 0;
	station_length = 100.0;
	link_length = 300.0;
	platform_time = 20.0;
	platform_speed = 15.0;
	platform_offset = 5.0;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	RoutePrep *program = 0;
	try {
		program = new RoutePrep ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
