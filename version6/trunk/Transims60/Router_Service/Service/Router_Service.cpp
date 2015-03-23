//*********************************************************
//	Router_Service.cpp - path building service
//*********************************************************

#include "Router_Service.hpp"

//---------------------------------------------------------
//	Router_Service constructor
//---------------------------------------------------------

Router_Service::Router_Service (void) : Data_Service (), Flow_Time_Service ()
{
	Service_Level (ROUTER_SERVICE);
	
	System_File_Type required_files [] = {
		NODE, LINK, CONNECTION, LOCATION, END_FILE
	};
	System_File_Type optional_files [] = {
		POCKET, LANE_USE, TURN_PENALTY, PARKING, ACCESS_LINK, 
		TRANSIT_STOP, TRANSIT_FARE, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		HOUSEHOLD, SELECTION, PERFORMANCE, TURN_DELAY, VEHICLE_TYPE,  
		RIDERSHIP, NEW_PLAN, NEW_PROBLEM, NEW_PERFORMANCE, END_FILE
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);

	walk_path_flag = bike_path_flag = drive_path_flag = access_link_flag = true;
	transit_path_flag = parkride_path_flag = kissride_path_flag = skim_check_flag = true;
	select_flag = update_flag = thread_flag = false;

	Reset_Skim_Gap ();

#ifdef THREADS
	Enable_Threads (true);
#endif
}

//---------------------------------------------------------
//	Router_Service_Keys
//---------------------------------------------------------

