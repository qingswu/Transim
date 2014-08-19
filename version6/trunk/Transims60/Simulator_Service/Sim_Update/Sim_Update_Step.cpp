//*********************************************************
//	Sim_Update_Step.cpp - update simulation environment
//*********************************************************

#include "Sim_Update_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Update_Step -- constructor
//---------------------------------------------------------

Sim_Update_Step::Sim_Update_Step (void) : Static_Service ()
{
	num_updates = 0;

#ifdef THREADS
	num_threads = 0;
	sim_update_process = 0;
#endif
}

//---------------------------------------------------------
//	Sim_Update_Step -- destructor
//---------------------------------------------------------

Sim_Update_Step::~Sim_Update_Step (void)
{
	Stop_Processing ();
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Update_Step::Initialize (void)
{
	update_array.push_back (new Sim_Control_Update ());
	update_array.push_back (new Sim_Network_Update ());
	update_array.push_back (new Sim_Transit_Update ());

	num_updates = (int) update_array.size ();

#ifdef THREADS
	update_queue.Max_Records (num_updates);

	num_threads = sim->Num_Threads ();
	if (num_threads == 1) return;
	if (num_threads > num_updates) num_threads = num_updates;

	sim_update_process = new Sim_Update_Process * [num_threads];

	for (int i=0; i < num_threads; i++) {
		sim_update_process [i] = new Sim_Update_Process (this);
		threads.push_back (thread (ref (*(sim_update_process [i]))));
	}
#endif
}

//---------------------------------------------------------
//	Start_Processing
//---------------------------------------------------------

void Sim_Update_Step::Start_Processing (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		int i;
		update_queue.Start_Work ();

		for (i=0; i < num_updates; i++) {
			update_queue.Put (i);
		}
		update_queue.Complete_Work ();
		return;
	}
#endif
	Update_Itr itr;

	for (itr = update_array.begin (); itr != update_array.end (); itr++) {
		(*itr)->Update_Check ();
	}
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

void Sim_Update_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_update_process != 0) {
		if (num_threads > 1) {
			update_queue.Exit_Queue ();
			threads.Join_All ();
		}
		for (int i=0; i < num_threads; i++) {
			delete sim_update_process [i];
			sim_update_process [i] = 0;
		}
		delete [] sim_update_process;
		sim_update_process = 0;
	}
#endif
	Update_Itr itr;

	for (itr = update_array.begin (); itr != update_array.end (); itr++) {
		delete (*itr);
	}
	update_array.clear ();
}

#ifdef THREADS
//---------------------------------------------------------
//	operator
//---------------------------------------------------------

void Sim_Update_Step::Sim_Update_Process::operator()()
{
	int num;

	for (;;) {
		num = step_ptr->update_queue.Get ();
		if (num < 0) break;

		Sim_Update_Data *update = step_ptr->update_array [num];

		update->Update_Check ();

		step_ptr->update_queue.Finished ();
	}
}
#endif
