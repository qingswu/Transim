//*********************************************************
//	TransitNet.hpp - Transit Conversion Utility
//*********************************************************

#ifndef TRANSITNET_HPP
#define TRANSITNET_HPP

#include "Data_Service.hpp"
#include "Db_Array.hpp"
#include "Db_Header.hpp"
#include "Time_Periods.hpp"
#include "Best_List.hpp"
#include "TypeDefs.hpp"

#include "Node_Path_Data.hpp"

//#define MIN_LENGTH		37.5
//#define MAX_AREA_TYPE	16

//---------------------------------------------------------
//	TransitNet - execution class definition
//---------------------------------------------------------

class SYSLIB_API TransitNet : public Data_Service
{
public:
	TransitNet (void);

	virtual void Execute (void);

protected:
	enum TransimsNet_Keys { 
		ROUTE_DATA_FILE = 1, ROUTE_DATA_FORMAT, ROUTE_JOIN_FIELD,
		PARK_AND_RIDE_FILE, PNR_ACCESS_DISTANCE, STATION_ACCESS_DISTANCE, 
		STOP_SPACING_BY_AREA_TYPE, STOP_FACILITY_TYPE_RANGE,
		TRANSIT_TRAVEL_TIME_FACTORS, MINIMUM_DWELL_TIME,
		INTERSECTION_STOP_TYPE, INTERSECTION_STOP_OFFSET, COORDINATE_SCHEDULES,
		IGNORE_PATH_ERRORS, NEW_ROUTE_CHANGE_FILE, NEW_ROUTE_CHANGE_FORMAT
	};
	virtual void Program_Control (void);
	virtual int Put_Line_Data (Line_File &file, Line_Data &data);

private:
	enum TransitNet_Reports {ZONE_EQUIV = 1};

	bool route_data_flag, parkride_flag, equiv_flag, coordinate_flag, ignore_errors_flag;
	bool dwell_flag, time_flag, speed_flag, at_flag, access_flag, new_link_flag, change_flag;
	bool facility_flag [EXTERNAL+1];

	int naccess, nlocation, nparking, line_edit, route_edit, schedule_edit, driver_edit;
	int nstop, nroute, nschedule, ndriver, end_warnings, parking_warnings, max_link;
	int max_parking, max_access, max_location, max_stop, nparkride, new_access, new_stop;
	int min_dwell, num_periods, stop_type, left_turn, bus_code, rail_code, stop_offset;
	int route_data_field, route_join_field, PNR_distance, station_distance;
	int change_route_fld, change_mode_fld, change_name_fld, change_in_len_fld, change_out_len_fld, change_in_stops_fld, change_out_stops_fld;

	Double_List min_stop_spacing, time_factor;

	Db_Header route_data_file, parkride_file, change_file;
	Db_Sort_Array route_data_array;

	Node_Path_Array node_path_array;
	Path_Leg_Array path_leg_array;

	Point_Map stop_pt;
	Integers node_list, dir_list, local_access, fare_zone;

	vector <Int_Map> dir_stop_array;

	Str_ID join_map;

	//---- methods ----

	void Read_Route_Data (void);
	void Data_Setup (void);
	void Add_Link (int anode, int bnode);
	void Read_ParkRide (void);
	void Node_Path (int node1, int node2, Use_Type use, int index = -1);
	void Build_Routes (void);
	void Coordinate_Schedules (void);
	void Station_Access (void);
};
#endif
