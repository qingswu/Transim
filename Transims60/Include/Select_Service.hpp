//*********************************************************
//	Select_Service.hpp - selection service keys and data
//*********************************************************

#ifndef SELECT_SERVICE_HPP
#define SELECT_SERVICE_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Time_Periods.hpp"
#include "Data_Range.hpp"
#include "Arcview_File.hpp"
#include "Plan_File.hpp"
#include "Execution_Service.hpp"
#include "Selection_File.hpp"
#include "Selection_Data.hpp"

//---------------------------------------------------------
//	Select_Service - system data class definition
//---------------------------------------------------------

class SYSLIB_API Select_Service
{
public:

	Select_Service (void);

	enum Select_Service_Keys { 
		SELECT_HOUSEHOLDS = SELECT_SERVICE_OFFSET, SELECT_MODES, SELECT_PURPOSES, 
		SELECT_PRIORITIES, SELECT_TIME_OF_DAY, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_ORIGINS, SELECT_DESTINATIONS, SELECT_TRAVELER_TYPES, SELECT_FACILITY_TYPES, 
		SELECT_PARKING_LOTS, SELECT_VC_RATIOS, SELECT_TIME_RATIOS, SELECT_TRANSIT_MODES, 
		SELECT_VEHICLE_TYPES, SELECT_PROBLEM_TYPES, SELECT_LINKS, SELECT_NODES, 
		SELECT_STOPS, SELECT_ROUTES, SELECT_SUBAREAS, SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 
		PERCENT_TIME_DIFFERENCE, MINIMUM_TIME_DIFFERENCE, MAXIMUM_TIME_DIFFERENCE, 
		PERCENT_COST_DIFFERENCE, MINIMUM_COST_DIFFERENCE, MAXIMUM_COST_DIFFERENCE, 
		PERCENT_TRIP_DIFFERENCE, MINIMUM_TRIP_DIFFERENCE, MAXIMUM_TRIP_DIFFERENCE, 
		PERCENT_PATH_DIFFERENCE, MINIMUM_PATH_DIFFERENCE, MAXIMUM_PATH_DIFFERENCE, 
		SELECTION_PERCENTAGE, MAXIMUM_PERCENT_SELECTED, SELECTION_POLYGON, 
		DELETION_FILE, DELETION_FORMAT,	DELETE_HOUSEHOLDS, DELETE_MODES, 
		DELETE_TRAVELER_TYPES,
	};
	void Select_Service_Keys (int *keys = 0);

protected:

	void Read_Select_Keys (void);
	bool Select_Plan_Links (Plan_Data &plan);
	bool Select_Plan_Nodes (Plan_Data &plan);
	bool Select_Plan_Stops (Plan_Data &plan);
	bool Select_Plan_Routes (Plan_Data &plan);
	bool Select_Plan_Subareas (Plan_Data &plan);
	bool Select_Plan_Polygon (Plan_Data &plan);
	bool Select_Path_Difference (Plan_Data &plan1, Plan_Data &plan2);

	//---- selection data ----
	
	bool select_households, select_purposes, select_travelers, select_start_times, select_end_times;
	bool select_origins, select_destinations, select_org_zones, select_des_zones, select_links, select_nodes;
	bool select_vehicles, select_problems, select_subareas, select_polygon, percent_flag, max_percent_flag;
	bool select_mode [MAX_MODE], select_facility [EXTERNAL+1], select_modes, select_facilities;
	bool select_transit [ANY_TRANSIT], select_transit_modes, select_priorities, select_priority [SKIP+1];
	bool select_stops, select_routes, select_time_of_day, select_parking, select_vc, select_ratio;
	bool delete_mode [MAX_MODE], delete_modes, delete_travelers, delete_households, delete_flag;
	bool time_diff_flag, cost_diff_flag, trip_diff_flag, path_diff_flag;
	Dtime min_time_diff, max_time_diff, max_min_time_diff, min_trip_diff, max_trip_diff, max_min_trip_diff;
	int min_cost_diff, max_cost_diff, max_min_cost_diff, min_path_diff, max_path_diff, max_min_path_diff;
	double percent_time_diff, percent_cost_diff, percent_trip_diff, percent_path_diff;
	double select_percent, max_percent_select, vc_ratio, time_ratio;;

	Data_Range hhold_range, purpose_range, traveler_range, vehicle_range, problem_range, stop_range, route_range;
	Data_Range org_range, des_range, org_zone_range, des_zone_range, hhold_delete, traveler_delete, parking_range, subarea_range;
	Data_Ranges link_ranges, node_ranges;
	Time_Periods start_range, end_range, time_range;
	Arcview_File polygon_file;
	Select_Map delete_map;

private:
	void Read_Deletions (void);

	Selection_File delete_file;
};

#endif
