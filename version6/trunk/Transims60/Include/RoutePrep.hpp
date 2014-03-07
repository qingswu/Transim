//*********************************************************
//	RoutePrep.hpp - Transit Route Preparation Utility
//*********************************************************

#ifndef ROUTEPREP_HPP
#define ROUTEPREP_HPP

#include "Data_Service.hpp"
#include "User_Program.hpp"
#include "Data_Range.hpp"
#include "Projection_Service.hpp"
#include "Arcview_File.hpp"
#include "Best_List.hpp"
#include "List_Data.hpp"
#include "TypeDefs.hpp"
#include "Link_Detail_File.hpp"
#include "Route_Nodes_File.hpp"
#include "Db_File.hpp"
#include "Best_List.hpp"

//---------------------------------------------------------
//	RoutePrep - execution class definition
//---------------------------------------------------------

class SYSLIB_API RoutePrep : public Data_Service
{
public:

	RoutePrep (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:
	enum RoutePrep_Keys { 
		FIRST_NODE_NUMBER = 1, FIRST_LINK_NUMBER, FIRST_ROUTE_NUMBER, ROUTE_MODE_MAP, MODE_VEH_TYPE_MAP, 
		CONVERT_NODE_NUMBERS, INPUT_NODE_FILE, INPUT_NODE_FORMAT, NODE_MAP_FILE, NEW_NODE_MAP_FILE,

		TRANSIT_TIME_PERIODS, TRANSIT_PERIOD_OFFSETS, PERIOD_TRAVEL_TIMES, TRANSIT_NODE_TYPES, 
		COLLAPSE_ROUTE_DATA, COORDINATE_RESOLUTION, CONVERSION_SCRIPT, 

		ROUTE_SHAPE_FILE, ROUTE_NAME_FIELD, ROUTE_LINK_FIELD, ROUTE_ORDER_FIELD,
		ROUTE_HEADWAY_FIELD, ROUTE_FREQUENCY_FIELD,
		ROUTE_STOP_FILE, ROUTE_STOP_FORMAT, ROUTE_STOP_ROUTE_FIELD, ROUTE_STOP_STOP_FIELD, 
		ROUTE_STOP_OFFSET_FIELD, ROUTE_STOP_ORDER_FIELD, ROUTE_LINK_LENGTH,

		STATION_NODE_FILE, STATION_NODE_FORMAT, STATION_NODE_FIELD, STATION_NAME_FIELD, 
		STATION_X_FIELD, STATION_Y_FIELD, 
		STATION_LINK_LENGTH, STATION_PLATFORM_TIME, STATION_PLATFORM_FLAG, STATION_PLATFORM_OFFSET,

		INPUT_ROUTE_FILE, INPUT_ROUTE_FORMAT, ROUTE_PERIOD_MAP, ROUTE_VEHICLE_TYPE, NEW_ROUTE_FILE, NEW_ROUTE_FORMAT,

		ROUTE_SPEED_FILE, ROUTE_SPEED_FORMAT, SPEED_ROUTE_FIELD, SPEED_PEAK_FIELD, SPEED_OFFPEAK_FIELD,
		SEGMENT_SPEED_FACTOR_FILE, SEGMENT_SPEED_FACTOR_FORMAT, SEGMENT_FIELD, SPEED_FACTOR_FIELD,
		IGNORE_EXISTING_SPEEDS,
	};
	virtual void Program_Control (void);

private:
	enum RoutePrep_Reports { SCRIPT_REPORT = 1, STACK_REPORT, SEGMENT_NODES };
	
	int new_link, new_node, new_route, num_periods, num_match;
	int route_name_field, route_link_field, route_order_field;
	int rstop_route_field, rstop_stop_field, rstop_offset_field, rstop_order_field;
	int station_node_field, station_name_field, station_x_field, station_y_field;
	int speed_route_field, speed_peak_field, speed_offpeak_field, segment_field, speed_fac_field;
	bool convert_flag, shape_flag, offset_flag, time_flag, collapse_routes, in_route_flag, out_route_flag, segment_report;
	bool route_freq_flag, route_shape_flag, route_stop_flag, station_flag, platform_flag, new_route_flag, route_speed_flag, seg_fac_flag;
	bool convert_node_flag, input_node_flag, node_map_flag, new_map_flag, ignore_speeds;
	double resolution, station_length, platform_speed, platform_offset, link_length;
	Dtime platform_time;

	List_Array node_list;
	Integers route_headway_flds;
	Ints_Array station_nodes;
	Int_Map input_map;

	Db_Header route_stop_file, station_file, speed_file, seg_fac_file, input_node_file;
	Db_File script_file, node_map_file, new_map_file;
	Link_File *new_link_file;

	Shape_File_Array shape_file_array;
	Arcview_File *route_shape_file;
	Route_Nodes_File *new_route_nodes;

	User_Program convert;
	
	Projection projection;
	Projection_Service proj_service;

	//---- transit line files ----

	Time_Periods schedule_periods;

	typedef struct {
		Dtimes headways;
		Int2_Set links;
	} Route_Link_Data;

	typedef map <String, Route_Link_Data>    Route_Link_Map;
	typedef pair <String, Route_Link_Data>   Route_Link_Map_Data;
	typedef Route_Link_Map::iterator         Route_Link_Map_Itr;
	typedef pair <Route_Link_Map_Itr, bool>  Route_Link_Map_Stat;

	Route_Link_Map route_link_map;

	typedef struct {
		int stop;
		double offset;
	} Stop_Offset_Data;

	typedef map <int, Stop_Offset_Data>       Stop_Offset_Map;
	typedef pair <int, Stop_Offset_Data>      Stop_Offset_Map_Data;
	typedef Stop_Offset_Map::iterator         Stop_Offset_Map_Itr;
	typedef pair <Stop_Offset_Map_Itr, bool>  Stop_Offset_Map_Stat;

	typedef map <String, Stop_Offset_Map>     Route_Stop_Map;
	typedef pair <String, Stop_Offset_Map>    Route_Stop_Map_Data;
	typedef Route_Stop_Map::iterator          Route_Stop_Map_Itr;
	typedef pair <Route_Stop_Map_Itr, bool>   Route_Stop_Map_Stat;

	Route_Stop_Map route_stop_map;
	
	typedef struct {
		bool oneway, flip;
		int mode;
		Dtime ttime, offset;
		String name, notes;
		double peak, offpeak;
		Dtimes headway;
		Doubles speeds;
		Integers nodes;
	} Route_Data; 

	typedef struct {
		String name;
		Integers links;
	} Station_Data;

	typedef map <int, Station_Data>    Station_Map;
	typedef pair <int, Station_Data>   Station_Map_Data;
	typedef Station_Map::iterator  Station_Map_Itr;
	typedef pair <Station_Map_Itr, bool>  Station_Map_Stat;

	Station_Map station_map;
	
	Integers mode_map;
	Integer_List mode_type_map;

	typedef struct {
		int group;
		int veh_type;
		Route_Nodes_File *in_file;
		Route_Nodes_File *out_file;
		Integer_List period_map;
	} File_Group;

	typedef vector <File_Group>        File_Group_Array;
	typedef File_Group_Array::iterator File_Group_Itr;
	
	File_Group_Array file_groups;
	Ints_Map segments;

	Str_ID speed_map;
	Dbls_Array speed_array;
	Int_Dbl_Map segment_factors;

	void Read_Scripts (void);
	void Read_Route_Stops (void);
	void Read_Routes (void);
	int  Insert_Link (Points &points);
	void Link_Order (void);
	void Read_Stations (void);
	void Insert_Stations (void);
	void Endline_Station (int node, int link);
	void Inline_Station (int node, int link1, int link2);
	void Update_Routes (int node, int from, int to, int new1, int new2);
	void Platform_Link (int link);
	void Transfer_Link (void);
	void Input_Routes (void);
	void Save_Route (Route_Data &data);
	void Write_Route (Db_File *file, Route_Data &data);
	void Route_Segments (void);
	void Read_Speeds (void);
	void Read_Speed_Factors (void);
	void Read_Node_Map (void);
	void Read_Node_File (void);
	void Write_Node_Map (void);
};
#endif
