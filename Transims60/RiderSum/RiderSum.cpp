//*********************************************************
//	RiderSum.cpp - Ridership File Display Utility
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	RiderSum constructor
//---------------------------------------------------------

RiderSum::RiderSum (void) : Data_Service (), Select_Service ()
{
	Program ("RiderSum");
	Version (5);
	Title ("Ridership File Summary Utility");

	System_File_Type required_files [] = {
		NODE, LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, VEHICLE_TYPE, RIDERSHIP, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, STOP_EQUIVALENCE_FILE, LINE_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_MODES, SELECT_TIME_OF_DAY, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_LINKS, SELECT_ROUTES, SELECT_STOPS, 0
	};
	Control_Key ridersum_keys [] = {  //--- code, key, level, status, type, default, range, help ----
		{ NEW_LINE_RIDER_FILE, "NEW_LINE_RIDER_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINE_GROUP_FILE, "NEW_LINE_GROUP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_STOP_ROUTE_FILE, "NEW_STOP_ROUTE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_STOP_PROFILE , "NEW_STOP_PROFILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_STOP_DETAIL_FILE, "NEW_STOP_DETAIL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};

	const char *reports [] = {
		"STOP_EQUIVALENCE",
		"LINE_EQUIVALENCE",
		"LINE_RIDER_PROFILE",
		"LINE_GROUP_PROFILE",
		"LINE_SUMMARY_REPORT",
		"LINE_GROUP_REPORT",
		"STOP_SUMMARY_REPORT",
		"STOP_GROUP_REPORT",
		"STOP_RUN_DETAILS",
		"STOP_ROUTE_SUMMARY",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (ridersum_keys);
	Report_List (reports);

	rider_flag = on_off_flag = line_rider_flag = line_group_flag = stop_route_flag = stop_profile_flag = false;
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	RiderSum *program = 0;
	try {
		program = new RiderSum ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
