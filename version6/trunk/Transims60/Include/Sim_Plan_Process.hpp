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

typedef Ordered_Work <Plan_Data, Sim_Travel_Data> Plan_Queue;
#endif

//---------------------------------------------------------
//	Sim_Plan_Process - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Plan_Process : Static_Service
{
public:
	Sim_Plan_Process (void);

#ifdef THREADS
	Sim_Plan_Process (Plan_Queue &queue);

	Plan_Queue *plan_queue;

	void operator()();
#endif
	void Initialize (void);

	Sim_Travel_Ptr Plan_Processing (Plan_Data *plan_ptr);

	int  Num_Plans (void)               { return (num_plans); }

	void Reset_Counters (void)          { num_plans = 0; }

private:
	int num_plans;

	bool Best_Lanes (Sim_Travel_Ptr sim_travel_ptr);
};
#endif
