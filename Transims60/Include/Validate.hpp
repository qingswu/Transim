//*********************************************************
//	Validate.hpp - Network Validation Utility
//*********************************************************

#ifndef VALIDATE_HPP
#define VALIDATE_HPP

#include "Data_Service.hpp"
#include "Db_Header.hpp"
#include "Link_Data_File.hpp"
#include "Link_Dir_File.hpp"
#include "Volume_Array.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Validate - execution class definition
//---------------------------------------------------------

class SYSLIB_API Validate : public Data_Service
{
public:
	Validate (void);

	virtual void Execute (void);

protected:
	enum Validate_Keys { 
		INPUT_VOLUME_FILE = 1, TRAFFIC_COUNT_FILE, TURN_VOLUME_FILE, TURN_COUNT_FILE, LINE_GROUP_COUNT_FILE, 
		STOP_GROUP_COUNT_FILE, NEW_VOLUME_FILE, NEW_VOLUME_COUNT_FILE, ANALYSIS_METHOD, ENTRY_DATA_FLAG, 
		ADJUSTMENT_FACTOR, FACILITY_TYPE_LABELS, AREA_TYPE_LABELS, 
		CHECK_NETWORK_FLAG, NEW_PROBLEM_NODE_FILE, NEW_PROBLEM_LINK_FILE, NEW_PROBLEM_COORDINATE_FILE,
	};
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Validate_Reports {VOLUME_LEVEL = 1, FACILITY_TYPE, AREA_TYPE, ZONE_GROUP, ZONE_AT_FT,
		LINK_GROUP, GROUP_DETAILS, TURN_MOVEMENT, TURN_LOS, ZONE_EQUIV, LINK_EQUIV,
		LINE_GROUP, STOP_GROUP, BOARD_GROUP, ALIGHT_GROUP, LINE_EQUIV, STOP_EQUIV};

	bool delay_flag, turn_delay_flag, check_net_flag, problem_node_flag, problem_link_flag, problem_coord_flag;
	bool link_flag, turn_flag, output_flag, zone_flag, method, entry_flag, line_flag, stop_flag, vc_flag, cnt_dir_flag;
	double factor, hours;

	Db_Header zone_file, line_count_file, stop_count_file;
	Db_File problem_node_file, problem_link_file, problem_coord_file;

	Link_Data_File count_file, volume_file,  output_file;
	Link_Direction_File vol_cnt_file, cnt_dir_file;
	Turn_Delay_File turn_count, turn_volume;
	
	Volume_Array  link_count_array, link_volume_array;
	Turn_Period_Array  turn_count_array;

	String stats_format;
	String header1, header2;

	static int volume_level [];
	String group_label;

	//---- facility type and area type labels ----

	Str_Map facility_type, area_type;

	//---- evaluation statistics ----

	typedef struct {
		double per_diff;
		double avg_error;
		double per_error;
		double std_dev;
		double r_sq;
		double rmse;
		//double correl;
		double avg_vc;
	} Statistics; 

	typedef struct {
		int    index;
		int    number;
		double count;
		double volume;
		double count_sq;
		double volume_sq;
		double count_volume;
		double error;
		double error_sq;
		double capacity;
		double max_vc;
	} Group_Data; 

	typedef map <int, Group_Data>        Group_Map;
	typedef pair <int, Group_Data>       Group_Map_Data;
	typedef Group_Map::iterator          Group_Map_Itr;
	typedef pair <Group_Map_Itr, bool>   Group_Map_Stat;

	Group_Data total;
	Group_Map group_data;
	Integers node_zone;

	//---- methods ----

	void Check_Network (void);

	void Write_Volume (void);
	void Write_Volume_Count (void);
    int  Closest_Zone (int x, int y);

	void Volume_Stats (void);
	void Volume_Header (void);

	void Facility_Stats (void);
	void Facility_Header (void);

	void Area_Stats (void);
	void Area_Header (void);

	void Zone_Group_Stats (void);
	void Zone_Group_Header (void);

	void Link_Group_Stats (void);
	void Link_Group_Header (void);

	void Link_Group_Details (void);
	void Group_Detail_Header (void);

	void Turn_Stats (void);
	void Turn_Header (void);

	void Turn_LOS (void);
	void Turn_LOS_Header (void);

	void Calc_Stats (Group_Data *data, Statistics &stats);
	void Sum_Group (int group, Link_Data *link, int dir);
	void Subtotal (Group_Data &subtot, Group_Data &data);

	void Line_Group_Stats (void);
	void Line_Group_Header (void);

	void Stop_Group_Stats (int type);
	void Stop_Group_Header (int type);

	void Zone_AT_FT_Stats (void);
	void Zone_AT_FT_Header (void);

};

#endif
