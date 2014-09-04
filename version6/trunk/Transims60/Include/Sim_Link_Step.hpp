//*********************************************************
//	Sim_Link_Step.hpp - simulate directional links
//*********************************************************

#ifndef SIM_LINK_STEP_HPP
#define SIM_LINK_STEP_HPP

#include "Sim_Link_Process.hpp"

//---------------------------------------------------------
//	Sim_Link_Step - process traveler events
//---------------------------------------------------------

class SYSLIB_API Sim_Link_Step : public Static_Service
{
public:
	Sim_Link_Step (void);
	~Sim_Link_Step (void);

	void Initialize (void);
	void Start_Processing (void);
	void Stop_Processing (void);

	void Randomize_Links (void);

	void Add_Statistics (Sim_Statistics &_stats);

	int  Num_PCE (void)                 { return (num_pce); }
	int  Num_Vehicles (void)            { return (num_vehicles); }
	int  Num_Waiting (void)             { return (num_waiting); }

private:
	int num_pce, num_vehicles, num_waiting;
	Integers link_list;

#ifdef THREADS
	int num_threads;
	Threads threads;
	Work_Queue link_queue;
	Sim_Link_Process **sim_link_process;
#else
	Sim_Link_Process sim_link_process;
#endif
};
#endif
