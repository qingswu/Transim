//*********************************************************
//	Converge_Service.cpp - convergence service keys and data
//*********************************************************

#include "Converge_Service.hpp"

const char * Converge_Service::reports [] = {
	"TRAVELER_TYPE_SCRIPT",
	"TRAVELER_TYPE_STACK",
	"LINK_GAP_REPORT",
	"TRIP_GAP_REPORT",
	"ITERATION_PROBLEMS", 
	""
};

//---------------------------------------------------------
//	Converge_Service constructor
//---------------------------------------------------------

Converge_Service::Converge_Service (void) : Router_Service (), Select_Service ()
{
	Service_Level (CONVERGE_SERVICE);

	first_iteration = true;
	max_iteration = 1;
	num_parts = 0;
	max_speed_updates = 0;
	min_speed_diff = 0.01;
	num_trip_rec = num_trip_sel = 0;
	loc_cap_field = loc_fuel_field = -1;
	;
	initial_factor = factor_increment = maximum_factor = link_gap = trip_gap = transit_gap = 0.0;
	reroute_time = 0;
	
	method = TRAVEL_PLANS;

	cap_duration = fuel_duration = Dtime (5.0, MINUTES);
	initial_priority = CRITICAL;	
	total_records = num_time_updates = num_trip_parts = select_records = select_weight = 0;
	num_reroute = num_reskim = num_update = num_build = num_copied = 0;
	percent_selected = 0.0;
	minimum_fuel = seek_fuel = 0;

	min_vht_flag = link_gap_flag = trip_gap_flag = trip_gap_map_flag = save_link_gap = save_trip_gap = memory_flag = false;
	iteration_flag = save_iter_flag = link_report_flag = trip_report_flag = trip_gap_parts = save_plan_flag = false;
	script_flag = hhfile_flag = capacity_flag = fuel_flag = priority_flag = reroute_flag = sched_acc_flag = false;
	new_set_flag = problem_set_flag = trip_memory_flag = plan_memory_flag = preload_flag = info_flag = false;
	plan_set_flag = update_flag = select_flag = flow_flag = turn_flag = rider_flag = time_order_flag = false;
	trip_flag = plan_flag = new_plan_flag = problem_flag = cap_const_flag = fuel_const_flag = choice_flag = false;

	trip_file = 0;
	plan_file = new_plan_file = 0;
	problem_file = 0;

	System_Data_Reserve (HOUSEHOLD, 0);

	Enable_Partitions (true);
#ifdef MPI_EXE
	Enable_MPI (true);
#endif
}

//---------------------------------------------------------
//	Converge_Service_Keys
//---------------------------------------------------------

