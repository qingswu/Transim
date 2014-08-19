//******************************************************** 
//	Old_Delay_File.hpp - Link Delay / Performance File
//********************************************************

#ifndef OLD_DELAY_FILE_HPP
#define OLD_DELAY_FILE_HPP

#include "APIDefs.hpp"
#include "Db_Header.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Old_Delay_File Class definition
//---------------------------------------------------------

class SYSLIB_API Old_Delay_File : public Db_Header
{
public:
	Old_Delay_File (Access_Type access, string format, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);
	Old_Delay_File (string filename, Access_Type access, string format, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);
	Old_Delay_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);
	Old_Delay_File (string filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT, Units_Type flow = VEHICLES, bool turn_flag = true, bool flow_flag = false);

	int    Link (void)                { return (Get_Integer (link)); }
	int    Dir (void)                 { return (Get_Integer (dir)); }
	int    Type (void)                { return (Get_Integer (type)); }
	Dtime  Start (void)               { return (Get_Time (start)); }
	Dtime  End (void)                 { return (Get_Time (end)); }
	double Flow (void)                { return (Get_Double (flow)); }
	double Time (void)                { return (Get_Double (time)); }
	int    Out_Link (void)            { return (Get_Integer (out_link)); }
	double Out_Flow (void)            { return (Get_Double (out_flow)); }
	double Out_Time (void)            { return (Get_Double (out_time)); }

	double Speed (void)               { return (Get_Double (speed)); }
	double Delay (void)               { return (Get_Double (delay)); }
	double Density (void)             { return (Get_Double (density)); }
	double Max_Density (void)         { return (Get_Double (max_den)); }
	double Time_Ratio (void)          { return (Get_Double (ratio)); }
	double Queue (void)               { return (Get_Double (queue)); }
	int    Max_Queue (void)           { return (Get_Integer (max_que)); }
	int    Cycle_Failure (void)       { return (Get_Integer (fail)); }

	int    TOD (void)                 { return (Get_Integer (tod)); }
	int    Volume (void)              { return (Get_Integer (volume)); }
	int    Enter (void)               { return (Get_Integer (enter)); }
	int    Exit (void)                { return (Get_Integer (exit)); }
	double Flow_Dist (void)           { return (Get_Double (vmt)); }
	double Flow_Time (void)           { return (Get_Double (vht)); }

	void   Link (int value)           { Put_Field (link, value); }
	void   Dir (int value)            { Put_Field (dir, value); }
	void   Type (int value)           { Put_Field (type, value); }
	void   Start (Dtime value)        { Put_Field (start, value); }
	void   End (Dtime value)          { Put_Field (end, value); }
	void   Flow (double value)        { Put_Field (flow, value); }
	void   Time (double value)        { Put_Field (time, value); }
	void   Time (Dtime value)         { Put_Field (time, value.Seconds ()); }
	void   Out_Link (int value)       { Put_Field (out_link, value); }
	void   Out_Flow (double value)    { Put_Field (out_flow, value); }
	void   Out_Time (double value)    { Put_Field (out_time, value); }
	void   Out_Time (Dtime value)     { Put_Field (out_time, value.Seconds ()); }

	void   Speed (double value)       { Put_Field (speed, value); }
	void   Delay (double value)       { Put_Field (delay, value); }
	void   Delay (Dtime value)        { Put_Field (delay, value.Seconds ()); }
	void   Density (double value)     { Put_Field (density, value); }
	void   Max_Density (double value) { Put_Field (max_den, value); }
	void   Time_Ratio (double value)  { Put_Field (ratio, value); }
	void   Queue (double value)       { Put_Field (queue, value); }
	void   Max_Queue (int value)      { Put_Field (max_que, value); }
	void   Cycle_Failure (int value)  { Put_Field (fail, value); }

	void   TOD (int value)            { Put_Field (tod, value); }
	void   Volume (int value)         { Put_Field (volume, value); }
	void   Enter (int value)          { Put_Field (enter, value); }
	void   Exit (int value)           { Put_Field (exit, value); }
	void   Flow_Dist (double value)   { Put_Field (vmt, value); }
	void   Flow_Time (double value)   { Put_Field (vht, value); }

	int    Increment (void)           { return (increment); }
	int    Iteration (void)           { return (iteration); }
	int    Period (void)              { return ((TOD () - 1) / increment); }

	void   Increment (int value)      { if (value > 0 && value <= MIDNIGHT) increment = value; }
	void   Iteration (int value)      { if (value > 0) iteration = value; }
	void   Period (int value)         { Put_Field (tod, value * increment); }
	
	Units_Type Flow_Units (void)      { return (flow_units); }
	void Flow_Units (Units_Type type) { flow_units = type; }

	bool   Turn_Flag (void)           { return (turn_flag); }
	void   Turn_Flag (bool flag)      { turn_flag = flag; }
	
	void   Skip_Turns (bool flag);

	bool   Lane_Use_Flows (void)      { return (type_flag); }
	void   Lane_Use_Flows (bool flag) { type_flag = flag; }

	bool   Link_Delay_Flag (void)     { return (volume < 0 || speed < 0); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);
	
	int link, dir, type, start, end, flow, time;
	int out_link, out_flow, out_time;
	int tod, volume, enter, exit, vmt, vht;
	int speed, delay, density, max_den, ratio, queue, max_que, fail;
	int increment, iteration;
	Units_Type flow_units;
	bool turn_flag, type_flag;
};

#endif
