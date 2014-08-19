//*********************************************************
//	Volume_Array.hpp - volume by time period
//*********************************************************

#ifndef VOLUME_ARRAY_HPP
#define VOLUME_ARRAY_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Time_Periods.hpp"
#include "Data_Pack.hpp"

typedef Dbls_Ptr   Vol_Period_Ptr;

//---------------------------------------------------------
//	Volume_Array class definition
//---------------------------------------------------------

class SYSLIB_API Volume_Array  : public Dbls_Array
{
public:
	Volume_Array (void)        { periods = 0; num_records = 0; }

	void Initialize (Time_Periods *periods, int num_records = 0);

	int  Num_Records (void)    { return (num_records); }
	
	void Zero_Volumes (Dtime first_time = 0);
	
	double Volume (int period, int index);
	void   Volume (int period, int index, double volume);

	Vol_Period_Ptr Period_Ptr (Dtime time);
	Vol_Period_Ptr Period_Ptr (int period)       { return (&at (period)); }

	Time_Periods *periods;

private:
	int num_records;
};
typedef Volume_Array::iterator  Vol_Array_Itr;

//---------------------------------------------------------
//	Vol_Spd_Data class definition
//---------------------------------------------------------

class SYSLIB_API Vol_Spd_Data
{
public:
	Vol_Spd_Data (void)                 { Clear (); }

	double Volume (void)                { return (volume); }
	double Speed (void)                 { return (speed); }

	void   Volume (double value)        { volume = (float) value; }
	void   Speed (double value)         { speed = (float) value; }

	void   Add_Volume (double value)    { volume = (float) (volume + value); }

	void   Add_Vol_Spd (double volume, double speed);
	void   Add_Vol_Spd (Vol_Spd_Data &data) { Add_Vol_Spd (data.Volume (), data.Speed ()); }

	void   Clear (void)                 { volume = speed = 0; }

private:
	float volume;
	float speed;
};

//---------------------------------------------------------
//	Vol_Spd_Period class definition
//---------------------------------------------------------

class SYSLIB_API Vol_Spd_Period : public Vector <Vol_Spd_Data>
{
public:
	Vol_Spd_Period (void) { }

	double Volume (int index)      { return (at (index).Volume ()); }
	double Speed (int index)       { return (at (index).Speed ()); }

	Vol_Spd_Data * Data_Ptr (int index) { return (&at (index)); }

	Vol_Spd_Data Total_Vol_Spd (int index);
	Vol_Spd_Data Total_Vol_Spd (int index, int use_index);
};
typedef Vol_Spd_Period::iterator  Vol_Spd_Itr;

//---------------------------------------------------------
//	Vol_Spd_Period_Array class definition
//---------------------------------------------------------

class SYSLIB_API Vol_Spd_Period_Array : public Vector <Vol_Spd_Period>
{
public:
	Vol_Spd_Period_Array (void) { periods = 0; num_records = 0; }

	void Initialize (Time_Periods *periods, int num_records = 0);

	void Replicate (Vol_Spd_Period_Array &period_array);

	void Zero_Data (void);

	Vol_Spd_Period * Period_Ptr (Dtime time);
	Vol_Spd_Period * Period_Ptr (int period)       { return (&at (period)); }

	Time_Periods * periods;
	int  Num_Records (void)  { return (num_records); }

private:
	int num_records;
};
typedef Vol_Spd_Period_Array::iterator  Vol_Spd_Period_Itr;

#endif

