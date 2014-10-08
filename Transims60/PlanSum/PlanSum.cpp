//*********************************************************
//	PlanSum.cpp - travel plan summary utility
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	PlanSum constructor
//---------------------------------------------------------

PlanSum::PlanSum (void) : Data_Service (), Select_Service (), Flow_Time_Service ()
{
	Program ("PlanSum");
	Version (8);

	Title ("Travel Plan Summary Utility");

	System_File_Type required_files [] = {
		PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		NODE, LINK, ZONE, CONNECTION, LANE_USE, LOCATION, PARKING, ACCESS_LINK, 
		TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		SELECTION, VEHICLE_TYPE, PERFORMANCE, TURN_DELAY, NEW_PERFORMANCE, NEW_TURN_DELAY, 
		NEW_RIDERSHIP, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, SAVE_LANE_USE_FLOWS, ZONE_EQUIVALENCE_FILE, 
		LINK_EQUIVALENCE_FILE, STOP_EQUIVALENCE_FILE, LINE_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, PERIOD_CONTROL_POINT, 0
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECT_MODES, SELECT_PURPOSES, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_VEHICLE_TYPES, SELECT_TRAVELER_TYPES, 
		SELECT_LINKS, SELECT_NODES, SELECT_SUBAREAS, SELECT_STOPS, SELECT_ROUTES, SELECTION_POLYGON, SELECTION_PERCENTAGE, 0
	};
	int flow_time_service_keys [] = {
		UPDATE_FLOW_RATES, CLEAR_INPUT_FLOW_RATES, UPDATE_TURNING_MOVEMENTS, UPDATE_TRAVEL_TIMES, 
		LINK_FLOW_FACTOR, EQUATION_PARAMETERS, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NEW_TRIP_TIME_FILE, "NEW_TRIP_TIME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINK_VOLUME_FILE, "NEW_LINK_VOLUME_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ACCESS_DETAIL_FILE, "NEW_ACCESS_DETAIL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ACCESS_GROUP_FILE, "NEW_ACCESS_GROUP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_STOP_DIURNAL_FILE, "NEW_STOP_DIURNAL_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_LINE_ON_OFF_FILE, "NEW_LINE_ON_OFF_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_STOP_BOARDING_FILE, "NEW_STOP_BOARDING_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"TOP_100_V/C_RATIOS",
		"ALL_V/C_RATIOS_GREATER_THAN_*",
		"LINK_GROUP_V/C_RATIOS_*",
		"ZONE_EQUIVALENCE",
		"LINK_EQUIVALENCE",
		"STOP_EQUIVALENCE",
		"LINE_EQUIVALENCE",
		"TRANSIT_RIDERSHIP_SUMMARY",
		"TRANSIT_STOP_SUMMARY",
		"TRANSIT_TRANSFER_SUMMARY",
		"TRANSIT_TRANSFER_DETAILS",
		"TRANSIT_STOP_GROUP_SUMMARY",
		"TRANSIT_STOP_GROUP_DETAILS",
		"TRANSIT_LINE_GROUP_SUMMARY",
		"TRANSIT_LINE_GROUP_DETAILS",
		"TRANSIT_PASSENGER_SUMMARY",
		"TRANSIT_LINK_GROUP_SUMMARY",
		"LINE_TO_LINE_TRANSFERS",
		"STOP_GROUP_ACCESS_DETAILS",
		"TRIP_TIME_REPORT",
		"TRAVEL_SUMMARY_REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);
	Select_Service_Keys (select_service_keys);
	Flow_Time_Service_Keys (flow_time_service_keys);

	Key_List (keys);
	Report_List (reports);
	Enable_Partitions (true);

#ifdef THREADS
	Enable_Threads (true);
#endif
#ifdef MPI_EXE
	Enable_MPI (true);
#endif
	max_rail = 0;

	select_flag = new_perf_flag = turn_flag = skim_flag = trip_flag = zone_flag = time_flag = travel_flag = false; 
	volume_flag = passenger_flag = transfer_flag = rider_flag = xfer_flag = xfer_detail = line_xfer_flag = false;
	access_flag = zone_file_flag = access_file_flag = diurnal_flag = on_off_flag = boarding_flag = false;
	access_group_flag = false;
	cap_factor = 0.25;
	minimum_vc = 1.0;

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
	PlanSum *program = 0;
	try {
		program = new PlanSum ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif

//---------------------------------------------------------
//	Xfer_IO less than operator
//---------------------------------------------------------

bool operator < (Xfer_IO left, Xfer_IO right)
{
	if (left.group < right.group) return (true);
	if (left.group == right.group) {	
		if (left.period < right.period) return (true);
		if (left.period == right.period) {
			if (left.from_stop < right.from_stop) return (true);
			if (left.from_stop == right.from_stop) {
				if (left.from_line < right.from_line) return (true);
				if (left.from_line == right.from_line) {
					if (left.to_stop < right.to_stop) return (true);
					if (left.to_stop == right.to_stop) {
						if (left.to_line < right.to_line) return (true);
					}
				}
			}
		}
	}
	return (false); 
}
