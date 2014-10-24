//*********************************************************
//	CountSum.cpp - Traffic Count Processing
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	CountSum constructor
//---------------------------------------------------------

CountSum::CountSum (void) : Data_Service ()
{
	Program ("CountSum");
	Version (8);
	Title ("Traffic Count Processing");

	System_File_Type required_files [] = {
		NODE, LINK, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, NEW_PERFORMANCE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ ARC_SIGNAL_FILE, "ARC_SIGNAL_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SIGNAL_ID_FIELD, "SIGNAL_ID_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SIGNAL_TYPE_FIELD, "SIGNAL_TYPE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SELECT_SIGNAL_TYPES, "SELECT_SIGNAL_TYPES", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SIGNAL_PRIMARY_STREET, "SIGNAL_PRIMARY_STREET", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SIGNAL_CROSS_STREET, "SIGNAL_CROSS_STREET", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },

		{ SIGNAL_ID_MAP_FILE, "SIGNAL_ID_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SIGNAL_ID_MAP_FORMAT, "SIGNAL_ID_MAP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ SIGNAL_ID_MAP_FIELD, "SIGNAL_ID_MAP_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ID", "", NO_HELP },
		{ SIGNAL_NODE_FIELD, "SIGNAL_NODE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "NODE", "", NO_HELP },

		{ SIGNAL_DETECTOR_FILE, "SIGNAL_DETECTOR_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SIGNAL_DETECTOR_FORMAT, "SIGNAL_DETECTOR_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "CSV_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ DEVICE_ID_FIELD, "DEVICE_ID_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DETECTOR_ID_FIELD, "DETECTOR_ID_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DETECTOR_TYPE_FIELD, "DETECTOR_TYPE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SELECT_DETECTOR_TYPES, "SELECT_DETECTOR_TYPES", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DETECTOR_NAME_FIELD, "DETECTOR_NAME_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },

		{ ARC_STATION_FILE, "ARC_STATION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ STATION_ID_FIELD, "STATION_ID_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ STATION_DIR_FIELD, "STATION_DIR_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ STATION_OFFSET_FIELD, "STATION_OFFSET_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ STATION_TYPE_FIELD, "STATION_TYPE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SELECT_STATION_TYPES, "SELECT_STATION_TYPES", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ SELECT_FACILITY_TYPES, "SELECT_FACILITY_TYPES", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },

		{ LINK_ID_MAP_FILE, "LINK_ID_MAP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_ID_MAP_FORMAT, "LINK_ID_MAP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ LINK_ID_FIELD, "LINK_ID_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ID", "", NO_HELP },
		{ LINK_DIR_FIELD, "LINK_DIR_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "LINK", "", NO_HELP },
		{ LINK_OFFSET_FIELD, "LINK_OFFSET_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "OFFSET", "", NO_HELP },

		{ MAX_SEARCH_DISTANCE, "MAX_SEARCH_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "3.0 feet", "0..1000 feet", NO_HELP },

		{ DATA_FILE, "DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FORMAT, "DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ DATA_ID_FIELD, "DATA_ID_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DATA_VOLUME_FIELD, "DATA_VOLUME_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DATA_SPEED_FIELD, "DATA_SPEED_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DATA_DAY_TIME_FIELD, "DATA_DAY_TIME_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DATA_DAY_FIELD, "DATA_DAY_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DATA_TIME_FIELD, "DATA_TIME_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DATA_TIME_FORMAT, "DATA_TIME_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "HOUR_CLOCK", TIME_FORMAT_RANGE, NO_HELP },
		{ SELECT_DAYS, "SELECT_DAYS", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ADJUSTMENT_FACTOR, "ADJUSTMENT_FACTOR", LEVEL1, OPT_KEY, FLOAT_KEY, "1.0", "0.5..2.0", NO_HELP },

		{ OUTPUT_DAY_DATA, "OUTPUT_DAY_DATA", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ OUTPUT_MIN_MAX_DATA, "OUTPUT_MIN_MAX_DATA", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FILE, "NEW_LINK_DATA_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_DATA_FORMAT, "NEW_LINK_DATA_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_SIGNAL_NODE_FILE, "NEW_SIGNAL_NODE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SIGNAL_NODE_FORMAT, "NEW_SIGNAL_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NEW_LINK_ID_MAP_FILE, "NEW_LINK_ID_MAP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_ID_MAP_FORMAT, "NEW_LINK_ID_MAP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);

	Key_List (keys);
	Report_List (reports);
	
	projection.Add_Keys ();

	script_flag = data_flag = link_data_flag = day_flag = link_map_flag = new_map_flag = min_max_flag = false;
	signal_detect_flag = signal_node_flag = signal_map_flag = arc_signal_flag = arc_station_flag = false;
	num_records = 0;
	max_dist = 10.0;
	time_format = HOUR_CLOCK;

	signal_field = type_field = primary_field = cross_field = -1;
	device_field = detector_field = det_type_field =  det_name_field = -1;
	station_id_field = station_dir_field = station_offset_field = station_type_field = -1;
	link_id_field = link_dir_field = link_offset_field = -1;
	data_id_field = volume_field = speed_field = day_time_field = day_field = time_field = -1;

	int ignore_keys [] = {
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
	CountSum *program = 0;
	try {
		program = new CountSum ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
