//*********************************************************
//	PlanSelect.cpp - travel plan selection utility
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	PlanSelect constructor
//---------------------------------------------------------

PlanSelect::PlanSelect (void) : Data_Service (), Select_Service ()
{
	Program ("PlanSelect");
	Version (2);
	Title ("Travel Plan Selection Utility");

	System_File_Type required_files [] = {
		PLAN, NEW_SELECTION, END_FILE
	};
	System_File_Type optional_files [] = {
		NODE, LINK, LANE_USE, CONNECTION, LOCATION, VEHICLE_TYPE, LINK_DELAY, NEW_PLAN, END_FILE
	};
	int file_service_keys [] = {
		SAVE_LANE_USE_FLOWS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_VEHICLE_TYPES, SELECT_TRAVELER_TYPES, 
		SELECT_FACILITY_TYPES, SELECT_PARKING_LOTS, SELECT_VC_RATIOS, SELECT_TIME_RATIOS, 
		SELECT_LINKS, SELECT_NODES, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, SELECT_SUBAREA_POLYGON, 
		PERCENT_TIME_DIFFERENCE, MINIMUM_TIME_DIFFERENCE, MAXIMUM_TIME_DIFFERENCE, 
		SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, DELETION_FILE, DELETION_FORMAT, 
		DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		END_CONTROL
	};
	const char *reports [] = {
		//"FIRST_REPORT",
		//"SECOND_REPORT",
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

	delay_flag = turn_flag = new_plan_flag = select_parts = false; 
	num_trips = num_select = 0;

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
	PlanSelect *program = 0;
	try {
		program = new PlanSelect ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif

