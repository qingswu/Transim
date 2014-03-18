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
	num_threads = 0;
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

void Sim_Update_Step::Initialize (void)
{
	num_threads = sim->Num_Threads ();

	update_array.push_back (new Sim_Control_Update ());
	update_array.push_back (new Sim_Network_Update ());
	update_array.push_back (new Sim_Transit_Update ());
	
	//---- create update threads ----

#ifdef THREADS
	if (num_threads > 1) {
		int num_barrier = (int) update_array.size ();

		update_barrier.Num_Barriers (num_barrier);

		Update_Itr update_itr;

		for (update_itr = update_array.begin (); update_itr != update_array.end (); update_itr++) {
			threads.push_back (thread (*(*update_itr)));
		}
	}
#endif
}

//---------------------------------------------------------
//	Start_Processing
//---------------------------------------------------------

void Sim_Update_Step::Start_Processing (void)
{
	if (num_threads > 1) {
		update_barrier.Start ();
	} else {
		Update_Itr itr;

		for (itr = update_array.begin (); itr != update_array.end (); itr++) {
			(*itr)->Update_Check ();
		}
	}
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

void Sim_Update_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		if (threads.size () > 0) {
			update_barrier.Exit ();
			threads.Join_All ();
		}
		num_threads = 0;
	}
#endif
}
