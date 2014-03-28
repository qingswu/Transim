//*********************************************************
//	Sim_Plan_Data.hpp - travel plan data
//*********************************************************

#ifndef SIM_PLAN_DATA_HPP
#define SIM_PLAN_DATA_HPP

#include "Data_Pack.hpp"
#include "Trip_Data.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Sim_Leg_Data class definition
//---------------------------------------------------------

class Sim_Leg_Data
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

	void  Clear (void)                { memset (this, '\0', sizeof (*this)); }

	bool Pack (Data_Buffer &data)     { return (data.Add_Data (this, sizeof (*this))); }
	bool UnPack (Data_Buffer &data)   { return (data.Get_Data (this, sizeof (*this))); }

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

typedef Deque <Sim_Leg_Data>             Sim_Leg_Queue;
typedef Sim_Leg_Queue::iterator          Sim_Leg_Itr;
typedef Sim_Leg_Queue::reverse_iterator  Sim_Leg_RItr;
typedef Sim_Leg_Data *                   Sim_Leg_Ptr;

//---------------------------------------------------------
//	Sim_Plan_Data class definition
//---------------------------------------------------------

class Sim_Plan_Data : public Sim_Leg_Queue
{
public:
	Sim_Plan_Data (void)             { Clear (); }

	int   Tour (void)                { return (x.tour); }
	int   Trip (void)                { return (x.trip); }
	int   Type (void)                { return (x.type); }
	Dtime Start (void)               { return (x.start); }
	Dtime End (void)                 { return (x.end_time); }
	Dtime Duration (void)            { return (x.duration); }
	int   Origin (void)              { return (x.origin); }
	int   Destination (void)         { return (x.destination); }
	int   Purpose (void)             { return (x.purpose); }
	int   Mode (void)                { return (x.mode); }
	int   Constraint (void)          { return (x.constraint); }
	int   Priority (void)            { return (x.priority); }
	int   Vehicle (void)             { return (x.vehicle); }
	int   Veh_Type (void)            { return (x.veh_type); }

	void  Tour (int value)           { x.tour = (char) value; }
	void  Trip (int value)           { x.trip = (char) value; }
	void  Type (int value)           { x.type = (short) value; }
	void  Start (Dtime value)        { x.start = value; }
	void  End (Dtime value)          { x.end_time = value; }
	void  Duration (Dtime value)     { x.duration = value; }
	void  Origin (int value)         { x.origin = value; }
	void  Destination (int value)    { x.destination = value; }
	void  Purpose (int value)        { x.purpose = (char) value; }
	void  Mode (int value)           { x.mode = (char) value; }
	void  Constraint (int value)     { x.constraint = (char) value; }
	void  Priority (int value)       { x.priority = (char) value; }
	void  Vehicle (int value)        { x.vehicle = (short) value; }
	void  Veh_Type (int value)       { x.veh_type = (short) value; }

	bool  Active (void)              { return (!empty ()); }
	bool  Check_Ahead (void)         { return (size () > 1); }
	bool  Next_Leg (void)            { if (Active ()) { /*x.schedule += front ().Time ();*/ pop_front (); } return (Active ()); }

	Sim_Plan_Data *next_plan_ptr;

	void  Clear (void)
	{
		memset (&x, '\0', sizeof (x)); next_plan_ptr = 0;; clear ();
	}
	bool Pack (Data_Buffer &data)
	{
		if (data.Add_Data (&x, sizeof (x))) {
			if (Sim_Leg_Queue::Pack (data)) {
				if (next_plan_ptr == 0) {
					int flag = 0;
					return (data.Add_Data (&flag, sizeof (int)));
				} else {
					int flag = 1;
					if (data.Add_Data (&flag, sizeof (int))) {
						return (data.Add_Data (next_plan_ptr, sizeof (Sim_Plan_Data)));
					}
				}
			}
		}
		return (false);
	}
	bool UnPack (Data_Buffer &data)
	{
		if (data.Get_Data (&x, sizeof (x))) {
			if (Sim_Leg_Queue::UnPack (data)) {
				int flag;
				if (data.Get_Data (&flag, sizeof (int))) {
					if (flag == 0) {
						next_plan_ptr = 0;
						return (true);
					}
					return (data.Get_Data (next_plan_ptr, sizeof (Sim_Plan_Data)));
				}
			}
		}
		return (false);
	}

private:
	struct {
		char  tour;
		char  trip;
		short type;
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
	} x;
};
typedef Vector <Sim_Plan_Data>    Sim_Plan_Array;
typedef Sim_Plan_Array::iterator  Sim_Plan_Itr;
typedef Sim_Plan_Data *           Sim_Plan_Ptr;

typedef Deque <Sim_Plan_Data>     Sim_Plan_Queue;

#endif
