//*********************************************************
//	Sim_Plan_Process.hpp - select and convert travel plans
//*********************************************************

#ifndef SIM_PLAN_PROCESS_HPP
#define SIM_PLAN_PROCESS_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Plan_Data.hpp"
#include "Sim_Travel_Data.hpp"
#include "Sim_Plan_Data.hpp"

#ifdef THREADS
#include "Ordered_Work.hpp"

typedef Ordered_Work <Plan_Data, Sim_Trip_Data> Trip_Queue;
#endif

//---------------------------------------------------------
//	Sim_Plan_Process - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Plan_Process : Static_Service
{
public:
	Sim_Plan_Process ();

#ifdef THREADS
	Sim_Plan_Process (Trip_Queue *queue, int id = 0);

	Trip_Queue *trip_queue;

	void operator()();
#endif

	Sim_Trip_Ptr Plan_Processing (Plan_Data *plan_ptr);

	int  ID (void)                      { return (id); }
	int  Leg_Pool (void)                { return (leg_pool); }
	int  Num_Plans (void)               { return (num_plans); }

	void Reset_Counters (void)          { num_plans = 0; }

private:
	int id, leg_pool, num_plans;

	bool Best_Lanes (Sim_Trip_Ptr sim_trip, Integers &leg_list);
};
#endif
