//*********************************************************
//	TripSum.cpp - trip summary utility
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	TripSum constructor
//---------------------------------------------------------

TripSum::TripSum (void) : Data_Service (), Select_Service ()
{
	Program ("TripSum");
	Version (4);
	Title ("Trip Summary Utility");

	System_File_Type required_files [] = {
		TRIP, END_FILE
	};
	System_File_Type optional_files [] = {
		SELECTION, NODE, LINK, SHAPE, LOCATION, ZONE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, ZONE_EQUIVALENCE_FILE, TIME_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
//char * TripSum::HOUSEHOLD_PERSON_COUNT		= "HOUSEHOLD_PERSON_COUNT";
//char * TripSum::SELECT_ORIGIN_POLYGON		= "SELECT_ORIGIN_POLYGON";
//char * TripSum::SELECT_DESTINATION_POLYGON	= "SELECT_DESTINATION_POLYGON";
//char * TripSum::SELECT_PROBABILITY_FILE		= "SELECT_PROBABILITY_FILE";
//char * TripSum::SELECT_PROBABILITY_FORMAT	= "SELECT_PROBABILITY_FORMAT";
		
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_VEHICLE_TYPES, 
		SELECT_SUBAREAS, SELECTION_POLYGON, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, SELECTION_PERCENTAGE, 
		0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_TIME_DISTRIBUTION_FILE, "NEW_TIME_DISTRIBUTION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_TIME_FILE, "NEW_TRIP_TIME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_LENGTH_FILE, "NEW_TRIP_LENGTH_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_TRIP_END_FILE, "NEW_LINK_TRIP_END_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_TRIP_END_FORMAT, "NEW_LINK_TRIP_END_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_LOCATION_TRIP_END_FILE, "NEW_LOCATION_TRIP_END_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LOCATION_TRIP_END_FORMAT, "NEW_LOCATION_TRIP_END_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_ZONE_TRIP_END_FILE, "NEW_ZONE_TRIP_END_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ZONE_TRIP_END_FORMAT, "NEW_ZONE_TRIP_END_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ NEW_TRIP_TABLE_FILE, "NEW_TRIP_TABLE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_TABLE_FORMAT, "NEW_TRIP_TABLE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ TRIP_TIME_INCREMENT, "TRIP_TIME_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "15 minutes", MINUTE_RANGE, NO_HELP },
		{ TRIP_LENGTH_INCREMENT, "TRIP_LENGTH_INCREMENT", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0 miles", "0.0, 0.1..10.0 miles", NO_HELP },
		{ DISTANCE_CALCULATION, "DISTANCE_CALCULATION", LEVEL0, OPT_KEY, TEXT_KEY, "STRAIGHT_LINE", DISTANCE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TOP_100_LINK_TRIP_ENDS",
		"TOP_100_LANE_TRIP_ENDS",
		"TOP_100_TRIP/CAPACITY_RATIOS",
		"ZONE_EQUIVALENCE",
		"TIME_EQUIVALENCE",
		"TIME_DISTRIBUTION", 
		"TRIP_TIME_REPORT",
		"TRIP_LENGTH_SUMMARY",
		"TRIP_PURPOSE_SUMMARY",
		"MODE_LENGTH_SUMMARY",
		"MODE_PURPOSE_SUMMARY",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

#ifdef THREADS
	Enable_Threads (true);
#endif
#ifdef MPI_EXE
	Enable_MPI (true);
#endif

	select_flag = time_flag = trip_len_flag = trip_purp_flag = mode_len_flag = mode_purp_flag = false;
	diurnal_flag = time_file_flag = len_file_flag = link_trip_flag = loc_trip_flag = zone_trip_flag = trip_table_flag = false;

	distance_units = MILES;

	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	TripSum *program = 0;
	try {
		program = new TripSum ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
