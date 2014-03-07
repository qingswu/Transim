//*********************************************************
//	Trip_Data.hpp - trip data classes
//*********************************************************

#ifndef TRIP_DATA_HPP
#define TRIP_DATA_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Trip_Index.hpp"
#include "Time_Index.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Trip_Data class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Data
{
public:
	Trip_Data (void)                 { Clear (); }

	int   Household (void)           { return (hhold); }
	int   Person (void)              { return (person); }
	int   Tour (void)                { return (tour); }
	int   Trip (void)                { return (trip); }
	Dtime Start (void)               { return (start); }
	Dtime End (void)                 { return (end_time); }
	Dtime Duration (void)            { return (duration); }
	int   Origin (void)              { return (origin); }
	int   Destination (void)         { return (destination); }
	int   Purpose (void)             { return (purpose); }
	int   Mode (void)                { return (mode); }
	int   Constraint (void)          { return (constraint); }
	int   Priority (void)            { return (priority); }
	int   Vehicle (void)             { return (vehicle); }
	int   Veh_Type (void)            { return (veh_type); }
	int   Type (void)                { return (type); }
	int   Partition (void)           { return (partition); }

	void  Household (int value)      { hhold = value; }
	void  Person (int value)         { person = (short) value; }
	void  Tour (int value)           { tour = (char) value; }
	void  Trip (int value)           { trip = (char) value; }
	void  Start (Dtime value)        { start = value; }
	void  End (Dtime value)          { end_time = value; }
	void  Duration (Dtime value)     { duration = value; }
	void  Origin (int value)         { origin = value; }
	void  Destination (int value)    { destination = value; }
	void  Purpose (int value)        { purpose = (char) value; }
	void  Mode (int value)           { mode = (char) value; }
	void  Constraint (int value)     { constraint = (char) value; }
	void  Priority (int value)       { priority = (char) value; }
	void  Vehicle (int value)        { vehicle = (short) value; }
	void  Veh_Type (int value)       { veh_type = (short) value; }
	void  Type (int value)           { type = (short) value; }
	void  Partition (int value)      { partition = (short) value; }

	Trip_Index Get_Trip_Index (void) { return (Trip_Index (hhold, person, tour, trip)); }
	Time_Index Get_Time_Index (void) { return (Time_Index (start, hhold, person)); }

	void Get_Trip_Index (Trip_Index &index) { index.Set (hhold, person, tour, trip); }
	void Get_Time_Index (Time_Index &index) { index.Set (start, hhold, person); }

	void  Clear (void)
	{
		hhold = origin = destination = 0; 
		tour = trip = purpose = mode = constraint = priority = 0;
		person = vehicle = veh_type = type = partition = 0; 
		start = end_time = duration = 0;
	}
private:
	int   hhold;
	short person;
	char  tour;
	char  trip;
	Dtime start;
	Dtime end_time;
	Dtime duration;
	int   origin;
	int   destination;
	char  purpose;
	char  mode;
	char  constraint;
	char  priority;
	short vehicle;
	short veh_type;
	short type;
	short partition;
};

typedef Trip_Data *               Trip_Ptr;

//---------------------------------------------------------
//	Trip_Partition class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Partition
{
public:
	Trip_Partition (void)                { max_part = 0; }

	int  Max_Partition (void)            { return (max_part); }
	void Max_Partition (int value)       { max_part = value; }
	void Max_Partition (Trip_Data &dat)  { if (dat.Partition () > max_part) max_part = dat.Partition (); }
	void Max_Partition (Trip_Ptr dat)    { if (dat->Partition () > max_part) max_part = dat->Partition (); }
private:
	int max_part;
};

//---------------------------------------------------------
//	Trip_Array class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Array : public Trip_Partition, public vector <Trip_Data> 
{
public:
	Trip_Array (void) { }
};
typedef Trip_Array::iterator      Trip_Itr;

//---------------------------------------------------------
//	Trip_Ptr_Array class definition
//---------------------------------------------------------

class SYSLIB_API Trip_Ptr_Array : public Trip_Partition, public vector <Trip_Ptr> 
{
public:
	Trip_Ptr_Array (void) { }
};
typedef Trip_Ptr_Array::iterator  Trip_Ptr_Itr;

#endif

