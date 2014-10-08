//*********************************************************
//	Travel_Step.hpp - traveler activity during a time step
//*********************************************************

#ifndef TRAVEL_STEP_HPP
#define TRAVEL_STEP_HPP

#include "Sim_Travel_Data.hpp"
#include "Sim_Plan_Data.hpp"
#include "Sim_Dir_Data.hpp"
#include "Sim_Veh_Data.hpp"
#include "Veh_Type_Data.hpp"
#include "Dtime.hpp"

//---------------------------------------------------------
//	Travel_Step class definition
//---------------------------------------------------------

class Travel_Step : public Sim_Veh_Array 
{
public:
	Travel_Step (void)            { Clear (); }

	int   Traveler (void)         { return (traveler); }
	int   Dir_Index (void)        { return (dir_index); }
	int   Parking (void)          { return (parking); }
	int   Stop (void)             { return (stop); }
	int   Speed (void)            { return (speed); }
	int   Delay (void)            { return (delay); }
	int   Problem (void)          { return (problem); }
	int   Event_Type (void)       { return (event_type); }
	int   Process_ID (void)       { return (process_id); }
	bool  Exit_Flag (void)        { return (exit_flag); }

	void  Traveler (int value)    { traveler = value; }
	void  Dir_Index (int value)   { dir_index = value; }
	void  Parking (int value)     { parking = value; }
	void  Stop (int value)        { stop = value; }
	void  Speed (int value)       { speed = value; }
	void  Delay (int value)       { delay = value; }
	void  Problem (int value)     { problem = value; }
	void  Event_Type (int value)  { event_type = value; }
	void  Process_ID (int value)  { process_id = value; }
	void  Exit_Flag (bool flag)   { exit_flag = flag; }

	Sim_Travel_Ptr   sim_travel_ptr;
	Sim_Plan_Ptr     sim_plan_ptr;
	Sim_Leg_Ptr      sim_leg_ptr;
	Sim_Dir_Ptr      sim_dir_ptr;
	Sim_Veh_Ptr      sim_veh_ptr;
	Veh_Type_Data    *veh_type_ptr;

	void  Clear (void)
	{
		traveler = dir_index = parking = stop = -1; speed = delay = problem = event_type = process_id = 0; 
		sim_travel_ptr = 0; sim_plan_ptr = 0; sim_leg_ptr = 0; sim_dir_ptr = 0; sim_veh_ptr = 0; veh_type_ptr = 0; 
		exit_flag = true;  clear ();
	}

	void  Clear_Travel (void)
	{
		traveler = parking = stop = -1; speed = delay = problem = event_type = 0; 
		sim_travel_ptr = 0; sim_plan_ptr = 0; sim_leg_ptr = 0; sim_veh_ptr = 0; veh_type_ptr = 0; 
		clear ();
	}

private:
	int   traveler;
	int   dir_index;
	int   parking;
	int   stop;
	int   speed;
	int   delay;
	int   problem;
	int   event_type;
	int   process_id;
	bool  exit_flag;
};
#endif
