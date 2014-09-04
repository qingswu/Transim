//*********************************************************
//	Sim_Node_Step.hpp - simulate approach links
//*********************************************************

#ifndef SIM_NODE_STEP_HPP
#define SIM_NODE_STEP_HPP

#include "Sim_Node_Process.hpp"

//---------------------------------------------------------
//	Sim_Node_Step - process traveler events
//---------------------------------------------------------

class SYSLIB_API Sim_Node_Step : public Static_Service
{
public:
	Sim_Node_Step (void);
	~Sim_Node_Step (void);

	void Initialize (void);
	void Start_Processing (void);
	void Stop_Processing (void);

	void Randomize_Nodes (void);

	void Add_Statistics (Sim_Statistics &_stats);

	int  Num_PCE (void)                 { return (num_pce); }
	int  Num_Vehicles (void)            { return (num_vehicles); }
	int  Num_Waiting (void)             { return (num_waiting); }

private:
	int num_pce, num_vehicles, num_waiting;
	Integers node_list;

#ifdef THREADS
	int num_threads;
	Threads threads;
	Work_Queue node_queue;
	Sim_Node_Process **sim_node_process;
#else
	Sim_Node_Process sim_node_process;
#endif
};
#endif
