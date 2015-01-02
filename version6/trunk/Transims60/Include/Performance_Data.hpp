//*********************************************************
//	Performance_Data.hpp - network performance data
//*********************************************************

#ifndef PERFORMANCE_DATA_HPP
#define PERFORMANCE_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Dir_Data.hpp"
#include "Link_Data.hpp"
#include "Data_Pack.hpp"
#include "Time_Periods.hpp"
#include "Dtime.hpp"
#include "Volume_Array.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Perf_Data class definition
//---------------------------------------------------------

class SYSLIB_API Perf_Data
{
public:
	Perf_Data (void)                       { Clear (); }

	Dtime  Time (void)                     { return (time); }
	double Persons (void)                  { return (persons); }
	double Volume (void)                   { return (volume); }
	double Enter (void)                    { return (enter); }
	double Exit (void)                     { return (exit); }
	double Max_Volume (void)               { return (max_volume); }
	double Queue (void)                    { return (queue); }
	double Max_Queue (void)                { return (max_queue); }
	double Failure (void)                  { return (failure); }
	double Veh_Dist (void)                 { return (veh_dist); }
	double Veh_Time (void)                 { return (veh_time); }
	double Occupancy (void)                { return (value1); }
	double Stop_Count (void)               { return (value2); }
	double Ratio_Dist (void)               { return (value1); }
	double Ratio_Time (void)               { return (value2); } 
	int    Ratios (void)                   { return (count1); }
	int    Count (void)                    { return (count2); }


	void   Time (Dtime value)              { time = value; }
	void   Persons (double value)          { persons = (float) value; }
	void   Volume (double value)           { volume = (float) value; }
	void   Enter (double value)            { enter = (float) value; }
	void   Exit (double value)             { exit = (float) value; }
	void   Max_Volume (double value)       { max_volume = (float) value; }
	void   Queue (double value)            { queue = (float) value; }
	void   Max_Queue (double value)        { max_queue = (float) value; }
	void   Failure (double value)          { failure = (float) value; }
	void   Veh_Dist (double value)         { veh_dist = (float) value; }
	void   Veh_Time (double value)         { veh_time = (float) value; }
	void   Occupancy (double value)        { value1 = (float) value; }
	void   Stop_Count (double value)       { value2 = (float) value; }
	void   Ratio_Dist (double value)       { value1 = (float) value; }
	void   Ratio_Time (double value)       { value2 = (float) value; }
	void   Ratios (int value)              { count1 = (short) value; }
	void   Count (int value)               { count2 = (short) value; }

	void   Time (int value)                { time = value; }
	void   Time (double value)             { time = exe->Round (value); }

	void   Add_Time (Dtime value)          { time = time + value; }
	void   Average_Time (Dtime value)      { time = (time + value) / 2; }
	void   Weight_Time (Dtime value, double weight)   { time = (int) ((time + value * weight) / (weight + 1) + 0.5); }
	void   Update_Time (Dtime value);

	void   Add_Persons (double value)      { persons = (float) (persons + value); }
	void   Add_Volume (double value)       { volume = (float) (volume + value); }
	void   Add_Enter (double value)        { enter = (float) (enter + value); }
	void   Add_Exit (double value)         { exit = (float) (exit + value); }
	void   Add_Max_Volume (double value)   { max_volume = (float) (max_volume + value); }
	void   Add_Queue (double value)        { queue = (float) (queue + value); }
	void   Add_Max_Queue (double value)    { max_queue = (float) (max_queue + value); }
	void   Add_Failure (double value)      { failure = (float) (failure + value); }
	void   Add_Veh_Dist (double value)     { veh_dist = (float) (veh_dist + value); }
	void   Add_Veh_Time (double value)     { veh_time = (float) (veh_time + value); }
	void   Add_Occupancy (double value)    { value1 = (float) (value1 + value); }
	void   Add_Stop_Count (double value)   { value2 = (float) (value2 + value); }
	void   Add_Ratio_Dist (double value)   { value1 = (float) (value1 + value); }
	void   Add_Ratio_Time (double value)   { value2 = (float) (value2 + value); }
	void   Add_Ratio (int value = 1)       { count1 = (short) (count1 + value); }
	void   Add_Count (int value = 1)       { count2 = (short) (count2 + value); }

