//*********************************************************
//	Sim_Plan_Data.hpp - travel plan data
//*********************************************************

#ifndef SIM_PLAN_DATA_HPP
#define SIM_PLAN_DATA_HPP

#include "Memory_Pool.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Sim_Leg_Data class definition
//---------------------------------------------------------

class Sim_Leg_Data : public Memory_Record
{
public:
	Sim_Leg_Data (void)               { Clear (); }

	int   Mode (void)                 { return (mode); }
	int   Type (void)                 { return (type); }
	int   Index (void)                { return (index); }
	int   Max_Speed (void)            { return (mode_data); }
	int   Stop_Number (void)          { return (mode_data); }
	Dtime Time (void)                 { return (time); }
	int   In_Lane_Low (void)          { return (in_lane_low); }
	int   In_Lane_High (void)         { return (in_lane_high); }
	int   In_Best_Low (void)          { return (in_best_low); }
	int   In_Best_High (void)         { return (in_best_high); }
	int   Out_Lane_Low (void)         { return (out_lane_low); }
	int   Out_Lane_High (void)        { return (out_lane_high); }
	int   Out_Best_Low (void)         { return (out_best_low); }
	int   Out_Best_High (void)        { return (out_best_high); }
	int   Connect (void)              { return (connect); }

	void  Mode (int value)            { mode = (char) value; }
	void  Type (int value)            { type = (char) value; }
	void  Index (int value)           { index = value; }
	void  Max_Speed (int value)       { mode_data = (short) value; }
	void  Stop_Number (int value)     { mode_data = (short) value; }
	void  Time (Dtime value)          { time = value; }
	void  In_Lane_Low (int value)     { in_lane_low = (char) value; }
	void  In_Lane_High (int value)    { in_lane_high = (char) value; }
	void  In_Best_Low (int value)     { in_best_low = (char) value; }
	void  In_Best_High (int value)    { in_best_high = (char) value; }
	void  Out_Lane_Low (int value)    { out_lane_low = (char) value; }
	void  Out_Lane_High (int value)   { out_lane_high = (char) value; }
	void  Out_Best_Low (int value)    { out_best_low = (char) value; }
	void  Out_Best_High (int value)   { out_best_high = (char) value; }
	void  Connect (int value)         { connect = value; }
	
	void  Clear (void)                { memset (this, '\0', sizeof (*this)); Memory_Record::Clear (); }

	bool  Pack (Data_Buffer &data)    { return (data.Add_Data (this, sizeof (*this))); }
	bool  UnPack (Data_Buffer &data)  { return (data.Get_Data (this, sizeof (*this))); }

private:
	int   index;
	char  mode;
	char  type;
	short mode_data;		//---- max_speed or line stop number ----
	Dtime time;
	char  in_lane_low;
	char  in_lane_high;
	char  in_best_low;
	char  in_best_high;
	char  out_lane_low;
	char  out_lane_high;
	char  out_best_low;
	char  out_best_high;
	int   connect;			//---- connect_array index ---- 
};

typedef Memory_Pool <Sim_Leg_Data> Sim_Leg_Pool;
typedef Sim_Leg_Pool::iterator     Sim_Leg_Itr;
typedef Sim_Leg_Data *             Sim_Leg_Ptr;

typedef vector <Sim_Leg_Pool>      Leg_Pool_Array;
typedef Leg_Pool_Array::iterator   Leg_Pool_Itr;
typedef Sim_Leg_Pool *             Leg_Pool_Ptr;

//---------------------------------------------------------
//	Sim_Plan_Data class definition
//---------------------------------------------------------

class Sim_Plan_Data : public Memory_Record
{
public:
	Sim_Plan_Data (void)              { Clear (); }

	int   Tour (void)                 { return (tour); }
	int   Trip (void)                 { return (trip); }
	int   Type (void)                 { return (type); }
	Dtime Start (void)                { return (start); }
	Dtime End (void)                  { return (end_time); }
	Dtime Activity (void)             { return (activity); }
	Dtime Schedule (void)             { return (schedule); }
	int   Origin (void)               { return (origin); }
	int   Destination (void)          { return (destination); }
	int   Purpose (void)              { return (purpose); }
	int   Mode (void)                 { return (mode); }
	int   Constraint (void)           { return (constraint); }
	int   Priority (void)             { return (priority); }
	int   Vehicle (void)              { return (vehicle); }
	int   Veh_Type (void)             { return (veh_type); }
	int   Leg_Pool (void)             { return (leg_pool); }
	int   First_Leg (void)            { return (first_leg); }

	void  Tour (int value)            { tour = (char) value; }
	void  Trip (int value)            { trip = (char) value; }
	void  Type (int value)            { type = (short) value; }
	void  Start (Dtime value)         { start = value; }
	void  End (Dtime value)           { end_time = value; }
	void  Activity (Dtime value)      { activity = value; }
	void  Schedule (Dtime value)      { schedule = value; }
	void  Origin (int value)          { origin = value; }
	void  Destination (int value)     { destination = value; }
	void  Purpose (int value)         { purpose = (char) value; }
	void  Mode (int value)            { mode = (char) value; }
	void  Constraint (int value)      { constraint = (char) value; }
	void  Priority (int value)        { priority = (char) value; }
	void  Vehicle (int value)         { vehicle = (char) value; }
	void  Veh_Type (int value)        { veh_type = (char) value; }
	void  Leg_Pool (int value)        { leg_pool = (short) value; }
	void  First_Leg (int value)       { first_leg = value; }

	Sim_Leg_Ptr Get_Leg (void);
	Sim_Leg_Ptr Get_Next (Sim_Leg_Ptr leg_ptr = 0);
	bool  Next_Leg (void);
	int   Add_Leg (Sim_Leg_Data &leg, int last_leg);

	void  Clear (void)                { memset (this, '\0', sizeof (*this)); Memory_Record::Clear (); First_Leg (-1); }

	bool  Pack (Data_Buffer &data)    { return (data.Add_Data (this, sizeof (*this))); }
	bool  UnPack (Data_Buffer &data)  { return (data.Get_Data (this, sizeof (*this))); }

private:
	char  tour;
	char  trip;
	short type;
	Dtime start;
	Dtime end_time;
	Dtime activity;
	Dtime schedule;
	int   origin;
	int   destination;
	char  purpose;
	char  mode;
	char  constraint;
	char  priority;
	char  vehicle;
	char  veh_type;
	short leg_pool;
	int   first_leg;
};

typedef Memory_Pool <Sim_Plan_Data> Sim_Plan_Pool;
typedef Sim_Plan_Pool::iterator     Sim_Plan_Itr;
typedef Sim_Plan_Data *             Sim_Plan_Ptr;

#endif
