//*********************************************************
//	Turn_Delay_Data.hpp - turning movement data
//*********************************************************

#ifndef TURN_DELAY_DATA_HPP
#define TURN_DELAY_DATA_HPP

#include "APIDefs.hpp"
#include "Execution_Service.hpp"
#include "Data_Pack.hpp"
#include "Time_Periods.hpp"
#include "Dtime.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Turn_Delay_Data class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Delay_Data
{
public:
	Turn_Delay_Data (void)                   { Clear (); }

	int    Dir_Index (void)                 { return (dir_index); }
	int    To_Index (void)                  { return (to_index); }
	Dtime  Start (void)                     { return (start); }
	Dtime  End (void)                       { return (end); }
	double Turn (void)                      { return (turn); }
	Dtime  Time (void)                      { return (time); }
	
	void   Dir_Index (int value)            { dir_index = value; }
	void   To_Index (int value)             { to_index = value; }
	void   Start (Dtime value)              { start = value; }
	void   End (Dtime value)                { end = value; }
	void   Turn (double value)              { turn = (float) value; }
	void   Time (Dtime value)               { time = value; }

	void  Clear (void)
	{
		dir_index = to_index = -1; start = end = time = 0; turn = 0; 
	}	
private:
	int    dir_index;
	int    to_index;
	Dtime  start;
	Dtime  end;
	float  turn;
	Dtime  time;
};

typedef vector <Turn_Delay_Data>    Turn_Delay_Array;
typedef Turn_Delay_Array::iterator  Turn_Delay_Itr;

//---------------------------------------------------------
//	Turn_Data class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Data
{
public:
	Turn_Data (void)                    { Clear (); }

	double Turn (void)                   { return (turn); }
	Dtime  Time (void)                   { return (time); }

	void   Turn (double value)           { turn = (float) value; }
	void   Time (Dtime value)            { time = value; }
	void   Time (int value)              { time = value; }
	void   Time (double value)           { time = exe->Round (value); }

	void   Add_Turn (double value)       { turn = (float) (turn + value); }
	void   Add_Time (Dtime value)        { time += value; }

	void   Add_Turn_Time (double turn, Dtime tim);
	void   Add_Turn_Time (Turn_Data &rec) 
	                                     { Add_Turn_Time (rec.Turn (), rec.Time ()); }

	void   Average_Turn (double turn);

	void   Average_Turn_Time (double turn, Dtime tim, double weight = 1.0);
	void   Average_Turn_Time (Turn_Data &rec, double weight = 1.0) 
	                                     { Average_Turn_Time (rec.Turn (), rec.Time (), weight); }

	void   Clear (void)                  { turn = 0; time = 0; }

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
	float turn;
	Dtime time;
};

//---------------------------------------------------------
//	Turn_Period class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Period : public Vector <Turn_Data>
{
public:
	Turn_Period (void) { }

	Dtime  Time (int index)       { return (at (index).Time ()); }
	double Turn (int index)       { return (at (index).Turn ()); }

	Turn_Data * Data_Ptr (int index) { return (&at (index)); }
};
typedef Turn_Period::iterator  Turn_Itr;

//---------------------------------------------------------
//	Turn_Period_Array class definition
//---------------------------------------------------------

class SYSLIB_API Turn_Period_Array : public Vector <Turn_Period>
{
public:
	Turn_Period_Array (void) { periods = 0; num_records = 0; }

	void Initialize (Time_Periods *periods, int num_records = 0);

	void Replicate (Turn_Period_Array &period_array);

	void Zero_Times (void);
	void Zero_Turns (Dtime first_time = 0);

	void Copy_Turn_Data (Turn_Period_Array &period_array, bool zero_flag = false, Dtime first_time = 0);
	void Copy_Time_Data (Turn_Period_Array &period_array, bool zero_flag = true, Dtime first_time = 0);

	void Add_Turns (Turn_Period_Array &period_array, bool zero_flag = false);
	void Add_Turn_Times (Turn_Period_Array &period_array, bool zero_flag = false);

	void Average_Turns (Turn_Period_Array &period_array, bool zero_flag = false);
	void Average_Turn_Times (Turn_Period_Array &period_array, int weight = 1, bool zero_flag = false);

	Turn_Period * Period_Ptr (Dtime time);
	Turn_Period * Period_Ptr (int period)       { return (&at (period)); }

	Time_Periods * periods;
	int  Num_Records (void)  { return (num_records); }

private:
	int num_records;
};
typedef Turn_Period_Array::iterator  Turn_Period_Itr;

#endif

