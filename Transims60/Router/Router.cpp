//*********************************************************
//	Router.cpp - Network Path Building
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Router constructor
//---------------------------------------------------------

Router::Router (void) : Router_Service ()
{
	Program ("Router");
<<<<<<< .working
	Version (60);
=======
	Version (74);
>>>>>>> .merge-right.r1529
	Title ("Network Path Building");

	System_File_Type optional_files [] = {
		TRIP, PLAN, SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, NEW_TURN_DELAY, NEW_RIDERSHIP, END_FILE
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_PRIORITIES, 
		SELECT_START_TIMES, SELECT_END_TIMES, SELECT_ORIGINS, SELECT_DESTINATIONS, 
		SELECT_TRAVELER_TYPES, SELECT_VEHICLE_TYPES, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 
		DELETION_FILE, DELETION_FORMAT, DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, help ----
		{ APPLICATION_METHOD, "APPLICATION_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "PLANS", ROUTER_METHOD_RANGE, NO_HELP },
		{ STORE_TRIPS_IN_MEMORY, "STORE_TRIPS_IN_MEMORY", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ STORE_PLANS_IN_MEMORY, "STORE_PLANS_IN_MEMORY", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ INITIALIZE_TRIP_PRIORITY, "INITIALIZE_TRIP_PRIORITY", LEVEL0, OPT_KEY, TEXT_KEY, "CRITICAL", "NO, LOW, MEDIUM, HIGH, CRITICAL", NO_HELP },
		{ UPDATE_PLAN_RECORDS, "UPDATE_PLAN_RECORDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ REROUTE_FROM_TIME_POINT, "REROUTE_FROM_TIME_POINT", LEVEL0, OPT_KEY, TIME_KEY, "0:00", "", NO_HELP },
		{ PRINT_UPDATE_WARNINGS, "PRINT_UPDATE_WARNINGS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ OUTPUT_ALL_RECORDS, "OUTPUT_ALL_RECORDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PRELOAD_TRANSIT_VEHICLES, "PRELOAD_TRANSIT_VEHICLES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	Optional_System_Files (optional_files);
	Select_Service_Keys (select_service_keys);

	Router_Service_Keys ();
	Key_List (keys);
	Report_List (reports);
	
	Enable_Partitions (true);
#ifdef MPI_EXE
	Enable_MPI (true);
#endif

	method = TRAVEL_PLANS;
	reroute_time = 0;
	new_set_flag = problem_set_flag = trip_memory_flag = plan_memory_flag = time_sort_flag = false;
	plan_set_flag = update_flag = priority_flag = warn_flag = full_flag = false;
	select_flag = new_delay_flag = flow_flag = turn_flag = reroute_flag = rider_flag = false;
	
	preload_flag = false;
	initial_priority = CRITICAL;

	num_file_sets = 1;
	total_records = num_time_updates = num_trip_parts = num_reroute = num_reskim = num_update = num_build = num_copied = 0;
	select_records = select_weight = random_seed = 0;
	trip_flag = plan_flag = new_plan_flag = problem_flag = false;
	percent_selected = 0.0;

	trip_file = 0;
	plan_file = new_plan_file = 0;
	problem_file = 0;
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	Router *program = 0;
	try {
		program = new Router ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif

