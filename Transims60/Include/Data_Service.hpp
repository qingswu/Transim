//*********************************************************
//	Data_Service.hpp - system data input service
//*********************************************************

#ifndef DATA_SERVICE_HPP
#define DATA_SERVICE_HPP

#include "APIDefs.hpp"
#include "File_Service.hpp"
#include "TypeDefs.hpp"
#include "Compass.hpp"
#include "Time_Periods.hpp"
#include "Data_Range.hpp"

#include "Node_Data.hpp"
#include "Zone_Data.hpp"
#include "Shape_Data.hpp"
#include "Link_Data.hpp"
#include "Dir_Data.hpp"
#include "Pocket_Data.hpp"
#include "Lane_Use_Data.hpp"
#include "Connect_Data.hpp"
#include "Turn_Pen_Data.hpp"
#include "Parking_Data.hpp"
#include "Location_Data.hpp"
#include "Access_Data.hpp"
#include "Sign_Data.hpp"
#include "Signal_Data.hpp"
#include "Timing_Data.hpp"
#include "Phasing_Data.hpp"
#include "Detector_Data.hpp"
#include "Stop_Data.hpp"
#include "Fare_Data.hpp"
#include "Line_Data.hpp"
#include "Schedule_Data.hpp"
#include "Driver_Data.hpp"
#include "Route_Nodes_Data.hpp"
#include "Selection_Data.hpp"
#include "Household_Data.hpp"
#include "Performance_Data.hpp"
#include "Turn_Delay_Data.hpp"
#include "Ridership_Data.hpp"
#include "Veh_Type_Data.hpp"
#include "Trip_Data.hpp"
#include "Problem_Data.hpp"
#include "Plan_Data.hpp"
#include "Skim_Data.hpp"
#include "Event_Data.hpp"
#include "Traveler_Data.hpp"
#include "Matrix_Data.hpp"
#include "Lane_Map_Data.hpp"

#include "Trip_Index.hpp"
#include "Time_Index.hpp"
#include "Vehicle_Index.hpp"

#include "Link_Data_File.hpp"
#include "Link_Dir_File.hpp"
#include "Volume_Array.hpp"

//---------------------------------------------------------
//	Data_Service - system data class definition
//---------------------------------------------------------

class SYSLIB_API Data_Service : public File_Service
{
	friend class Draw_Service;

public:

	Data_Service (void);

	//---- data arrays ----

	Node_Array          node_array;
	Zone_Array          zone_array;
	Shape_Array         shape_array;
	Link_Array          link_array;
	Dir_Array           dir_array;
	Pocket_Array        pocket_array;
	Lane_Use_Array      lane_use_array;
	Use_Period_Array    use_period_array;
	Link_Dir_Array		use_period_index;
	Connect_Array       connect_array;
	Turn_Pen_Array      turn_pen_array;
	Parking_Array       parking_array;
	Location_Array      location_array;
	Access_Array        access_array;
	Sign_Array          sign_array;
	Signal_Array        signal_array;
	Detector_Array      detector_array;
	Stop_Array          stop_array;
	Line_Array          line_array;
	Route_Nodes_Array   route_nodes_array;
	Household_Array     hhold_array;
	Perf_Period_Array   perf_period_array;
	Turn_Period_Array   turn_period_array;
	Veh_Type_Array      veh_type_array;
	Trip_Array          trip_array;
	Problem_Array       problem_array;
	Plan_Array          plan_array;
	Event_Array         event_array;
	Traveler_Array		traveler_array;

	Integers            lane_use_flow_index;

	//---- data indices ----

