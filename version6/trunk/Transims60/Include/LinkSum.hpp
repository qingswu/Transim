//*********************************************************
//	LinkSum.hpp - Summarize Link Performance Data
//*********************************************************

#ifndef LINKSUM_HPP
#define LINKSUM_HPP

#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Link_Data_File.hpp"
#include "Link_Dir_File.hpp"
#include "Arcview_File.hpp"
#include "Db_Header.hpp"
#include "Db_Array.hpp"
#include "TypeDefs.hpp"
#include "Data_Range.hpp"

#define NUM_SUM_BINS	1000
#define NUM_PERCENTILES	9
#define RESOLUTION		100

//---------------------------------------------------------
//	LinkSum - execution class definition
//---------------------------------------------------------

class SYSLIB_API LinkSum : public Data_Service, public Select_Service
{
public:
	LinkSum (void);

	virtual void Execute (void);

	virtual void Initialize_Locations (Location_File &file);
	virtual bool Get_Location_Data (Location_File &file, Location_Data &data);
	virtual bool Get_Performance_Data (Performance_File &file, Performance_Data &data);

protected:
	enum LinkSum_Keys { 
<<<<<<< .working
		COMPARE_PERFORMANCE_FILE = 1, COMPARE_PERFORMANCE_FORMAT, 
		MINIMUM_LINK_VOLUME, PERSON_BASED_STATISTICS, SELECT_BY_LINK_GROUP,  
		COMPARE_TURN_DELAY_FILE, COMPARE_TURN_DELAY_FORMAT, TURN_NODE_RANGE,
		NEW_LINK_ACTIVITY_FILE, NEW_LINK_ACTIVITY_FORMAT, COPY_LOCATION_FIELDS, 
		NEW_ZONE_TRAVEL_FILE, NEW_ZONE_TRAVEL_FORMAT, 
=======
		COMPARE_PERFORMANCE_FILE = 1, COMPARE_PERFORMANCE_FORMAT, 
		COMPARE_LINK_MAP_FILE, COMPARE_LINK_MAP_FORMAT, 
		MINIMUM_LINK_VOLUME, PERSON_BASED_STATISTICS, SELECT_BY_LINK_GROUP,  
		COMPARE_TURN_DELAY_FILE, COMPARE_TURN_DELAY_FORMAT, TURN_NODE_RANGE,
		NEW_LINK_ACTIVITY_FILE, NEW_LINK_ACTIVITY_FORMAT, COPY_LOCATION_FIELDS, 
		NEW_ZONE_TRAVEL_FILE, NEW_ZONE_TRAVEL_FORMAT, 
>>>>>>> .merge-right.r1529
		NEW_GROUP_TRAVEL_FILE, NEW_GROUP_TRAVEL_FORMAT, 
<<<<<<< .working
		NEW_LINK_DIRECTION_FILE, NEW_LINK_DIRECTION_FORMAT, NEW_LINK_DIRECTION_FIELD, NEW_LINK_DIRECTION_INDEX, 
		NEW_LINK_DATA_FILE, NEW_LINK_DATA_FORMAT, NEW_LINK_DATA_FIELD, 
		NEW_DATA_SUMMARY_FILE, NEW_DATA_SUMMARY_FORMAT, NEW_DATA_SUMMARY_PERIODS, NEW_DATA_SUMMARY_RATIOS,
=======
		NEW_LINK_DIRECTION_FILE, NEW_LINK_DIRECTION_FORMAT, NEW_LINK_DIRECTION_FIELD, NEW_LINK_DIRECTION_INDEX, 
		NEW_LINK_DIRECTION_FLIP, NEW_LINK_DATA_FILE, NEW_LINK_DATA_FORMAT, NEW_LINK_DATA_FIELD, 
		NEW_DATA_SUMMARY_FILE, NEW_DATA_SUMMARY_FORMAT, NEW_DATA_SUMMARY_PERIODS, NEW_DATA_SUMMARY_RATIOS,
>>>>>>> .merge-right.r1529
		NEW_GROUP_SUMMARY_FILE, NEW_GROUP_SUMMARY_FORMAT, 
	};
	virtual void Program_Control (void);

