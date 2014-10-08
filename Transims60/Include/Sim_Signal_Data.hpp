//*********************************************************
//	Sim_Signal_Data.hpp - current signal status data
//*********************************************************

#ifndef SIM_SIGNAL_DATA_HPP
#define SIM_SIGNAL_DATA_HPP

#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Ring_Data class definition
//---------------------------------------------------------

class Ring_Data : public Integers
{
public:
	Ring_Data (void)                { Clear (); }

	Dtime Start (void)              { return (start); }
	Dtime Check (void)              { return (check); }
	int   Status (void)             { return (status); }
	int   Phase_Index (void)        { return (phase); }
	int   Barrier (void)            { return (barrier); }

	void  Start (Dtime value)       { start = value; }
	void  Check (Dtime value)       { check = value; }
	void  Status (int value)        { status = (short) value; }
	void  Phase_Index (int value)   { phase = (char) value; }
	void  Barrier (int value)       { barrier = (char) value; }

	void Clear (void)
	{
		start = check = 0; status = 0; phase = barrier = 0; clear ();
	}
private:
	Dtime  start;
	Dtime  check;
	short  status;
	char   phase;
	char   barrier;
};

typedef Vector <Ring_Data>    Ring_Array;
typedef Ring_Array::iterator  Ring_Itr;

//---------------------------------------------------------
//	Sim_Signal_Data class definition
//---------------------------------------------------------
 
class Sim_Signal_Data : public Ring_Array
{
public:
	Sim_Signal_Data (void)          { Clear (); }

	Dtime End_Time (void)           { return (end_time); }
	Dtime Check_Time (void)         { return (check_time); }
	int   Timing_Index (void)       { return (timing_index); }
	int   Status (void)             { return (status); }

	void  End_Time (Dtime value)    { end_time = value; }
	void  Check_Time (Dtime value)  { check_time = value; }
	void  Timing_Index (int value)  { timing_index = value; }
	void  Status (int value)        { status = value; }

	void Clear (void)
	{
		end_time = check_time = MAX_INTEGER; timing_index = -1; status = 0; clear ();
	}
private:
	Dtime end_time;
	Dtime check_time;
	int   timing_index;
	int   status;
};

typedef Vector <Sim_Signal_Data>    Sim_Signal_Array;
typedef Sim_Signal_Array::iterator  Sim_Signal_Itr;

#endif