	Int_Map       node_map;
	Int_Map       zone_map;
	Int_Map       shape_map;
	Int_Map       link_map;
	Int_Map       dir_map;
	Int2_Map      ab_map;
	Int_Map       parking_map;
	Int_Map       location_map;
	Int_Map       access_map;
	Int2_Map      connect_map;
	Int_Map       signal_map;
	Int_Map       detector_map;
	Int_Map       stop_map;
	Int_Map       line_map;
	Fare_Map      fare_map;
	Int_Map       route_map;
	Select_Map    select_map;
	Int_Map       hhold_map;
	Person_Map    person_map;
	Int_Map       veh_type_map;
	Trip_Map      trip_map;
	Trip_Map      plan_trip_map;
	Time_Map      plan_time_map;
	Event_Map     event_map;
	Traveler_Map  traveler_map;
	Vehicle_Map   vehicle_map;

	Timing40_Map  timing40_map;
	Int2_Map      vehtype40_map;
	
	Int_Map route_mode_map;
	int mode_type_map [ANY_TRANSIT];

	int  Num_Fare_Zones (void)          { return (num_fare_zone); }

	int  Max_Zone_Number (void)         { return (max_zone); }
	void Max_Zone_Number (int value)    { max_zone = value; }

	int  Bearing_Offset (void)          { return (bearing_offset); }
	void Update_Bearings (bool flag)    { update_bearings = flag; }
	bool Update_Bearings (void)         { return (update_bearings); }
	void Bearing_Warnings (bool flag)   { bearing_warnings = flag; }
	bool Bearing_Warnings (void)        { return (bearing_warnings); }

	Compass_Points compass;
	Time_Periods sum_periods;

	bool Time_Table_Flag (void)         { return (time_table_flag); }
	void Time_Table_Flag (bool flag)    { time_table_flag = flag; }
	
	bool Ratio_Flag (void)              { return (ratio_flag); }
	void Ratio_Flag (bool flag)         { ratio_flag = flag; }

	bool Sum_Flow_Flag (void)           { return (sum_flow_flag); }
	void Sum_Flow_Flag (bool flag)      { sum_flow_flag = flag; }

	bool Clear_Flow_Flag (void)         { return (clear_flow_flag); }
	void Clear_Flow_Flag (bool flag)    { clear_flow_flag = flag; }

	int  Num_Lane_Use_Flows (void)      { return (num_lane_flows); }
	void Num_Lane_Use_Flows (int n)     { num_lane_flows = n; }
	int  Add_Lane_Use_Flows (void)      { return (num_lane_flows++); }

	bool Location_XY_Flag (void)        { return (loc_xy_flag); }
	void Location_XY_Flag (bool flag)   { loc_xy_flag = flag; }

	bool Transit_Veh_Flag (void)        { return (transit_veh_flag); }
	void Transit_Veh_Flag (bool flag)   { transit_veh_flag = flag; }

	int  Congested_Ratio (void)         { return (congested_ratio); }
	void Congested_Ratio (int value)    { congested_ratio = value; }
	
	int  Maximum_Time_Ratio (void)      { return (maximum_ratio); }
	void Maximum_Time_Ratio (int value) { maximum_ratio = value; }

	Trip_Sort_Type Trip_Sort (void)     { return (trip_sort); }
	void Trip_Sort (Trip_Sort_Type t)   { trip_sort = t; }

	bool Time_Sort_Flag (void)          { return (time_sort_flag); }
	void Time_Sort_Flag (bool flag)     { time_sort_flag = flag; }

	bool Trip_Sort_Flag (void)          { return (trip_sort_flag); }
	void Trip_Sort_Flag (bool flag)     { trip_sort_flag = flag; }
	
	bool Person_Map_Flag (void)         { return (person_map_flag); }
	void Person_Map_Flag (bool flag)    { person_map_flag = flag; }

	double turn_shape_setback;

	//---- write methods ----
	
