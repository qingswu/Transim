//*********************************************************
//	Simulator.cpp - Simulate Travel Plans
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	Simulator constructor
//---------------------------------------------------------

Simulator::Simulator (void) : Simulator_Service ()
{
	Program ("Simulator");
	Version (1);
	Title ("Simulate Travel Plans");

	System_File_Type required_files [] = {
		NODE, LINK, POCKET, CONNECTION, PARKING, LOCATION, 
		VEHICLE_TYPE, PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		ACCESS_LINK, LANE_USE, TURN_PENALTY, SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR, 
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		HOUSEHOLD, NEW_PROBLEM, END_FILE
	};
	Control_Key msim_keys [] = { //--- code, key, level, status, type, help ----
		END_CONTROL
	};
	const char *reports [] = {
		"HOUSEHOLD_TYPE_SCRIPT",
		"HOUSEHOLD_TYPE_STACK",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);

	//Router_Service_Keys ();
	Simulator_Service_Keys ();
	Key_List (msim_keys);
	Report_List (reports);

	io_flag = false;

	lane_change_levels = 10;
	max_vehicles = 0;
	time_step = max_time = 0;
	signal_update_time = timing_update_time = transit_update_time = 0;

	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
	Transit_Veh_Flag (true);
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	Simulator *program = 0;
	try {
		program = new Simulator ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif

