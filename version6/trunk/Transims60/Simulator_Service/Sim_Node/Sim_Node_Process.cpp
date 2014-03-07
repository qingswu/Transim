//*********************************************************
//	Sim_Node_Process.cpp - simulate links entering a node
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Node_Process -- constructor
//---------------------------------------------------------

Sim_Node_Process::Sim_Node_Process (void) : Static_Service ()
{
	num_vehicles = num_waiting = 0;
	stats.Clear_Statistics ();
}

//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Node_Process::operator()()
{
	int node;

	while (sim->node_barrier.Go (node)) {
		node = sim->node_list [node];
		Node_Processing (node);
		sim->node_barrier.Result ();
	}
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Node_Process::Initialize (void)
{
	num_vehicles = num_waiting = 0;
}
