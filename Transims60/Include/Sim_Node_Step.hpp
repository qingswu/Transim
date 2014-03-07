//*********************************************************
//	Sim_Node_Step.hpp - simulate links entering nodes
//*********************************************************

#ifndef SIM_NODE_STEP_HPP
#define SIM_NODE_STEP_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"
#include "Static_Service.hpp"
#include "Sim_Node_Process.hpp"
#include "Sim_Statistics.hpp"

//---------------------------------------------------------
//	Sim_Node_Step - process traveler events
//---------------------------------------------------------

class SYSLIB_API Sim_Node_Step : public Static_Service
{
public:
	Sim_Node_Step (void);
	~Sim_Node_Step (void)              { Stop_Processing (); }

	void Initialize (void);
	void Start_Processing (void);
	void Stop_Processing (void);

	void Randomize_Nodes (void);

	void Add_Statistics (Sim_Statistics &_stats);

	int  Num_Vehicles (void)            { return (num_vehicles); }
	int  Num_Waiting (void)             { return (num_waiting); }

private:
	int num_vehicles, num_waiting;

#ifdef THREADS
	int num_threads;
	Threads threads;

	Sim_Node_Process **sim_node_process;
#else
	Sim_Node_Process sim_node_process;
#endif
};
#endif