void Router_Service::Router_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ IMPEDANCE_SORT_METHOD, "IMPEDANCE_SORT_METHOD", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SAVE_ONLY_SKIMS, "SAVE_ONLY_SKIMS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ WALK_PATH_DETAILS, "WALK_PATH_DETAILS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ IGNORE_VEHICLE_ID, "IGNORE_VEHICLE_ID", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ LIMIT_PARKING_ACCESS, "LIMIT_PARKING_ACCESS", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ ADJUST_ACTIVITY_SCHEDULE, "ADJUST_ACTIVITY_SCHEDULE", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ IGNORE_ACTIVITY_DURATIONS, "IGNORE_ACTIVITY_DURATIONS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ IGNORE_TIME_CONSTRAINTS, "IGNORE_TIME_CONSTRAINTS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ END_TIME_CONSTRAINT, "END_TIME_CONSTRAINT", LEVEL0, OPT_KEY, TIME_KEY, "0 minutes", "0..360 minutes", NO_HELP },
		{ IGNORE_ROUTING_PROBLEMS, "IGNORE_ROUTING_PROBLEMS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ TRANSIT_CAPACITY_PENALTY, "TRANSIT_CAPACITY_PENALTY", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PERCENT_RANDOM_IMPEDANCE, "PERCENT_RANDOM_IMPEDANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 percent", "0.0..100.0 percent", NO_HELP },
		{ TRAVELER_TYPE_SCRIPT, "TRAVELER_TYPE_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRAVELER_PARAMETER_FILE, "TRAVELER_PARAMETER_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ WALK_SPEED, "WALK_SPEED", LEVEL0, OPT_KEY, FLOAT_KEY, "3.0 mph", "1.5..12.0 mph", NO_HELP },
		{ BICYCLE_SPEED, "BICYCLE_SPEED", LEVEL0, OPT_KEY, FLOAT_KEY, "12.0 mph", "3.0..30.0 mph", NO_HELP },
		{ WALK_TIME_VALUES, "WALK_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "20.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ BICYCLE_TIME_VALUES, "BICYCLE_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "15.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ FIRST_WAIT_VALUES, "FIRST_WAIT_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "20.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ TRANSFER_WAIT_VALUES, "TRANSFER_WAIT_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "20.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ PARKING_TIME_VALUES, "PARKING_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "0.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ VEHICLE_TIME_VALUES, "VEHICLE_TIME_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "10.0 impedance/second", "0.0..1000.0", NO_HELP },
		{ DISTANCE_VALUES, "DISTANCE_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "0.0 impedance/foot", "0.0..1000.0", NO_HELP },
		{ COST_VALUES, "COST_VALUES", LEVEL1, OPT_KEY, LIST_KEY, "0.0 impedance/cent", "0.0..1000.0", NO_HELP },
		{ FREEWAY_BIAS_FACTORS, "FREEWAY_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..2.0", NO_HELP },
		{ EXPRESSWAY_BIAS_FACTORS, "EXPRESSWAY_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.5..2.0", NO_HELP },
		{ LEFT_TURN_PENALTIES, "LEFT_TURN_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ RIGHT_TURN_PENALTIES, "RIGHT_TURN_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ U_TURN_PENALTIES, "U_TURN_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ TRANSFER_PENALTIES, "TRANSFER_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..100000", NO_HELP },
		{ STOP_WAITING_PENALTIES, "STOP_WAITING_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..100000", NO_HELP },
		{ STATION_WAITING_PENALTIES, "STATION_WAITING_PENALTIES", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..100000", NO_HELP },
		{ BUS_BIAS_FACTORS, "BUS_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "1.0..3.0", NO_HELP },
		{ BUS_BIAS_CONSTANTS, "BUS_BIAS_CONSTANTS", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ BRT_BIAS_FACTORS, "BRT_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "1.0..3.0", NO_HELP },
		{ BRT_BIAS_CONSTANTS, "BRT_BIAS_CONSTANTS", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "0..10000", NO_HELP },
		{ RAIL_BIAS_FACTORS, "RAIL_BIAS_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "1.0", "0.1..1.0", NO_HELP },
		{ RAIL_BIAS_CONSTANTS, "RAIL_BIAS_CONSTANTS", LEVEL1, OPT_KEY, LIST_KEY, "0 impedance", "-1000..0", NO_HELP },
		{ MAX_WALK_DISTANCES, "MAX_WALK_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "6000 feet", "300..60000 feet", NO_HELP },
		{ WALK_PENALTY_DISTANCES, "WALK_PENALTY_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "6000 feet", "300..30000 feet", NO_HELP },
		{ WALK_PENALTY_FACTORS, "WALK_PENALTY_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAX_BICYCLE_DISTANCES, "MAX_BICYCLE_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "30000 feet", "3000..120000 feet", NO_HELP },
		{ BIKE_PENALTY_DISTANCES, "BIKE_PENALTY_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "30000 feet", "3000..60000 feet", NO_HELP },
		{ BIKE_PENALTY_FACTORS, "BIKE_PENALTY_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAX_WAIT_TIMES, "MAX_WAIT_TIMES", LEVEL1, OPT_KEY, LIST_KEY, "60 minutes", "5..400 minutes", NO_HELP },
		{ WAIT_PENALTY_TIMES, "WAIT_PENALTY_TIMES", LEVEL1, OPT_KEY, LIST_KEY, "60 minutes", "5..200 minutes", NO_HELP },
		{ WAIT_PENALTY_FACTORS, "WAIT_PENALTY_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MIN_WAIT_TIMES, "MIN_WAIT_TIMES", LEVEL1, OPT_KEY, LIST_KEY, "0 seconds", "0..3600 seconds", NO_HELP },
		{ MAX_NUMBER_OF_TRANSFERS, "MAX_NUMBER_OF_TRANSFERS", LEVEL1, OPT_KEY, LIST_KEY, "3", "0..10", NO_HELP },
		{ MAX_PARK_RIDE_PERCENTS, "MAX_PARK_RIDE_PERCENTS", LEVEL1, OPT_KEY, LIST_KEY, "50 percent", "1..100 percent", NO_HELP },
		{ MAX_KISS_RIDE_PERCENTS, "MAX_KISS_RIDE_PERCENTS", LEVEL1, OPT_KEY, LIST_KEY, "35 percent", "1..100 percent", NO_HELP },
		{ KISS_RIDE_TIME_FACTORS, "KISS_RIDE_TIME_FACTORS", LEVEL1, OPT_KEY, LIST_KEY, "2.5", "1.0..4.4", NO_HELP },
		{ KISS_RIDE_STOP_TYPES, "KISS_RIDE_STOP_TYPES", LEVEL0, OPT_KEY, TEXT_KEY, "EXTERNAL", "", NO_HELP },
		{ MAX_KISS_RIDE_DROPOFF_WALK, "MAX_KISS_RIDE_DROPOFF_WALK", LEVEL0, OPT_KEY, FLOAT_KEY, "300 feet", "30..1500 feet", NO_HELP },
		{ TRANSIT_PENALTY_FILE, "TRANSIT_PENALTY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PARKING_PENALTY_FILE, "PARKING_PENALTY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DEFAULT_PARKING_DURATION, "DEFAULT_PARKING_DURATION", LEVEL0, OPT_KEY, TIME_KEY, "0.0 hours", "0.0..24.0 hours", NO_HELP },
		{ MAX_LEGS_PER_PATH, "MAX_LEGS_PER_PATH", LEVEL0, OPT_KEY, INT_KEY, "1000", "10..10000", NO_HELP },
		{ MAX_NUMBER_OF_PATHS, "MAX_NUMBER_OF_PATHS", LEVEL0, OPT_KEY, INT_KEY, "4", "1..10", NO_HELP },
		{ FARE_CLASS_DISTRIBUTION, "FARE_CLASS_DISTRIBUTION", LEVEL0, OPT_KEY, LIST_KEY, "0", "", NO_HELP },
		{ LOCAL_ACCESS_DISTANCE, "LOCAL_ACCESS_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "6000 feet", "600..25000 meters", NO_HELP },
		{ LOCAL_FACILITY_TYPE, "LOCAL_FACILITY_TYPE", LEVEL0, OPT_KEY, TEXT_KEY, "EXTERNAL", "MAJOR..LOCAL, EXTERNAL", NO_HELP },
		{ LOCAL_IMPEDANCE_FACTOR, "LOCAL_IMPEDANCE_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..25.0", NO_HELP },
		{ MAX_CIRCUITY_RATIO, "MAX_CIRCUITY_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0..10.0", NO_HELP },
		{ MIN_CIRCUITY_DISTANCE, "MIN_CIRCUITY_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "6000 feet", "0..30000 feet", NO_HELP },
		{ MAX_CIRCUITY_DISTANCE, "MAX_CIRCUITY_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "60000 feet", "0..300000 feet", NO_HELP },
		{ MIN_DURATION_FACTORS, "MIN_DURATION_FACTORS", LEVEL0, OPT_KEY, LIST_KEY, "0.1, 0.5, 0.8, 1.0", "0.0..1.0", NO_HELP },
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
				Error (String ("Router Service Key %d was Not Found") % keys [i]);
			}
		}
	}
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, 0
	};
	File_Service_Keys (file_service_keys);

	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, TRIP_SORT_TYPE, PLAN_SORT_TYPE, 0
	};
	Data_Service_Keys (data_service_keys);

	int flow_time_service_keys [] = {
		UPDATE_FLOW_RATES,  UPDATE_TURNING_MOVEMENTS, CLEAR_INPUT_FLOW_RATES, 
		UPDATE_TRAVEL_TIMES, TIME_UPDATE_RATE, AVERAGE_TRAVEL_TIMES, LINK_FLOW_FACTOR, EQUATION_PARAMETERS, 0
	};
	Flow_Time_Service_Keys (flow_time_service_keys);
}

//---------------------------------------------------------
//	Router_Service -- Skim_Gap
//---------------------------------------------------------

double Router_Service::Skim_Gap (void)
{
	return ((skim_time > 0) ? skim_gap / skim_time : skim_gap); 
}