	virtual void Write_Nodes (void);
	virtual void Write_Zones (void);
	virtual int  Write_Shapes (void);
	virtual void Write_Links (void);
	virtual void Write_Pockets (void);
	virtual void Write_Lane_Uses (void);
	virtual void Write_Connections (void);
	virtual void Write_Turn_Pens (void);
	virtual void Write_Parking_Lots (void);
	virtual void Write_Locations (void);
	virtual void Write_Access_Links (void);
	virtual void Write_Signs (void);
	virtual void Write_Signals (void);
	virtual void Write_Timing_Plans (void);
	virtual void Write_Phasing_Plans (void);
	virtual void Write_Detectors (void);
	virtual void Write_Stops (void);
	virtual void Write_Fares (void);
	virtual void Write_Lines (void);
	virtual void Write_Schedules (void);
	virtual void Write_Drivers (void);
	virtual void Write_Route_Nodes (void);
	virtual void Write_Selections (void);
	virtual void Write_Households (void);

	virtual void Write_Performance (void);
	virtual void Write_Performance (Performance_File &file, Perf_Period_Array &data);
	virtual void Write_Performance (Vol_Spd_Period_Array &data);

	virtual void Write_Turn_Delays (void);
	virtual void Write_Turn_Delays (Turn_Delay_File &file, Turn_Period_Array &data);

	virtual void Write_Ridership (void);
	virtual void Write_Veh_Types (void);
	virtual void Write_Trips (void);
	virtual void Write_Problems (void);
	virtual void Write_Plans (void);
	virtual void Write_Skims (int period = -1);
	virtual void Write_Events (void);
	virtual void Write_Travelers (void);

	virtual void Write_Link_Data (Link_Data_File &file, Volume_Array &data, bool fill_flag = false);
	virtual void Write_Link_Data (Link_Data_File &file, Vol_Spd_Period_Array &data, bool vol_flag = true, bool fill_flag = false);

	//---- data processing methods ----

	virtual int Put_Node_Data (Node_File &file, Node_Data &data);
	virtual int Put_Zone_Data (Zone_File &file, Zone_Data &data);
	virtual int Put_Shape_Data (Shape_File &file, Shape_Data &data);
	virtual int Put_Link_Data (Link_File &file, Link_Data &data);
	virtual int Put_Pocket_Data (Pocket_File &file, Dir_Data &data);
	virtual int Put_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &data);
	virtual int Put_Connect_Data (Connect_File &file, Connect_Data &data);
	virtual int Put_Turn_Pen_Data (Turn_Pen_File &file, Dir_Data &data);
	virtual int Put_Parking_Data (Parking_File &file, Parking_Data &data);
	virtual int Put_Location_Data (Location_File &file, Location_Data &data);
	virtual int Put_Access_Data (Access_File &file, Access_Data &data);
	virtual int Put_Sign_Data (Sign_File &file, Dir_Data &data);
	virtual int Put_Signal_Data (Signal_File &file, Signal_Data &data);
	virtual int Put_Timing_Data (Timing_File &file, Signal_Data &data);
	virtual int Put_Phasing_Data (Phasing_File &file, Signal_Data &data);
	virtual int Put_Detector_Data (Detector_File &file, Detector_Data &data);
	virtual int Put_Stop_Data (Stop_File &file, Stop_Data &data);
	//virtual int Put_Fare_Data (Fare_File &file, Fare_Data &data);
	virtual int Put_Line_Data (Line_File &file, Line_Data &data);
	virtual int Put_Schedule_Data (Schedule_File &file, Line_Data &data);
	virtual int Put_Driver_Data (Driver_File &file, Line_Data &data);
	virtual int Put_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data);
	virtual int Put_Selection_Data (Selection_File &file, Trip_Index &index, Select_Data &data);
	virtual int Put_Household_Data (Household_File &file, Household_Data &data);
	virtual int Put_Performance_Data (Performance_File &file, Performance_Data &data);
	virtual int Put_Turn_Delay_Data (Turn_Delay_File &file, Turn_Delay_Data &data);
	virtual int Put_Ridership_Data (Ridership_File &file, Line_Data &data);
	virtual int Put_Veh_Type_Data (Veh_Type_File &file, Veh_Type_Data &data);
	virtual int Put_Trip_Data (Trip_File &file, Trip_Data &data);
	virtual int Put_Problem_Data (Problem_File &file, Problem_Data &data);
	virtual int Put_Plan_Data (Plan_File &file, Plan_Data &data);
	virtual int Put_Skim_Data (Skim_File &file, Skim_Record &data);
	virtual int Put_Event_Data (Event_File &file, Event_Data &data);
	virtual int Put_Traveler_Data (Traveler_File &file, Traveler_Data &data);

	enum Data_Service_Keys { 
		DAILY_WRAP_FLAG = DATA_SERVICE_OFFSET, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 
		PERIOD_CONTROL_POINT, CONGESTED_TIME_RATIO, MAXIMUM_TIME_RATIO, 
		TRIP_SORT_TYPE, PLAN_SORT_TYPE, HIGHEST_ZONE_NUMBER, UPDATE_LINK_BEARINGS, LINK_BEARING_WARNINGS,
		ROUTE_MODE_MAP, MODE_VEH_TYPE_MAP, 
	};
	void Data_Service_Keys (int *keys = 0);