	virtual void Page_Header (void);

private:
	enum LinkSum_Reports { TOP_LINK_VOL = 1, TOP_LANE_VOL, TOP_PERIOD, TOP_SPEED, 
		TOP_TIME_RATIO, TOP_VC_RATIO, TOP_TIME_CHANGE, TOP_VOL_CHANGE, 
		LINK_REPORT, LINK_GROUP, LINK_EQUIV, ZONE_EQUIV,
		TIME_DISTRIB, VC_RATIOS, TIME_CHANGE, VOLUME_CHANGE, TRAVEL_TIME,
		PERF_REPORT, PERF_SUMMARY, GROUP_REPORT, GROUP_SUMMARY, RELATIVE_GAP };

<<<<<<< .working
	bool select_flag, activity_flag, zone_flag, group_flag, person_flag, periods_flag, ratios_flag;
	bool compare_flag, group_select, summary_flag, group_sum_flag, turn_flag, turn_compare_flag;
=======
	bool select_flag, activity_flag, zone_flag, group_flag, person_flag, periods_flag, ratios_flag, compare_perf_flag;
	bool compare_flag, group_select, summary_flag, group_sum_flag, turn_flag, turn_compare_flag, link_map_flag;
>>>>>>> .merge-right.r1529
	int increment, num_inc, nerror;
	double header_value, cap_factor, minimum_volume, congested_ratio;
	String header_label;
	
	typedef vector <Doubles>           Doubles_Array;
	typedef Doubles_Array::iterator    Doubles_Itr;

	Doubles_Array sum_bin, zone_vmt, zone_vht;
	Doubles data_ratios;
	Time_Periods data_periods;

	Performance_File compare_file;
	Turn_Delay_File turn_compare_file;

	Data_Range turn_range;

	Perf_Period_Array compare_perf_array;
	Turn_Period_Array compare_turn_array;

	Db_Sort_Array link_db;
	Db_Header activity_file, zone_file, group_file, summary_file, group_sum_file, link_map_file;
	Integers field_list;

	Int_Map zone_list;
	Ints_Map compare_link_map;

	typedef struct {
		int group;
		int field;
		Link_Data_File *file;
	} Data_Group;

	typedef vector <Data_Group>        Data_Group_Array;
	typedef Data_Group_Array::iterator Data_Group_Itr;

	Data_Group_Array data_group;
	
	typedef struct {
		int group;
		int field;
		Link_Direction_File *file;
		bool index;
		bool flip;
	} Dir_Group;

	typedef vector <Dir_Group>         Dir_Group_Array;
	typedef Dir_Group_Array::iterator  Dir_Group_Itr;

	Dir_Group_Array dir_group;

	typedef struct {
		int link;
		int location;
		int zone;
		int count;
	} Link_Location;

	typedef map <Int2_Key, Link_Location>  Link_Loc_Map;
	typedef pair <Int2_Key, Link_Location> Link_Loc_Map_Data;
	typedef Link_Loc_Map::iterator         Link_Loc_Map_Itr;
	typedef pair <Link_Loc_Map_Itr, bool>  Link_Loc_Map_Stat;

	Link_Loc_Map link_location;

	static int percent_break [NUM_PERCENTILES];

	//---- methods ----

	void Compare_Links (void);
	void Set_Link_Dir (Dir_Group *group);
	void Set_Link_Data (Data_Group *group);
	void Select_Links (void);
	void Error_Check (void);
	void Write_Activity (void);
	void Zone_Summary (void);
	void Write_Zone (void);
	void Write_Group (void);

	void Write_Link_Data (Data_Group_Itr &group);
	void Write_Link_Dir (Dir_Group_Itr &group);
	void Write_Turn_Data (void);

	void Top_100_Report (int number);
	void Top_100_Link_Header (void);
	void Top_100_Lane_Header (void);
	void Top_100_Time_Header (void);

	void Top_100_Ratios (int number);
	void Top_100_Speed_Header (void);
	void Top_100_Travel_Time_Header (void);
	void Top_100_VC_Header (void);
	void Top_100_Time_Change_Header (void);
	void Top_100_Volume_Change_Header (void);

	void Link_Report (double limit);
	void Link_Report_Header (void);

	void Link_Group (double limit);
	void Link_Group_Header (void);

	void Travel_Time_Report (void);
	void Travel_Time_Header (void);

	void Volume_Capacity_Report (void);
	void Volume_Capacity_Header (void);

	void Travel_Time_Change (void);
	void Time_Change_Header (void);

	void Volume_Change (void);
	void Volume_Change_Header (void);

	void Group_Time_Report (void);
	void Group_Time_Header (void);

	void Performance_Report (void);
	void Performance_Header (void);

	void Perf_Sum_Report (void);
	void Perf_Sum_Header (void);

	void Group_Report (void);
	void Group_Header (void);

	void Group_Sum_Report (void);
	void Group_Sum_Header (void);

	void Relative_Gap_Report (void);
	void Relative_Gap_Header (void);

	void Summary_File (void);
	void Custom_Summaries (void);
	void Group_Sum_File (void);
};
#endif