void Converge_Service::Converge_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ APPLICATION_METHOD, "APPLICATION_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "PLANS", ROUTER_METHOD_RANGE, NO_HELP },
		{ STORE_TRIPS_IN_MEMORY, "STORE_TRIPS_IN_MEMORY", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ STORE_PLANS_IN_MEMORY, "STORE_PLANS_IN_MEMORY", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ INITIALIZE_TRIP_PRIORITY, "INITIALIZE_TRIP_PRIORITY", LEVEL0, OPT_KEY, TEXT_KEY, "CRITICAL", "NO, LOW, MEDIUM, HIGH, CRITICAL", NO_HELP },
		{ PRELOAD_TRANSIT_VEHICLES, "PRELOAD_TRANSIT_VEHICLES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ MAXIMUM_NUMBER_OF_ITERATIONS, "MAXIMUM_NUMBER_OF_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "0", "0..1000", NO_HELP },
		{ LINK_CONVERGENCE_CRITERIA, "LINK_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ TRIP_CONVERGENCE_CRITERIA, "TRIP_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ TRANSIT_CAPACITY_CRITERIA, "TRANSIT_CAPACITY_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0..10.0", NO_HELP },
		{ INITIAL_WEIGHTING_FACTOR, "INITIAL_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", ">= 0.0", NO_HELP },
		{ ITERATION_WEIGHTING_INCREMENT, "ITERATION_WEIGHTING_INCREMENT", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..5.0", NO_HELP },
		{ MAXIMUM_WEIGHTING_FACTOR, "MAXIMUM_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "20.0", "0.0, >= 2.0", NO_HELP },
		{ MINIMIZE_VEHICLE_HOURS, "MINIMIZE_VEHICLE_HOURS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ MAXIMUM_RESKIM_ITERATIONS, "MAXIMUM_RESKIM_ITERATIONS", LEVEL0, OPT_KEY, INT_KEY, "0", "0..100", NO_HELP },
		{ RESKIM_CONVERGENCE_CRITERIA, "RESKIM_CONVERGENCE_CRITERIA", LEVEL0, OPT_KEY, FLOAT_KEY, "0.02", "0..10.0", NO_HELP },
		{ NEW_LINK_CONVERGENCE_FILE, "NEW_LINK_CONVERGENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_CONVERGENCE_FILE, "NEW_TRIP_CONVERGENCE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ SAVE_AFTER_ITERATIONS, "SAVE_AFTER_ITERATIONS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ NEW_SAVE_PLAN_FILE, "NEW_SAVE_PLAN_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_SAVE_PLAN_FORMAT, "NEW_SAVE_PLAN_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ SAVE_PLAN_HOUSEHOLDS, "SAVE_PLAN_HOUSEHOLDS", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ CAPACITY_CONSTRAINT_FIELD, "CAPACITY_CONSTRAINT_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NEXT_DESTINATION_FIELDS, "NEXT_DESTINATION_FIELDS", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ DESTINATION_SHARE_FIELDS, "DESTINATION_SHARE_FIELDS", LEVEL1, OPT_KEY, LIST_KEY, "", "", NO_HELP },
		{ CONSTRAINED_ACTIVITY_DURATION, "CONSTRAINED_ACTIVITY_DURATION", LEVEL0, OPT_KEY, TIME_KEY, "5 minutes", "0..60 minutes", NO_HELP },
		{ NEW_CAPACITY_CONSTRAINT_FILE, "NEW_CAPACITY_CONSTRAINT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ DESTINATION_CHOICE_TRAVELERS, "DESTINATION_CHOICE_TRAVELERS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ FUEL_SUPPLY_FIELD, "FUEL_SUPPLY_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ FUEL_ACTIVITY_DURATION, "FUEL_ACTIVITY_DURATION", LEVEL0, OPT_KEY, TIME_KEY, "5 minutes", "0..60 minutes", NO_HELP },
		{ MINIMUM_INITIAL_FUEL, "MINIMUM_INITIAL_FUEL", LEVEL0, OPT_KEY, INT_KEY, "25 percent", "1..100 percent", NO_HELP },
		{ SEEK_FUEL_LEVEL, "SEEK_FUEL_LEVEL", LEVEL0, OPT_KEY, INT_KEY, "25 percent", "1..100 percent", NO_HELP },
		{ NEW_FUEL_CONSTRAINT_FILE, "NEW_FUEL_CONSTRAINT_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ FUEL_INFORMATION_TRAVELERS, "FUEL_INFORMATION_TRAVELERS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ SCHEDULED_ACCESS_TRAVELERS, "SCHEDULED_ACCESS_TRAVELERS", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ CONSTRAINT_TIME_INCREMENT, "CONSTRAINT_TIME_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", MINUTE_RANGE, NO_HELP },
		END_CONTROL
	};

	Router_Service_Keys ();

	if (keys == 0) {
		Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				Error (String ("Converge Service Key %d was Not Found") % keys [i]);
			}
		}
	}
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_PRIORITIES, 
		SELECT_START_TIMES, SELECT_END_TIMES, SELECT_ORIGINS, SELECT_DESTINATIONS, 
		SELECT_TRAVELER_TYPES, SELECT_VEHICLE_TYPES, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 
		DELETION_FILE, DELETION_FORMAT, DELETE_HOUSEHOLDS, DELETE_MODES, DELETE_TRAVELER_TYPES, 
		PERCENT_TIME_DIFFERENCE, MINIMUM_TIME_DIFFERENCE, MAXIMUM_TIME_DIFFERENCE, 
		PERCENT_COST_DIFFERENCE, MINIMUM_COST_DIFFERENCE, MAXIMUM_COST_DIFFERENCE, 
		PERCENT_TRIP_DIFFERENCE, MINIMUM_TRIP_DIFFERENCE, MAXIMUM_TRIP_DIFFERENCE,
		SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, 0
	};
	Select_Service_Keys (select_service_keys);
}
