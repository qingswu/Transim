//*********************************************************
//	Sim_Node_Process.cpp - simulate approach links
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Node_Process -- constructor
//---------------------------------------------------------

Sim_Node_Process::Sim_Node_Process (void) : Static_Service ()
{
}

#ifdef THREADS

Sim_Node_Process::Sim_Node_Process (Work_Queue *queue, int id) : Static_Service ()
{
	node_queue = queue;
	Initialize (id);
}

//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Node_Process::operator()()
{
	int node;

	for (;;) {
		node = node_queue->Get ();
		if (node < 0) break;
		Node_Processing (node);
		node_queue->Finished ();
	}
}
#endif

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Node_Process::Initialize (int _id)
{
	id = _id + 1; 
	num_pce = num_vehicles = num_waiting = 0;
	stats.Clear_Statistics ();
	random.Seed (sim->Random_Seed () * id);
}
