//*********************************************************
//	Sim_Travel_Data.cpp - simulator travel data
//*********************************************************

#include "Sim_Travel_Data.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Get_Plan
//---------------------------------------------------------

Sim_Plan_Ptr Sim_Travel_Data::Get_Plan (void)
{
#ifdef CHECK
	if (plan_index >= 0 && (int) sim->sim_plan_array.size () <= plan_index) sim->Error ("Sim_Travel_Data::Get_Plan: plan_index");
#endif
	return ((plan_index >= 0) ? sim->sim_plan_array.Record_Pointer (plan_index) : 0);
}

//---------------------------------------------------------
//	Next_Plan
//---------------------------------------------------------

bool Sim_Travel_Data::Next_Plan (void)
{
#ifdef CHECK
	if (plan_index < 0 || (int) sim->sim_plan_array.size () <= plan_index) sim->Error ("Sim_Travel_Data::Next_Plan: plan_index");
#endif
	return ((plan_index = sim->sim_plan_array.Free_Record (plan_index)) >= 0);
}

//---------------------------------------------------------
//	Add_Plan
//---------------------------------------------------------

void Sim_Travel_Data::Add_Plan (Sim_Plan_Data &plan)
{
	if (plan_index < 0) {
		plan_index = sim->sim_plan_array.Put_Record (plan);
		Status (NOT_ACTIVE);
	} else {
		int index;
		Sim_Plan_Ptr plan_ptr;

		for (index = plan_index; index >= 0; index = plan_ptr->Next_Record ()) {
#ifdef CHECK
			if (index < 0 || (int) sim->sim_plan_array.size () <= index) sim->Error ("Sim_Travel_Data::Add_Plan: index");
#endif
			plan_ptr = sim->sim_plan_array.Record_Pointer (index);
#ifdef CHECK
			if (plan_ptr == 0) sim->Error ("Sim_Travel_Data::Add_Plan: plan_ptr");
#endif
			if (plan_ptr->Next_Record () < 0) {
				sim->sim_plan_array.Put_Record (plan, index);
				break;
			}
		}
	}
}
