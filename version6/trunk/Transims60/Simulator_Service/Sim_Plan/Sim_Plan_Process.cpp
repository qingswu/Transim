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
	num_plans = leg_pool = 0;
	id = 1;
}

#ifdef THREADS

Sim_Plan_Process::Sim_Plan_Process (Trip_Queue *queue, int _id) : Static_Service ()
{ 
	trip_queue = queue; 
	num_plans = 0;
	leg_pool = _id;
	id = _id + 1;
}

//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Plan_Process::operator()()
{
	int number;
	Plan_Data *plan_ptr;
	Sim_Trip_Ptr sim_trip_ptr;

	for (;;) {
		plan_ptr = trip_queue->Get_Work (number);
		if (plan_ptr == 0) break;

		sim_trip_ptr = Plan_Processing (plan_ptr);
		if (!sim->router_flag) {
			delete plan_ptr;
		}
		if (sim_trip_ptr == 0) break;
		if (!trip_queue->Put_Result (sim_trip_ptr, number)) break;
	}
}
#endif

