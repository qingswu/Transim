//*********************************************************
//	Sim_Travel_Data.hpp - simulator travel data
//*********************************************************

#ifndef SIM_TRAVEL_DATA_HPP
#define SIM_TRAVEL_DATA_HPP

#include "Sim_Plan_Data.hpp"
#include "Random.hpp"

//---------------------------------------------------------
//	Sim_Travel_Data class definition
//---------------------------------------------------------

class Sim_Travel_Data
{
public:
	Sim_Travel_Data (void)            { Clear (); }

	int   Traveler (void)             { return (traveler); }
	int   Household (void)            { return (hhold); }
	int   Person (void)               { return (person); }
	int   Problem (void)              { return (problem); }
	int   Status (void)               { return (status); }
	Dtime Next_Event (void)           { return (next_event); }
	int   Wait (void)                 { return (wait); }
	int   Vehicle (void)              { return (vehicle); }
	int   Speed (void)                { return (speed); }
	int   Type (void)                 { return (type); }
	int   Passengers (void)           { return (passengers); }
	int   Step_Code (void)            { return (step); }
	int   Plan_Index (void)           { return (plan_index); }

	void  Traveler (int value)        { traveler = value; };
	void  Household (int value)       { hhold = value; }
	void  Person (int value)          { person = (short) value; }
	void  Problem (int value)         { problem = (char) value; }
	void  Status (int value)          { status = (char) value; }
	void  Next_Event (Dtime value)    { next_event = value; }
	void  Wait (Dtime value)          { wait = value; }	
	void  Vehicle (int value)         { vehicle = value; }
	void  Speed (int value)           { speed = (short) value; }
	void  Type (int value)            { type = (short) value; }
	void  Passengers (int value)      { passengers = (short) value; }
	void  Step_Code (int code)        { step = (short) code; }
	void  Plan_Index (int value)      { plan_index = value; }

	void  Add_Wait (Dtime value)      { wait += value; }

	Sim_Plan_Ptr  Get_Plan (void);
	bool Next_Plan (void);
	void Add_Plan (Sim_Plan_Data &plan);

	void  Clear (void)                { memset (this, '\0', sizeof (*this)); vehicle = plan_index = -1; }

	bool  Pack (Data_Buffer &data)    { return (data.Add_Data (this, sizeof (*this))); }
	bool  UnPack (Data_Buffer &data)  { return (data.Get_Data (this, sizeof (*this))); }

	Random random;

private:
	int   traveler;
	int   hhold;
	short person;
	char  problem;
	char  status;
	Dtime next_event;
	Dtime wait;
	int   vehicle;
	short speed;
	short type;
	short passengers;
	short step;
	int   plan_index;
};

typedef Vector <Sim_Travel_Data>    Sim_Travel_Array;
typedef Sim_Travel_Array::iterator  Sim_Travel_Itr;
typedef Sim_Travel_Data *           Sim_Travel_Ptr;

//---------------------------------------------------------
//	Sim_Trip_Data class definition
//---------------------------------------------------------

class Sim_Trip_Data
{
public:
	Sim_Trip_Data (void) {}

	Sim_Travel_Data sim_travel_data;
	Sim_Plan_Data   sim_plan_data;
};

typedef Sim_Trip_Data *  Sim_Trip_Ptr;

#endif
