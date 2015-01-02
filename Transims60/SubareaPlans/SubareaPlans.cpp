//*********************************************************
//	SubareaPlans.cpp - Create a Subarea Plan file
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	SubareaPlans constructor
//---------------------------------------------------------

SubareaPlans::SubareaPlans (void) : Data_Service (), Select_Service ()
{
	Program ("SubareaPlans");
	Version (3);
	Title ("Subarea Plan and Vehicle Files");

	System_File_Type required_files [] = {
		NODE, LINK, PARKING, LOCATION, PLAN, NEW_PLAN, 
		END_FILE
	};
	System_File_Type optional_files [] = {
		ACCESS_LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER,
		VEHICLE_TYPE, SELECTION, NEW_TRIP,
		END_FILE
	};
	int data_service_keys [] = {
		SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_LINKS, SELECT_NODES,
		SELECT_STOPS, SELECT_ROUTES, DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, ZONE_EQUIVALENCE_FILE, 0
	};
	Control_Key subareaplan_keys [] = { //--- code, key, level, status, type, default, range, help ----
		END_CONTROL
	};
	const char *reports [] = {
		"TRIP_SUMMARY_REPORT",
		"ZONE_EQUIVALENCE",
		"TIME_PERIOD_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);

	Key_List (subareaplan_keys);
	Report_List (reports);
	Enable_Partitions (true);

#ifdef THREADS
	Enable_Threads (true);
#endif

	select_flag = transit_flag = access_flag = trip_flag = false;
	num_trips = 0;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	SubareaPlans *program = 0;
	try {
		program = new SubareaPlans ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif