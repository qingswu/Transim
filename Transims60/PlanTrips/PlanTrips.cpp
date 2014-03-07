//*********************************************************
//	PlanTrips.cpp - update trip start and end times
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	PlanTrips constructor
//---------------------------------------------------------

PlanTrips::PlanTrips (void) : Data_Service (), Select_Service () 
{
	Program ("PlanTrips");
	Version (1);
	Title ("Update Trip Start and End Times");
	
	System_File_Type required_files [] = {
		PLAN, NEW_TRIP, END_FILE
	};
	System_File_Type optional_files [] = {
		TRIP, SELECTION, END_FILE
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Enable_Partitions (true);

	select_flag = false;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
}

//---------------------------------------------------------
//	PlanTrips destructor
//---------------------------------------------------------

PlanTrips::~PlanTrips (void)
{
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	PlanTrips *program = 0;
	try {
		program = new PlanTrips ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif