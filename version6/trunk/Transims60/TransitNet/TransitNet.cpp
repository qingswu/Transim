//*********************************************************
//	TransitNet.cpp - Transit Conversion Utility
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	TransitNet constructor
//---------------------------------------------------------

TransitNet::TransitNet (void) : Data_Service ()
{
	Program ("TransitNet");
	Version (7);
	Title ("Transit Network Conversion Utility");
	
	System_File_Type required_files [] = {
		NODE, LINK, CONNECTION, VEHICLE_TYPE, ROUTE_NODES, NEW_TRANSIT_STOP, 
		NEW_TRANSIT_ROUTE, NEW_TRANSIT_SCHEDULE, NEW_TRANSIT_DRIVER, END_FILE
	};
	System_File_Type optional_files [] = {
		ZONE, SHAPE, LOCATION, PARKING, ACCESS_LINK, NEW_LINK, NEW_PARKING, NEW_ACCESS_LINK, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, ZONE_EQUIVALENCE_FILE, 0
	};
	Control_Key transimsnet_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ ROUTE_DATA_FILE, "ROUTE_DATA_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ROUTE_DATA_FORMAT, "ROUTE_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ ROUTE_JOIN_FIELD, "ROUTE_JOIN_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ PARK_AND_RIDE_FILE, "PARK_AND_RIDE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PNR_ACCESS_DISTANCE, "PNR_ACCESS_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "0 feet", "0..10000 feet", NO_HELP },
		{ STATION_ACCESS_DISTANCE, "STATION_ACCESS_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "0 feet", "0..10000 feet", NO_HELP },
		{ STOP_SPACING_BY_AREA_TYPE, "STOP_SPACING_BY_AREA_TYPE", LEVEL1, OPT_KEY, LIST_KEY, "600 feet", "0, 100..10000 feet", NO_HELP },
		{ STOP_FACILITY_TYPE_RANGE, "STOP_FACILITY_TYPE_RANGE", LEVEL0, OPT_KEY, TEXT_KEY, "PRINCIPAL..FRONTAGE", "FREEWAY..EXTERNAL", NO_HELP },
		{ TRANSIT_TIME_PERIODS, "TRANSIT_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP },
		{ TRANSIT_TRAVEL_TIME_FACTORS, "TRANSIT_TRAVEL_TIME_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..3.0", NO_HELP },
		{ MINIMUM_DWELL_TIME, "MINIMUM_DWELL_TIME", LEVEL0, OPT_KEY, TIME_KEY, "5 seconds", "0..300 seconds", NO_HELP },
		{ INTERSECTION_STOP_TYPE, "INTERSECTION_STOP_TYPE", LEVEL0, OPT_KEY, TEXT_KEY, "NEARSIDE", "NEARSIDE, FARSIDE, MIDBLOCK", NO_HELP },
		{ INTERSECTION_STOP_OFFSET, "INTERSECTION_STOP_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "30.0 feet", "0..300 feet", NO_HELP },
		{ COORDINATE_SCHEDULES, "COORDINATE_SCHEDULES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ IGNORE_PATH_ERRORS, "IGNORE_PATH_ERRORS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_ROUTE_CHANGE_FILE, "NEW_ROUTE_CHANGE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ROUTE_CHANGE_FORMAT, "NEW_ROUTE_CHANGE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"ZONE_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (transimsnet_keys);
	Report_List (reports);

	AB_Map_Flag (true);

	parkride_flag = equiv_flag = dwell_flag = time_flag = speed_flag = at_flag = access_flag = new_link_flag = false;
	route_data_flag = coordinate_flag = ignore_errors_flag = change_flag = false;

	memset (facility_flag, '\0', sizeof (facility_flag));

	naccess = nlocation = nparking = max_link = PNR_distance = station_distance = 0;
	line_edit = route_edit = schedule_edit = driver_edit = 0;
	max_parking = max_access = max_location = max_stop = nparkride = 0;
	nstop = nroute = nschedule = ndriver = end_warnings = parking_warnings = 0;
	min_dwell = 5;
	stop_type = NEARSIDE;

	route_data_field = route_join_field = -1;
	change_route_fld = change_mode_fld = change_name_fld = change_in_len_fld = change_out_len_fld = change_in_stops_fld = change_out_stops_fld = -1;

	left_turn = compass.Num_Points () * -70 / 360;
	bus_code = Use_Code ("BUS");
	rail_code = Use_Code ("RAIL");
	stop_offset = 10;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	TransitNet *program = 0;
	try {
		program = new TransitNet ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif