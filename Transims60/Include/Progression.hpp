//*********************************************************
//	Progression.hpp - Signal Progression Utility
//*********************************************************

#ifndef PROGRESSION_HPP
#define PROGRESSION_HPP

#include "Data_Service.hpp"
#include "Time_Periods.hpp"
#include "Db_Array.hpp"
#include "TypeDefs.hpp"
#include "Arcview_File.hpp"
#include "Projection_Service.hpp"

//---------------------------------------------------------
//	Progression - execution class definition
//---------------------------------------------------------

class Progression : public Data_Service
{
public:

	Progression (void);
	virtual ~Progression (void);

	virtual void Execute (void);
	
	virtual bool Get_Connect_Data (Connect_File &file, Connect_Data &data);

protected:

	enum Progression_Keys { 
		CLEAR_EXISTING_OFFSETS = 1, EVALUATE_EXISTING_OFFSETS, PROGRESSION_TIME_PERIODS, 
		PROGRESSION_PERIOD_SPEED, OPTIMIZATION_METHOD, GROUP_PERIOD_WEIGHT_FILE, 
		KEEP_LINK_GROUP_ORDER, NEW_ARC_PROGRESSION_FILE, LINK_DIRECTION_OFFSET
	};	
	virtual void Program_Control (void);
	virtual void Page_Header (void);

private:
	enum Progression_Reports { LINK_EQUIV = 1, WEIGHT_REPORT };
	enum Optimization_Methods { PERCENT_THRU, MAX_SPEED, VEH_HOURS };

	int fixed, updated, progression_time, period, mid_period, method, tot_thru, max_period, old_thru;
	int period_fld, order_fld, group_fld, percent_fld, time_fld, length_fld, speed_fld, vht_fld;
	double progression_speed, link_offset;
	bool clear_flag, speed_flag, period_flag, arcview_flag, delay_flag, weight_flag, order_flag;
	bool eval_flag;

	Db_Data_Array signal_db;

	Arcview_File arcview_file;
	Points_Array points;

	Projection_Service projection;	
	Perf_Period *perf_period_ptr;

	Db_File weight_file;
	Dbls_Map weight_data;
	Int_Map thru_connect;

	//Signal_Offset_Array signal_offset;
	//Thru_Connect_Array thru_connect;
	
	Time_Periods progress_time;
	Double_List progress_speed;

	//---- group link ----
	
	typedef struct {
		int   node;
		int   signal;
		int   offset;
		int   green;
		int   ttime;
		int   length;
		int   vht;
	} Group_Link;

	typedef vector <Group_Link>         Group_Link_Array;
	typedef Group_Link_Array::iterator  Group_Link_Itr;
	typedef Group_Link_Array::pointer   Group_Link_Ptr;

	//---- group map ----

	typedef struct {
		int group;
		int next;
		Group_Link_Array link;
		Points_Array points;
	} Group_Data;

	typedef vector <Group_Data>           Group_Data_Array;
	typedef Group_Data_Array::iterator    Group_Data_Itr;
	typedef Group_Data_Array::pointer     Group_Data_Ptr;

	Group_Data_Array group_data_array;

	//---- green phase map ----

	typedef struct {
		int green;
		int offset;
		int movement;
	} Green_Phase;

	typedef map <Int2_Key, Green_Phase>       Green_Phase_Map;
	typedef pair <Int2_Key, Green_Phase>              Green_Phase_Map_Data;
	typedef Green_Phase_Map::iterator         Green_Phase_Map_Itr;
	typedef pair <Green_Phase_Map_Itr, bool>  Green_Phase_Map_Stat;

	//---- signal offset ----

	typedef struct {
		int period;
		int preset;
		int input;
		int offset;
		int cycle;
		Integer_List group;
		Green_Phase_Map phase;
	} Signal_Offset;

	typedef map <int, Signal_Offset>             Signal_Offset_Map;
	typedef pair <int, Signal_Offset>            Signal_Offset_Map_Data;
	typedef Signal_Offset_Map::iterator          Signal_Offset_Map_Itr;
	typedef pair <Signal_Offset_Map_Itr, bool>   Signal_Offset_Map_Stat;

	Signal_Offset_Map signal_offset_map;

	void Read_Weights (void);
	void Signal_Setup (void);
	void User_Groups (void);
	void Signal_Groups (void);
	int  Set_Offset (int group, bool report_flag = false, bool evaluate_flag = false);

	void Weight_Header (void);
};
#endif

