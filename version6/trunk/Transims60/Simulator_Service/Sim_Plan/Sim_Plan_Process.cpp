//*********************************************************
//	Sim_Plan_Process.cpp - select and convert travel plans
//*********************************************************

#include "Sim_Plan_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Plan_Process -- constructor
//---------------------------------------------------------

Sim_Plan_Process::Sim_Plan_Process (void) : Static_Service ()
{
	num_plans = 0;
}

#ifdef THREADS

Sim_Plan_Process::Sim_Plan_Process (Plan_Queue &queue) : Static_Service ()
{ 
	plan_queue = &queue; 
	num_plans = 0;
}

//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Plan_Process::operator()()
{
	int number;
	Plan_Data *plan_ptr;
	Sim_Travel_Ptr sim_travel_ptr;

	for (;;) {
		plan_ptr = plan_queue->Get_Work (number);
		if (plan_ptr == 0) break;

		sim_travel_ptr = Plan_Processing (plan_ptr);
		delete plan_ptr;

		if (sim_travel_ptr == 0) break;

		if (!plan_queue->Put_Result (sim_travel_ptr, number)) break;
	}
}
#endif

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Plan_Process::Initialize (void)
{
	num_plans = 0;
}

