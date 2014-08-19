//*********************************************************
//	Sim_Travel_Process.cpp - travel event manager
//*********************************************************

#include "Sim_Travel_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Travel_Process -- constructor
//---------------------------------------------------------

Sim_Travel_Process::Sim_Travel_Process (void) : Static_Service ()
{
	id = 1;
	stats.Clear_Statistics (); 
}

#ifdef THREADS

Sim_Travel_Process::Sim_Travel_Process (Work_Queue *queue, int _id) : Static_Service ()
{ 
	travel_queue = queue; 
	id = _id + 1;
	stats.Clear_Statistics (); 
}

//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Travel_Process::operator()()
{
	int number;
	Sim_Travel_Ptr sim_travel_ptr;

	for (;;) {
		number = travel_queue->Get ();
		if (number < 0) break;

		sim_travel_ptr = &sim->sim_travel_array [number];

		Travel_Processing (sim_travel_ptr);

		travel_queue->Finished ();
	}
}
#endif