protected:

	virtual void Program_Control (void);
	virtual void Execute (void);

	//---- data processing methods ----

	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);
	virtual bool Get_Zone_Data (Zone_File &file, Zone_Data &data);
	virtual bool Get_Shape_Data (Shape_File &file, Shape_Data &data);
	virtual bool Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba);
	virtual bool Get_Pocket_Data (Pocket_File &file, Pocket_Data &data);
	virtual bool Get_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &data);
	virtual bool Get_Connect_Data (Connect_File &file, Connect_Data &data);
	virtual bool Get_Turn_Pen_Data (Turn_Pen_File &file, Turn_Pen_Data &data);
	virtual bool Get_Parking_Data (Parking_File &file, Parking_Data &data);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Access_Data (Access_File &file, Access_Data &data);
	virtual bool Get_Sign_Data (Sign_File &file, Sign_Data &data);
	virtual bool Get_Signal_Data (Signal_File &file, Signal_Data &data);
	virtual bool Get_Timing_Data (Timing_File &file, Timing_Record &data);
	virtual bool Get_Phasing_Data (Phasing_File &file, Phasing_Record &data);
	virtual bool Get_Detector_Data (Detector_File &file, Detector_Data &data);
	virtual bool Get_Stop_Data (Stop_File &file, Stop_Data &data);
	virtual bool Get_Fare_Data (Fare_File &file, Fare_Data &data);
	virtual bool Get_Line_Data (Line_File &file, Line_Data &data);
	virtual bool Get_Schedule_Data (Schedule_File &file, Schedule_Data &data);
	virtual bool Get_Driver_Data (Driver_File &file, Driver_Data &data);
	virtual bool Get_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data);
	virtual bool Get_Selection_Data (Selection_File &file, Selection_Data &data, int partition = 0);
	virtual bool Get_Household_Data (Household_File &file, Household_Data &data, int partition = 0);
	virtual bool Get_Performance_Data (Performance_File &file, Performance_Data &data);
	virtual bool Get_Turn_Delay_Data (Turn_Delay_File &file, Turn_Delay_Data &data);
	virtual bool Get_Ridership_Data (Ridership_File &file, Ridership_Data &data);
	virtual bool Get_Veh_Type_Data (Veh_Type_File &file, Veh_Type_Data &data);
	virtual bool Get_Trip_Data (Trip_File &file, Trip_Data &data, int partition = 0);
	virtual bool Get_Problem_Data (Problem_File &file, Problem_Data &data, int partition = 0);
	virtual bool Get_Plan_Data (Plan_File &file, Plan_Data &data, int partition = 0);
	virtual bool Get_Skim_Data (Skim_File &file, Skim_Record &data, int partition = 0);
	virtual bool Get_Event_Data (Event_File &file, Event_Data &data);
	virtual bool Get_Traveler_Data (Traveler_File &file, Traveler_Data &data);

	//---- data initialization methods ----

	virtual void Initialize_Nodes (Node_File &file);
	virtual void Initialize_Zones (Zone_File &file);
	virtual void Initialize_Shapes (Shape_File &file);
	virtual void Initialize_Links (Link_File &file);
	virtual void Initialize_Pockets (Pocket_File &file);
	virtual void Initialize_Lane_Uses (Lane_Use_File &file);
	virtual void Initialize_Connects (Connect_File &file);
	virtual void Initialize_Turn_Pens (Turn_Pen_File &file);
	virtual void Initialize_Parking_Lots (Parking_File &file);
	virtual void Initialize_Locations (Location_File &file);
	virtual void Initialize_Access (Access_File &file);
	virtual void Initialize_Signs (Sign_File &file);
	virtual void Initialize_Signals (Signal_File &file);
	virtual void Initialize_Timing_Plans (Timing_File &file);
	virtual void Initialize_Phasing_Plans (Phasing_File &file);
	virtual void Initialize_Detectors (Detector_File &file);
	virtual void Initialize_Stops (Stop_File &file);
	virtual void Initialize_Fares (Fare_File &file);
	virtual void Initialize_Lines (Line_File &file);
	virtual void Initialize_Schedules (Schedule_File &file);
	virtual void Initialize_Drivers (Driver_File &file);
	virtual void Initialize_Route_Nodes (Route_Nodes_File &file);
	virtual void Initialize_Selections (Selection_File &file);
	virtual void Initialize_Households (Household_File &file);
	virtual void Initialize_Performance (Performance_File &file, Perf_Period_Array &data);
	virtual void Initialize_Turn_Delays (Turn_Delay_File &file, Turn_Period_Array &data);
	virtual void Initialize_Ridership (Ridership_File &file);
	virtual void Initialize_Veh_Types (Veh_Type_File &file);
	virtual void Initialize_Trips (Trip_File &file);
	virtual void Initialize_Problems (Problem_File &file);
	virtual void Initialize_Plans (Plan_File &file);
	virtual void Initialize_Skims (Skim_File &file);
	virtual void Initialize_Events (Event_File &file);
	virtual void Initialize_Travelers (Traveler_File &file);

	Link_Data * Set_Link_Direction (Db_Header &file, int &link, int &dir, int &offset, bool flag = false);
	Link_Data * Set_Link_Direction (Db_Header &file, int &link, int &dir) 
					{ int offset = 0; return (Set_Link_Direction (file, link, dir, offset)); }
	Link_Data * Set_Link_Direction (Db_Header &file, int &link, int &dir, bool flag) 
					{ int offset = 0; return (Set_Link_Direction (file, link, dir, offset, flag)); }

	int  Convert_Lane_ID (Dir_Data *dir_ptr, int lane_id);
	int  Convert_Lane_ID (int dir_index, int lane_id)       { return (Convert_Lane_ID (&dir_array [dir_index], lane_id)); }

	int  Make_Lane_ID (Dir_Data *dir_ptr, int lane);
	int  Make_Lane_ID (int dir_index, int lane)             { return (Make_Lane_ID (&dir_array [dir_index], lane)); }

	void Convert_Lane_Range (Dir_Data *dir_ptr, int range, int &low, int &high);
	void Convert_Lane_Range (int dir_index, int range, int &low, int &high) 
	                                                        { Convert_Lane_Range (&dir_array [dir_index], range, low, high); }

	int  Make_Lane_Range (int dir_index, int low, int high) { return (Make_Lane_Range (&dir_array [dir_index], low, high)); }
	int  Make_Lane_Range (Dir_Data *dir_ptr, int low, int high); 

	void Lane_Map (Connect_Data *connect_ptr, Lane_Map_Array &lane_map);

	int  Fix_Lane_ID (Dir_Data *dir_ptr, int lane_id);
	int  Fix_Lane_ID (int dir_index, int lane_id)           { return (Fix_Lane_ID (&dir_array [dir_index], lane_id)); }

	void Fix_Lane_Range (Dir_Data *dir_ptr, int range, int &low, int &high);
	void Fix_Lane_Range (int dir_index, int range, int &low, int &high) 
	                                                        { Fix_Lane_Range (&dir_array [dir_index], range, low, high); }

	bool Link_Shape (Link_Data *link_ptr, int dir, Points &points, double offset = -1.0, double length = -1.0, double side = 0.0);
	bool Turn_Shape (int dir_in, int dir_out, Points &points, double setback = 0.0, bool curve_flag = false, double side_in = 0.0, double side_out = 0.0, double offset = -1.0, double length = -1.0);

	void Bearing_Offset (int value)     { bearing_offset = value; }
	void Link_Bearings (Link_Data &link_rec, int &bearing_in, int &bearing_out);

	int  VehType40_Map (int type, int sub);

	//---- read methods ----

	virtual void Read_Nodes (Node_File &file);
	virtual void Read_Zones (Zone_File &file);
	virtual void Read_Shapes (Shape_File &file);
	virtual void Read_Links (Link_File &file);
	virtual void Read_Pockets (Pocket_File &file);
	virtual void Read_Lane_Uses (Lane_Use_File &file);
	virtual void Read_Connections (Connect_File &file);
	virtual void Read_Turn_Pens (Turn_Pen_File &file);
	virtual void Read_Parking_Lots (Parking_File &file);
	virtual void Read_Locations (Location_File &file);
	virtual void Read_Access_Links (Access_File &file);
	virtual void Read_Signs (Sign_File &file);
	virtual void Read_Signals (Signal_File &file);
	virtual void Read_Timing_Plans (Timing_File &file);
	virtual void Read_Phasing_Plans (Phasing_File &file);
	virtual void Read_Detectors (Detector_File &file);
	virtual void Read_Stops (Stop_File &file);
	virtual void Read_Fares (Fare_File &file);
	virtual void Read_Lines (Line_File &file);
	virtual void Read_Schedules (Schedule_File &file);
	virtual void Read_Drivers (Driver_File &file);
	virtual void Read_Route_Nodes (Route_Nodes_File &file);
	virtual void Read_Selections (Selection_File &file);
	virtual void Read_Households (Household_File &file);
	virtual void Read_Performance (Performance_File &file, Perf_Period_Array &data);
	virtual void Read_Turn_Delays (Turn_Delay_File &file, Turn_Period_Array &data);
	virtual void Read_Ridership (Ridership_File &file);
	virtual void Read_Veh_Types (Veh_Type_File &file);
	virtual void Read_Trips (Trip_File &file);
	virtual void Read_Problems (Problem_File &file);
	virtual void Read_Plans (Plan_File &file);
	virtual void Read_Skims (Skim_File &file);
	virtual void Read_Events (Event_File &file);
	virtual void Read_Travelers (Traveler_File &file);

	virtual void Read_Link_Data (Link_Data_File &file, Volume_Array &data);
	virtual void Read_Performance (Performance_File &file, Volume_Array &data, bool entry_flag = false);
	virtual void Read_Link_Direction (Link_Direction_File &file, Volume_Array &data, bool base_flag = true);

private:

	bool ratio_flag, loc_xy_flag, sum_flow_flag, clear_flow_flag, time_table_flag, transit_veh_flag;
	bool person_map_flag, lane_use_flows, update_bearings, bearing_warnings, trip_sort_flag, time_sort_flag;
	int bearing_offset, num_fare_zone, congested_ratio, maximum_ratio, max_zone, num_lane_flows;

	Trip_Sort_Type trip_sort;
};

extern SYSLIB_API Data_Service *dat;

#ifdef HASH_MAP
SYSLIB_API size_t hash_value (const Vehicle_Index & value);
#endif

#endif
