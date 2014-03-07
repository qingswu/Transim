//*********************************************************
//	Sim_Travel_Data.hpp - simulator travel data
//*********************************************************

#ifndef SIM_TRAVEL_DATA_HPP
#define SIM_TRAVEL_DATA_HPP

#include "Sim_Plan_Data.hpp"
#include "Random.hpp"
#include "Data_Pack.hpp"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------------------------
//	Sim_Travel_Data class definition
//---------------------------------------------------------

class Sim_Travel_Data
{
public:
	Sim_Travel_Data (void)           { Clear (); }

	int   Traveler (void)            { return (x.traveler); }
	int   Household (void)           { return (x.hhold); }
	int   Person (void)              { return (x.person); }
	int   Problem (void)             { return (x.problem); }
	int   Status (void)              { return (x.status); }
	Dtime Schedule (void)            { return (x.schedule); }
	Dtime Next_Event (void)          { return (x.next_event); }
	int   Wait (void)                { return (x.wait); }
	int   Vehicle (void)             { return (x.vehicle); }
	int   Speed (void)               { return (x.speed); }
	int   Type (void)                { return (x.type); }
	int   Passengers (void)          { return (x.passengers); }
	bool  Step_Flag (void)           { return (x.step != 0); }

	void  Traveler (int value)       { x.traveler = value; };
	void  Household (int value)      { x.hhold = value; }
	void  Person (int value)         { x.person = (short) value; }
	void  Problem (int value)        { x.problem = (char) value; }
	void  Status (int value)         { x.status = (char) value; }
	void  Schedule (Dtime value)     { x.schedule = value; }
	void  Next_Event (Dtime value)   { x.next_event = value; }
	void  Wait (Dtime value)         { x.wait = value; }	
	void  Vehicle (int value)        { x.vehicle = value; }
	void  Speed (int value)          { x.speed = (short) value; }
	void  Type (int value)           { x.type = (short) value; }
	void  Passengers (int value)     { x.passengers = (short) value; }
	void  Step_Flag (bool flag)      { x.step = (short) ((flag) ? 1 : 0); }

	void  Add_Wait (Dtime value)     { x.wait += value; }

	Sim_Plan_Ptr Next_Plan (void)    { Sim_Plan_Ptr ptr = sim_plan_ptr; 
	                                   if (ptr > 0) { sim_plan_ptr = ptr->next_plan_ptr; delete ptr; } 
									   return (sim_plan_ptr); 
	                                 }
	Sim_Plan_Ptr sim_plan_ptr;
	Random random;

	void Clear (void)                { memset (&x, '\0', sizeof (x)); x.next_event = -1; x.vehicle = -1; sim_plan_ptr = 0; }

	bool  Pack (Data_Buffer &data)
	{
		if (data.Add_Data (&x, sizeof (x))) {
			if (data.Add_Data (&random, sizeof (random))) {
				if (sim_plan_ptr == 0) {
					int flag = 0;
					return (data.Add_Data (&flag, sizeof (int)));
				} else {
					int flag = 1;
					if (data.Add_Data (&flag, sizeof (int))) {
						return (data.Add_Data (sim_plan_ptr, sizeof (Sim_Plan_Data)));
					}
				}
			}
		}
		return (false);
	}
	bool  UnPack (Data_Buffer &data)
	{
		if (data.Get_Data (&x, sizeof (x))) {
			if (data.Get_Data (&random, sizeof (random))) {
				int flag;
				if (data.Get_Data (&flag, sizeof (int))) {
					if (flag == 0) {
						sim_plan_ptr = 0;
						return (true);
					}
					return (data.Get_Data (sim_plan_ptr, sizeof (Sim_Plan_Data)));
				}
			}
		}
		return (false);
	}

private:
	struct {
		int   traveler;
		int   hhold;
		short person;
		char  problem;
		char  status;
		Dtime schedule;
		Dtime next_event;
		Dtime wait;
		int   vehicle;
		short speed;
		short type;
		short passengers;
		short step;
	} x;
};

typedef Vector <Sim_Travel_Data>    Sim_Travel_Array;
typedef Sim_Travel_Array::iterator  Sim_Travel_Itr;
typedef Sim_Travel_Data *           Sim_Travel_Ptr;

#endif