	void   Sum_Max_Volume (double value)   { if (value > max_volume) max_volume = (float) value; }
	void   Sum_Max_Queue (double value)    { if (value > max_queue) max_queue = (float) value; }

	void   Clear (void)
	{
		time = 0; Clear_Flows (); 
	}
	void   Clear_Flows (void)
	{
		persons = volume = enter = exit = max_volume = queue = max_queue = failure = 0.0;
		veh_dist = veh_time = value1 = value2 = 0.0; count1 = count2 = 0; 
	}
	void   Set_Flows (Perf_Data *perf_ptr);
	void   Add_Flows (Perf_Data *perf_ptr);
	void   Average_Flows (Perf_Data *perf_ptr);
	void   Weight_Flows (Perf_Data *perf_ptr, double weight);
	void   Weight_Flows (Perf_Data *perf1_ptr, double fac1, Perf_Data *perf2_ptr, double fac2);
	void   Sum_Periods (Perf_Data *perf_ptr);
	bool   Output_Check (void);

private:
	Dtime  time;
	float  persons; 
	float  volume;
	float  enter;
	float  exit;
	float  max_volume;
	float  queue;
	float  max_queue;
	float  failure;
	float  veh_dist;
	float  veh_time;
	float  value1;
	float  value2;
	short  count1;
	short  count2;
};

//---------------------------------------------------------
//	Perf_Period class definition
//---------------------------------------------------------

class SYSLIB_API Perf_Period : public Vector <Perf_Data>
{
public:
	Perf_Period (void) { }
	
	Dtime  Time (int index)       { return (at (index).Time ()); }

	Perf_Data * Data_Ptr (int index) { return (&at (index)); }

	Perf_Data Total_Performance (int index);
	Perf_Data Total_Performance (int index, int use_index);
};
typedef Perf_Period::iterator   Perf_Itr;

//---------------------------------------------------------
//	Perf_Period_Array class definition
//---------------------------------------------------------

class SYSLIB_API Perf_Period_Array : public Vector <Perf_Period>
{
public:
	Perf_Period_Array (void) { periods = 0; num_records = 0; }

	void Initialize (Time_Periods *periods, int num_records = 0);
	
	void Replicate (Perf_Period_Array &period_array);

	void Set_Time0 (void);
	void Zero_Flows (Dtime first_time = 0);

	void Copy_Flow_Data (Perf_Period_Array &period_array, bool zero_flag = false, Dtime first_time = 0);
	void Copy_Time_Data (Perf_Period_Array &period_array, bool zero_flag = true, Dtime first_time = 0);

	void Add_Flows (Perf_Period_Array &period_array, bool zero_flag = false);
	void Add_Flow_Times (Perf_Period_Array &period_array, bool zero_flag = false);

	void Average_Flows (Perf_Period_Array &period_array, bool zero_flag = false);
	void Average_Flow_Times (Perf_Period_Array &period_array, int weight = 1, bool zero_flag = false);

	Dtime Travel_Time (int dir_index, Dtime time, double len_factor, bool forward_flag);

	Dtime Flow_Time (int dir_index, Dtime time, double len_factor, double len, double pce, double occ, bool forward_flag = true);
	Dtime Load_Flow (int dir_index, Dtime time, Dtime ttime, double len_factor, double len, double pce, double occ);

	Perf_Period * Period_Ptr (Dtime time);
	Perf_Period * Period_Ptr (int period)      { return (&at (period)); }

	Time_Periods * periods;
	int  Num_Records (void)  { return (num_records); }

private:
	int num_records;
};
typedef Perf_Period_Array::iterator  Perf_Period_Itr;

//---------------------------------------------------------
//	Performance_Data class definition
//---------------------------------------------------------

class SYSLIB_API Performance_Data
{
public:
	Performance_Data (void)           { Clear (); }

