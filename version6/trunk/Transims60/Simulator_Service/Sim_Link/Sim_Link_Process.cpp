//*********************************************************
//	Sim_Link_Process.cpp - simulate directional links
//*********************************************************

#include "Sim_Link_Process.hpp"

//---------------------------------------------------------
//	Sim_Link_Process -- constructor
//---------------------------------------------------------

Sim_Link_Process::Sim_Link_Process (void) : Static_Service ()
{
	id = 1;
	num_pce = num_vehicles = num_waiting = 0;
	stats.Clear_Statistics ();
}

#ifdef THREADS

Sim_Link_Process::Sim_Link_Process (Work_Queue *queue, int _id) : Static_Service ()
{
	link_queue = queue;
	id = _id + 1; 
	num_pce = num_vehicles = num_waiting = 0;
	stats.Clear_Statistics ();
}

//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Link_Process::operator()()
{
	int link;

	for (;;) {
		link = link_queue->Get ();
		if (link < 0) break;
		Link_Processing (link);
		link_queue->Finished ();
	}
}
#endif
