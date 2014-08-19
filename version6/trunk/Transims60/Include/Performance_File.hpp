//******************************************************** 
//	Performance_File.hpp - Link Performance Input/Output
//********************************************************

#ifndef PERFORMANCE_FILE_HPP
#define PERFORMANCE_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Performance_File Class definition
//---------------------------------------------------------

class SYSLIB_API Performance_File : public Db_Header
{
public:
	Performance_File (Access_Type access, string format, bool lane_use_flag = false);
	Performance_File (string filename, Access_Type access, string format, bool lane_use_flag = false);
	Performance_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool lane_use_flag = false);
	Performance_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, bool lane_use_flag = false);
	
	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Type (void)                { return (Get_Integer (type)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	double Time (void)                { return (Get_Double (time)); }
	double Persons (void)             { return (Get_Double (persons)); }
	double Volume (void)              { return (Get_Double (volume)); }
	double Enter (void)               { return (Get_Double (enter)); }
	double Exit (void)                { return (Get_Double (exit)); }
	double Flow (void)                { return (Get_Double (flow)); }
	double Speed (void)               { return (Get_Double (speed)); }
	double Time_Ratio (void)          { return (Get_Double (ratio)); }
	double Delay (void)               { return (Get_Double (delay)); }
	double Density (void)             { return (Get_Double (density)); }
	double Max_Density (void)         { return (Get_Double (max_density)); }
	double Queue (void)               { return (Get_Double (queue)); }
	double Max_Queue (void)           { return (Get_Double (max_queue)); }
	double Failure (void)             { return (Get_Double (failure)); }
	double Veh_Dist (void)            { return (Get_Double (veh_dist)); }
	double Veh_Time (void)            { return (Get_Double (veh_time)); }
	double Veh_Delay (void)           { return (Get_Double (veh_delay)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Time (double value)        { Put_Field (time, value); }
	void   Time (Dtime value)         { Put_Field (time, value.Seconds ()); }
	void   Persons (double value)     { Put_Field (persons, value); }
	void   Volume (double value)      { Put_Field (volume, value); }
	void   Enter (double value)       { Put_Field (enter, value); }
	void   Exit (double value)        { Put_Field (exit, value); }
	void   Flow (double value)        { Put_Field (flow, value); }
	void   Speed (double value)       { Put_Field (speed, value); }
	void   Time_Ratio (double value)  { Put_Field (ratio, value); }
	void   Delay (double value)       { Put_Field (delay, value); }
	void   Delay (Dtime value)        { Put_Field (delay, value.Seconds ()); }
	void   Density (double value)     { Put_Field (density, value); }
	void   Max_Density (double value) { Put_Field (max_density, value); }
	void   Queue (double value)       { Put_Field (queue, value); }
	void   Max_Queue (double value)   { Put_Field (max_queue, value); }
	void   Failure (double value)     { Put_Field (failure, value); }
	void   Veh_Dist (double value)    { Put_Field (veh_dist, value); }
	void   Veh_Time (double value)    { Put_Field (veh_time, value); }
	void   Veh_Delay (double value)   { Put_Field (veh_delay, value); }

	bool   Lane_Use_Flag (void)       { return (lane_use_flag); }
	void   Lane_Use_Flag (bool flag)  { lane_use_flag = flag; }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int link, dir, type, start, end, time, persons, volume, enter, exit, flow;
	int speed, delay, density, max_density, ratio, queue, max_queue, failure, veh_dist, veh_time, veh_delay;

	bool lane_use_flag;
};

#endif