	int    Dir_Index (void)           { return (dir_index); }
	int    Type (void)                { return (type); }
	Dtime  Start (void)               { return (start); }
	Dtime  End (void)                 { return (end); }
	Dtime  Time (void)                { return (time); }
	double Persons (void)             { return (persons); }
	double Volume (void)              { return (volume); }
	double Enter (void)               { return (enter); }
	double Exit (void)                { return (exit); }
	double Flow (void)                { return (flow); }
	double Speed (void)               { return (speed); }
	double Time_Ratio (void)          { return (time_ratio); }
	Dtime  Delay (void)               { return (delay); }
	double Density (void)             { return (density); }
	double Max_Density (void)         { return (max_density); }
	double Queue (void)               { return (queue); }
	double Max_Queue (void)           { return (max_queue); }
	double Failure (void)             { return (failure); }
	double Veh_Dist (void)            { return (veh_dist); }
	double Veh_Time (void)            { return (veh_time); }
	double Veh_Delay (void)           { return (veh_delay); }
	double Lane_Len (void)            { return (lane_len); }
	double VC_Ratio (void)            { return (vc_ratio); }
	double Ratio_Dist (void)          { return (ratio_dist); }
	double Ratio_Time (void)          { return (ratio_time); } 
	int    Ratios (void)              { return (ratios); }
	int    Count (void)               { return (count); }


	void   Dir_Index (int value)      { dir_index = value; }
	void   Type (int value)           { type = value; }
	void   Start (Dtime value)        { start = value; }
	void   End (Dtime value)          { end = value; }
	void   Time (Dtime value)         { time = value; }
	void   Persons (double value)     { persons = (float) value; }
	void   Volume (double value)      { volume = (float) value; }
	void   Enter (double value)       { enter = (float) value; }
	void   Exit (double value)        { exit = (float) value; }
	void   Flow (double value)        { flow = (float) value; }
	void   Speed (double value)       { speed = (float) value; }
	void   Time_Ratio (double value)  { time_ratio = (float) value; }
	void   Delay (Dtime value)        { delay = value; }
	void   Density (double value)     { density = (float) value; }
	void   Max_Density (double value) { max_density = (float) value; }
	void   Queue (double value)       { queue = (float) value; }
	void   Max_Queue (double value)   { max_queue = (float) value; }
	void   Failure (double value)     { failure = (float) value; }
	void   Veh_Dist (double value)    { veh_dist = value; }
	void   Veh_Time (double value)    { veh_time = value; }
	void   Veh_Delay (double value)   { veh_delay = (float) value; }
	void   Lane_Len (double value)    { lane_len = (float) value; }
	void   VC_Ratio (double value)    { vc_ratio = (float) value; }
	void   Ratio_Dist (double value)  { ratio_dist = (float) value; }
	void   Ratio_Time (double value)  { ratio_time = (float) value; }
	void   Ratios (int value)         { ratios = value; }
	void   Count (int value)          { count = value; }

	void   Time (int value)           { time = value; }
	void   Time (double value)        { time = exe->Round (value); }	
	void   Delay (int value)          { delay = value; }
	void   Delay (double value)       { delay = exe->Round (value); }

	bool   Get_Data (Perf_Data *perf_ptr, int dir_index, int max_ratio = 2000000);
	bool   Get_Data (Perf_Data *perf_ptr, Dir_Data *dir_ptr, Link_Data *link_ptr, int max_ratio = 2000000);
	bool   Get_Data (Vol_Spd_Data *vol_spd_ptr, Dir_Data *dir_ptr, Link_Data *link_ptr, int max_ratio = 2000000);

	void   Clear (void)
	{
		dir_index = type = ratios = count = 0; start = end = time = delay = 0; 
		persons = volume = enter = exit = flow = density = max_density = queue = max_queue = failure = speed = time_ratio = 0.0;   
		veh_dist = veh_time = 0.0; veh_delay = lane_len = vc_ratio = ratio_dist = ratio_time = 0; 
	}
private:
	int    dir_index;
	int    type;
	Dtime  start;
	Dtime  end;
	Dtime  time;
	float  persons;
	float  volume;
	float  enter;
	float  exit;
	float  flow;
	float  speed;
	float  time_ratio;
	Dtime  delay;
	float  density;
	float  max_density;
	float  queue;
	float  max_queue;
	float  failure;
	float  veh_delay;
	double veh_dist;
	double veh_time;
	float  lane_len; 
	float  vc_ratio;
	float  ratio_dist;
	float  ratio_time;
	int    ratios;
	int    count;
};

typedef vector <Performance_Data>    Performance_Array;
typedef Performance_Array::iterator  Performance_Itr;

#endif
