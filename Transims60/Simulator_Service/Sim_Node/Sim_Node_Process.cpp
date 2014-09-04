//*********************************************************
//	Sim_Node_Process.cpp - simulate approach links
//*********************************************************

#include "Sim_Node_Process.hpp"

//---------------------------------------------------------
//	Sim_Node_Process -- constructor
//---------------------------------------------------------

Sim_Node_Process::Sim_Node_Process (void) : Static_Service ()
{
	id = 1;
	num_pce = num_vehicles = num_waiting = 0;
	stats.Clear_Statistics ();
}

#ifdef THREADS

Sim_Node_Process::Sim_Node_Process (Work_Queue *queue, int _id) : Static_Service ()
{
	node_queue = queue;
	id = _id + 1; 
	num_pce = num_vehicles = num_waiting = 0;
	stats.Clear_Statistics ();
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
