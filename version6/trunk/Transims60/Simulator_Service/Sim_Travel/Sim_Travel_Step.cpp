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
//	Sim_Travel_Step -- Initialize
//---------------------------------------------------------

void Sim_Travel_Step::Initialize (void)
{
#ifdef THREADS
	num_threads = sim->Num_Threads ();
	sim_travel_process = new Sim_Travel_Process * [num_threads];

	if (num_threads > 1) {
		travel_queue.Max_Records (200 + 100 * num_threads);

		for (int i=0; i < num_threads; i++) {
			sim_travel_process [i] = new Sim_Travel_Process (travel_queue);
			threads.push_back (thread (*(sim_travel_process [i])));
		}
		save_results.Initialize (this);
		threads.push_back (thread (save_results));

		travel_queue.Start_Work ();
	} else {
		*sim_travel_process = new Sim_Travel_Process ();
	}
#else
	sim_travel_process.Initialize ();
#endif
}

//---------------------------------------------------------
//	Sim_Travel_Step -- Start_Processing
//---------------------------------------------------------

int Sim_Travel_Step::Start_Processing (void)
{
	int num_trips = 0;
	Sim_Travel_Itr itr;

#ifdef THREADS
	if (num_threads > 1) {
		travel_queue.Start_Work ();
		for (itr = sim->sim_travel_array.begin (); itr != sim->sim_travel_array.end (); itr++) {
			if (itr->sim_plan_ptr != 0) {
				sim->Active (true);
				num_trips++;
				if (itr->Next_Event () <= sim->time_step && itr->Status () <= 0) {
					travel_queue.Put_Work (&(*itr));
				}
			}
		}
		travel_queue.Complete_Work ();
	} else {
		for (itr = sim->sim_travel_array.begin (); itr != sim->sim_travel_array.end (); itr++) {
			if (itr->sim_plan_ptr != 0) {
				sim->Active (true);
				num_trips++;
				if (itr->Next_Event () <= sim->time_step && itr->Status () <= 0) {
					(*sim_travel_process)->Travel_Processing (&(*itr));
				}
			}
		}
	}
#else
	for (itr = sim->sim_travel_array.begin (); itr != sim->sim_travel_array.end (); itr++) {
		if (itr->sim_plan_ptr != 0) {
			if (itr->Status () != 0) sim->Active (true);

			if (itr->Status () <= 0) {
				num_trips++;
				if (itr->Next_Event () <= sim->time_step) {
					sim_travel_process.Travel_Processing (&(*itr));
				}
			}
		}
	}
#endif
	return (num_trips);
}

//---------------------------------------------------------
//	Sim_Travel_Step -- Stop_Processing
//---------------------------------------------------------

void Sim_Travel_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_travel_process != 0) {
		if (num_threads > 1) {
			travel_queue.End_of_Work ();
			threads.Join_All ();
		}
		for (int i=0; i < num_threads; i++) {
			delete sim_travel_process [i];
			sim_travel_process [i] = 0;
		}
		delete sim_travel_process;
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

#ifdef THREADS
//---------------------------------------------------------
//	Save_Results -- operator
//---------------------------------------------------------

void Sim_Travel_Step::Save_Results::operator()()
{
	Sim_Travel_Ptr sim_travel_ptr;

	for (;;) {
		sim_travel_ptr = ptr->travel_queue.Get_Result ();
		if (!sim_travel_ptr) break;

		if (sim_travel_ptr->Status () == 1) {
			Sim_Leg_Itr leg_itr = sim_travel_ptr->sim_plan_ptr->begin ();
			Sim_Dir_Ptr sim_dir_ptr = &sim->sim_dir_array [(++leg_itr)->Index ()];
			sim_dir_ptr->Load_Queue (sim_travel_ptr->Traveler ());
		}
		ptr->travel_queue.Result_Done ();
	}
}
#endif
