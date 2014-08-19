//*********************************************************
//	Sim_Travel_Step.hpp - travel event manager
//*********************************************************

#ifndef SIM_TRAVEL_STEP_HPP
#define SIM_TRAVEL_STEP_HPP

#include "Sim_Travel_Process.hpp"

//---------------------------------------------------------
//	Sim_Travel_Step - process travel events
//---------------------------------------------------------

class SYSLIB_API Sim_Travel_Step : public Static_Service
{
public:
	Sim_Travel_Step (void);
	~Sim_Travel_Step (void);

	void Initialize (void);
	void Start_Processing (void);
	void Stop_Processing (void);

	void Add_Statistics (Sim_Statistics &_stats);

private:
#ifdef THREADS
	int num_threads;
	Threads threads;
	Work_Queue travel_queue;
	Sim_Travel_Process **sim_travel_process;
#else
	Sim_Travel_Process sim_travel_process;
#endif
};
#endif
