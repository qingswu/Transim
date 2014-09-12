//*********************************************************
//	Simulator_Service.cpp - simulation service
//*********************************************************

#include "Simulator_Service.hpp"

Simulator_Service *sim = 0;

//---------------------------------------------------------
//	Simulator_Service constructor
//---------------------------------------------------------

//Simulator_Service::Simulator_Service (void) : Router_Service (), Select_Service ()
Simulator_Service::Simulator_Service (void) : Data_Service (), Select_Service ()
{
	Service_Level (SIMULATOR_SERVICE);
	max_subarea = num_vehicles = 0;
	num_subareas = num_sims = 1;
	max_method = 0;
	end_period = MAX_INTEGER;
	half_second = Dtime (0.5, SECONDS);
	one_second = Dtime (1, SECONDS);
	one_minute = Dtime (60, SECONDS);
	one_hour = Dtime (3600, SECONDS);
	random_time = 0;
	num_travelers = 1000;
	avg_cell_per_veh = 1.25;
	active = false;
	step_code = -1;

#ifdef THREADS
	Enable_Threads (true);
#endif
	sim = this;
}

//---------------------------------------------------------
//	Simulator_Service_Keys
//---------------------------------------------------------

void Simulator_Service::Simulator_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ SIMULATION_START_TIME, "SIMULATION_START_TIME", LEVEL0, OPT_KEY, TIME_KEY, "0:00", "", NO_HELP },
		{ SIMULATION_END_TIME, "SIMULATION_END_TIME", LEVEL0, OPT_KEY, TIME_KEY, "24:00", "", NO_HELP },
		{ SIMULATION_TIME_BREAKS, "SIMULATION_TIME_BREAKS", LEVEL0, OPT_KEY, TEXT_KEY, "NONE", TIME_BREAK_RANGE, NO_HELP }, 
		{ SIMULATION_GROUP_SUBAREAS, "SIMULATION_GROUP_SUBAREAS", LEVEL1, OPT_KEY, LIST_KEY, "ALL", "ALL, 0..10, 15, 20..100", NO_HELP },
		{ GROUP_PERIOD_METHODS, "GROUP_PERIOD_METHODS", LEVEL1, OPT_KEY, LIST_KEY, "MESOSCOPIC", "UNSIM, MACRO, MESO, MICRO", NO_HELP },
		{ RANDOM_NODE_INCREMENT, "RANDOM_NODE_INCREMENT", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", "0..180.0 minutes", NO_HELP },
		{ UNSIMULATED_TIME_STEPS, "UNSIMULATED_TIME_STEPS", LEVEL0, OPT_KEY, TIME_KEY, "1.0 minutes", "1.0..900.0 seconds", NO_HELP },
		{ MACROSCOPIC_TIME_STEPS, "MACROSCOPIC_TIME_STEPS", LEVEL0, OPT_KEY, TIME_KEY, "6.0 seconds", "1.0..10.0 seconds", NO_HELP },
		{ MESOSCOPIC_TIME_STEPS, "MESOSCOPIC_TIME_STEPS", LEVEL0, OPT_KEY, TIME_KEY, "1.0 seconds", "0.5..3.0 seconds", NO_HELP },
		{ MICROSCOPIC_TIME_STEPS, "MICROSCOPIC_TIME_STEPS", LEVEL0, OPT_KEY, TIME_KEY, "0.1 seconds", "0.1..1.0 seconds", NO_HELP },
		{ CELL_SIZE, "CELL_SIZE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 feet", "0, 5..35 feet", NO_HELP },
		{ PLAN_FOLLOWING_DISTANCE, "PLAN_FOLLOWING_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "1700 feet", "0..6000 feet", NO_HELP },
		{ LOOK_AHEAD_DISTANCE, "LOOK_AHEAD_DISTANCE", LEVEL0, OPT_KEY, INT_KEY, "800 feet", "0..6000 feet", NO_HELP },
		{ LOOK_AHEAD_LANE_FACTOR, "LOOK_AHEAD_LANE_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "4.0", "1.0..25.0", NO_HELP },
		{ LOOK_AHEAD_TIME_FACTOR, "LOOK_AHEAD_TIME_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0..5.0", NO_HELP },
		{ LOOK_AHEAD_VEHICLE_FACTOR, "LOOK_AHEAD_VEHICLE_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAXIMUM_SWAPPING_SPEED, "MAXIMUM_SWAPPING_SPEED", LEVEL0, OPT_KEY, FLOAT_KEY, "80 mph", "-1..80 mph", NO_HELP },
		{ MAXIMUM_SPEED_DIFFERENCE, "MAXIMUM_SPEED_DIFFERENCE", LEVEL0, OPT_KEY, FLOAT_KEY, "20 mph", "0..20 mph", NO_HELP },
		{ CAPACITY_FACTOR, "CAPACITY_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.5..3.0", NO_HELP },
		{ ENFORCE_PARKING_LANES, "ENFORCE_PARKING_LANES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ DRIVER_REACTION_TIME, "DRIVER_REACTION_TIME", LEVEL1, OPT_KEY, LIST_KEY, "1.0 seconds", "0.0..5.0 seconds", NO_HELP },
		{ PERMISSION_PROBABILITY, "PERMISSION_PROBABILITY", LEVEL1, OPT_KEY, LIST_KEY, "50 percent", "0..100 percent", NO_HELP },
		{ SLOW_DOWN_PROBABILITY, "SLOW_DOWN_PROBABILITY", LEVEL1, OPT_KEY, LIST_KEY, "0 percent", "0..100 percent", NO_HELP },
		{ SLOW_DOWN_PERCENTAGE, "SLOW_DOWN_PERCENTAGE", LEVEL1, OPT_KEY, LIST_KEY, "0 percent", "0..50 percent", NO_HELP },		
		{ MAX_COMFORTABLE_SPEED, "MAX_COMFORTABLE_SPEED", LEVEL1, OPT_KEY, LIST_KEY, "80.0 mph", "0, 30..80 mph", NO_HELP },
		{ TRAVELER_TYPE_FACTORS, "TRAVELER_TYPE_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..2.0", NO_HELP },
		{ PRIORITY_LOADING_TIME, "PRIORITY_LOADING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "60 seconds", "0..6500 seconds", NO_HELP },
		{ MAXIMUM_LOADING_TIME, "MAXIMUM_LOADING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "120 seconds", "0..6500 seconds", NO_HELP },
		{ PRIORITY_WAITING_TIME, "PRIORITY_WAITING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "60 seconds", "0..6500 seconds", NO_HELP },
		{ MAXIMUM_WAITING_TIME, "MAXIMUM_WAITING_TIME", LEVEL0, OPT_KEY, TIME_KEY, "120 seconds", "0..6500 seconds", NO_HELP },
		{ MAX_DEPARTURE_TIME_VARIANCE, "MAX_DEPARTURE_TIME_VARIANCE", LEVEL0, OPT_KEY, TIME_KEY, "60 minutes", "0..180 minutes", NO_HELP },
		{ MAX_ARRIVAL_TIME_VARIANCE, "MAX_ARRIVAL_TIME_VARIANCE", LEVEL0, OPT_KEY, TIME_KEY, "60 minutes", "0..180 minutes", NO_HELP },
		{ RELOAD_CAPACITY_PROBLEMS, "RELOAD_CAPACITY_PROBLEMS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },		
		{ COUNT_PROBLEM_WARNINGS, "COUNT_PROBLEM_WARNINGS", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ PRINT_PROBLEM_MESSAGES, "PRINT_PROBLEM_MESSAGES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NUMBER_OF_TRAVELERS, "NUMBER_OF_TRAVELERS", LEVEL0, OPT_KEY, INT_KEY, "1000", "0..100000000", NO_HELP },
		END_CONTROL
	};
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
				Error (String ("Simulator Service Key %d was Not Found") % keys [i]);
			}
		}
	}
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_TRAVELER_TYPES, SELECT_VEHICLE_TYPES, SELECT_PROBLEM_TYPES, 0
	};
	Select_Service_Keys (select_service_keys);

	sim_output_step.Add_Keys ();
}

//---------------------------------------------------------
//	Get_Statistics
//---------------------------------------------------------

Sim_Statistics * Simulator_Service::Get_Statistics (void)
{
	sim_travel_step.Add_Statistics (stats);
	sim_node_step.Add_Statistics (stats);
	return (&stats);
}

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool Simulator_Service::Get_Node_Data (Node_File &file, Node_Data &node_rec)
{
	if (Data_Service::Get_Node_Data (file, node_rec)) {
		if (node_rec.Subarea () > max_subarea) max_subarea = node_rec.Subarea ();
		subarea_map.insert (Int_Map_Data (node_rec.Subarea (), (int) subarea_map.size ()));
		return (true);
	}
	return (false);
}
