//*********************************************************
//	Sim_Travel_Process.cpp - simulate travel events
//*********************************************************

#include "Sim_Travel_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Travel_Process::Initialize (void)
{
	stats.Clear_Statistics ();
}

#ifdef THREADS
//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Travel_Process::operator()()
{
	int number;
	Sim_Travel_Ptr sim_travel_ptr;

	for (;;) {
		sim_travel_ptr = travel_queue->Get_Work (number);
		if (sim_travel_ptr == 0) break;

		Travel_Processing (sim_travel_ptr);

		if (!travel_queue->Put_Result (sim_travel_ptr, number)) break;
	}
}
#endif
