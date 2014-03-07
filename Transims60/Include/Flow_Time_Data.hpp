//*********************************************************
//	Flow_Time_Data.hpp - flow rate and travel time data
//*********************************************************

#ifndef FLOW_TIME_DATA_HPP
#define FLOW_TIME_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Data_Pack.hpp"
#include "Time_Periods.hpp"

//---------------------------------------------------------
//	Flow_Time_Data class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Data
{
public:
	Flow_Time_Data (void)                { Clear (); }

	double Flow (void)                   { return (flow); }
	Dtime  Time (void)                   { return (time); }

	void   Flow (double value)           { flow = (float) value; }
	void   Time (Dtime value)            { time = value; }
	void   Time (int value)              { time = value; }
	void   Time (double value)           { time = exe->Round (value); }

	void   Add_Flow (double value)       { flow = (float) (flow + value); }
	void   Add_Time (Dtime value)        { time += value; }

	void   Add_Flow_Time (double flw, Dtime tim);
	void   Add_Flow_Time (Flow_Time_Data &rec) 
	                                     { Add_Flow_Time (rec.Flow (), rec.Time ()); }

	void   Average_Flow (double flw);

	void   Average_Flow_Time (double flw, Dtime tim, int weight = 1);
	void   Average_Flow_Time (Flow_Time_Data &rec, int weight = 1) 
	                                     { Average_Flow_Time (rec.Flow (), rec.Time (), weight); }

	void   Clear (void)                  { flow = 0; time = 0; }

#ifdef MAKE_MPI
	bool Pack (Data_Buffer &data)
	{
		return (data.Add_Data (this, sizeof (*this)));
	}
	bool UnPack (Data_Buffer &data)
	{
		return (data.Get_Data (this, sizeof (*this)));
	}
#endif

private:
	float flow;
	Dtime time;
};

//---------------------------------------------------------
//	Flow_Time_Array class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Array : public Vector <Flow_Time_Data>
{
public:
	Flow_Time_Array (void) { }

	Dtime  Time (int index)       { return (at (index).Time ()); }
	double Flow (int index)       { return (at (index).Flow ()); }

	Flow_Time_Data * Data_Ptr (int index) { return (&at (index)); }

	Flow_Time_Data Total_Flow_Time (int index);
	Flow_Time_Data Total_Flow_Time (int index, int flow_index);
};
typedef Flow_Time_Array::iterator  Flow_Time_Itr;

//---------------------------------------------------------
//	Flow_Time_Period_Array class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Period_Array : public Vector <Flow_Time_Array>
{
public:
	Flow_Time_Period_Array (void) { periods = 0; num_records = 0; }

	void Initialize (Time_Periods *periods, int num_records = 0);

	void Replicate (Flow_Time_Period_Array &period_array);

	void Set_Time0 (void);
	void Zero_Flows (Dtime first_time = 0);

	void Copy_Flow_Data (Flow_Time_Period_Array &period_array, bool zero_flag = false);

	void Add_Flows (Flow_Time_Period_Array &period_array, bool zero_flag = false);
	void Add_Flow_Times (Flow_Time_Period_Array &period_array, bool zero_flag = false);

	void Average_Flows (Flow_Time_Period_Array &period_array, bool zero_flag = false);
	void Average_Flow_Times (Flow_Time_Period_Array &period_array, int weight = 1, bool zero_flag = false);

	Flow_Time_Array * Period_Ptr (Dtime time);

	Time_Periods * periods;
	int  Num_Records (void)  { return (num_records); }

private:
	int num_records;
};
typedef Flow_Time_Period_Array::iterator  Flow_Time_Period_Itr;

#endif
