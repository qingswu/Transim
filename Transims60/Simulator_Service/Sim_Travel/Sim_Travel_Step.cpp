//*********************************************************
//	Sim_Travel_Step.cpp - travel event manager
//*********************************************************

#include "Sim_Travel_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Travel_Step -- constructor
//---------------------------------------------------------

Sim_Travel_Step::Sim_Travel_Step (void)
{
#ifdef THREADS
	num_threads = 0;
	sim_travel_process = 0;
#endif
}

//---------------------------------------------------------
//	Sim_Travel_Step -- destructor
//---------------------------------------------------------

Sim_Travel_Step::~Sim_Travel_Step (void)
{
	Stop_Processing ();
}

//---------------------------------------------------------
//	Sim_Travel_Step -- Initialize
//---------------------------------------------------------

void Sim_Travel_Step::Initialize (void)
{
#ifdef THREADS
	num_threads = sim->Num_Threads ();

	sim_travel_process = new Sim_Travel_Process * [num_threads];

	if (num_threads > 1) {
		travel_queue.Max_Records (200 + 20 * num_threads);

		for (int i=0; i < num_threads; i++) {
			sim_travel_process [i] = new Sim_Travel_Process (&travel_queue, i);
			threads.push_back (thread (ref (*(sim_travel_process [i]))));
		}
	} else {
		*sim_travel_process = new Sim_Travel_Process ();
	}
#endif
}

//---------------------------------------------------------
//	Sim_Travel_Step -- Start_Processing
//---------------------------------------------------------

void Sim_Travel_Step::Start_Processing (void)
{
	Sim_Travel_Itr itr;

#ifdef THREADS
	if (num_threads > 1) {
		int i;
#ifdef CHECK
		int count = 0;
#endif
		travel_queue.Start_Work ();

		for (i=0, itr = sim->sim_travel_array.begin (); itr != sim->sim_travel_array.end (); itr++, i++) {
			if (itr->Plan_Index () >= 0) {
				if (itr->Person () > 0 || itr->Passengers () > 0) sim->Active (true);
				if (itr->Next_Event () <= sim->time_step && itr->Status () <= OFF_NET_END) {
					travel_queue.Put (i);
#ifdef CHECK
					count++;
#endif
				}
			}
		}
		travel_queue.Complete_Work ();

#ifdef CHECK
		if (count != travel_queue.Total_Records ()) {
			sim->Write (1, String ("Travel Queue %d vs %d") % count % travel_queue.Total_Records ());
		}
#endif
	} else {
		for (itr = sim->sim_travel_array.begin (); itr != sim->sim_travel_array.end (); itr++) {
			if (itr->Plan_Index () >= 0) {
				if (itr->Person () > 0 || itr->Passengers () > 0) sim->Active (true);
				if (itr->Next_Event () <= sim->time_step && itr->Status () <= OFF_NET_END) {
					(*sim_travel_process)->Travel_Processing (&(*itr));
				}
			}
		}
	}
#else
	for (itr = sim->sim_travel_array.begin (); itr != sim->sim_travel_array.end (); itr++) {
		if (itr->Plan_Index () >= 0) {
			if (itr->Person () > 0 || itr->Passengers () > 0) sim->Active (true);
			if (itr->Next_Event () <= sim->time_step && itr->Status () <= OFF_NET_END) {
				sim_travel_process.Travel_Processing (&(*itr));
			}
		}
	}
#endif
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

void Sim_Travel_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_travel_process != 0) {
		if (num_threads > 1) {
			travel_queue.Exit_Queue ();
			threads.Join_All ();
		}
		for (int i=0; i < num_threads; i++) {
			delete sim_travel_process [i];
			sim_travel_process [i] = 0;
		}
		delete [] sim_travel_process;
		sim_travel_process = 0;
	}
#endif	
}

//---------------------------------------------------------
//	Add_Statistics
//---------------------------------------------------------

void Sim_Travel_Step::Add_Statistics (Sim_Statistics &stats)
{
#ifdef THREADS
	if (num_threads > 1) {
		for (int i=0; i < num_threads; i++) {
			stats.Add_Statistics (sim_travel_process [i]->Get_Statistics ());
		}
	} else {
		stats.Add_Statistics ((*sim_travel_process)->Get_Statistics ());
	}
#else
	stats.Add_Statistics (sim_travel_process.Get_Statistics ());
#endif
}
