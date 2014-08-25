//*********************************************************
//	Sim_Plan_Data.cpp - travel plan data
//*********************************************************

#include "Sim_Plan_Data.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Get_Leg
//---------------------------------------------------------

Sim_Leg_Ptr  Sim_Plan_Data::Get_Leg (void)
{
#ifdef CHECK
	if (leg_pool < 0 || (int) sim->sim_leg_array.size () <= leg_pool) sim->Error ("Sim_Plan_Data::Get_Leg: leg_pool");
	if (first_leg >= 0 && (int) sim->sim_leg_array [leg_pool].size () <= first_leg) sim->Error ("Sim_Plan_Data::Get_Leg: first_leg");
#endif
	return ((first_leg >= 0) ? sim->sim_leg_array [leg_pool].Record_Pointer (first_leg) : 0);
}

//---------------------------------------------------------
//	Get_Next
//---------------------------------------------------------

Sim_Leg_Ptr  Sim_Plan_Data::Get_Next (Sim_Leg_Ptr leg_ptr)
{
	Sim_Leg_Ptr ptr;
	if (leg_ptr == 0) {
		ptr = Get_Leg ();
		if (ptr == 0) return (0);
	} else {
		ptr = leg_ptr;
	}
	int next = ptr->Next_Record ();

#ifdef CHECK
	if (leg_pool < 0 || (int) sim->sim_leg_array.size () <= leg_pool) sim->Error ("Sim_Plan_Data::Get_Next: leg_pool");
	if (next >= 0 && (int) sim->sim_leg_array [leg_pool].size () <= next) sim->Error ("Sim_Plan_Data::Get_Next: next");
#endif
	return ((next >= 0) ? sim->sim_leg_array [leg_pool].Record_Pointer (next) : 0);
}

//---------------------------------------------------------
//	Next_Leg
//---------------------------------------------------------

bool  Sim_Plan_Data::Next_Leg (void)
{
#ifdef CHECK
	if (leg_pool < 0 || (int) sim->sim_leg_array.size () <= leg_pool) sim->Error ("Sim_Plan_Data::Next_Leg: leg_pool");
	if (first_leg < 0 || (int) sim->sim_leg_array [leg_pool].size () <= first_leg) sim->Error ("Sim_Plan_Data::Next_Leg: first_leg");
#endif
	Sim_Leg_Ptr ptr = Get_Leg ();
	if (ptr != 0) {
		schedule += ptr->Time ();
	}
	return ((first_leg = sim->sim_leg_array [leg_pool].Free_Record (first_leg)) >= 0);
}

//---------------------------------------------------------
//	Add_Leg
//---------------------------------------------------------

int  Sim_Plan_Data::Add_Leg (Sim_Leg_Data &leg, int last_leg)
{
#ifdef CHECK
	if (leg_pool < 0 || (int) sim->sim_leg_array.size () <= leg_pool) sim->Error ("Sim_Plan_Data::Add_Leg: leg_pool");
#endif
	last_leg = sim->sim_leg_array [leg_pool].Put_Record (leg, last_leg);

	if (first_leg < 0) {
		first_leg = last_leg;
	}
	return (last_leg);
}
