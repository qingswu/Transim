//*********************************************************
//	Sim_Plan_Step.cpp - read and process travel plans
//*********************************************************

#include "Sim_Plan_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Plan_Step -- constructor
//---------------------------------------------------------

Sim_Plan_Step::Sim_Plan_Step (void) : Static_Service ()
{
	num_files = first_num = -1;
	first = true;
	stat = false;
#ifdef THREADS
	num_threads = 0;
	sim_plan_process = 0;
#endif
}

//---------------------------------------------------------
//	Sim_Plan_Step -- destructor
//---------------------------------------------------------

Sim_Plan_Step::~Sim_Plan_Step (void)
{
	Stop_Processing ();
}

//---------------------------------------------------------
//	Sim_Plan_Step -- Initialize
//---------------------------------------------------------

void Sim_Plan_Step::Initialize (void)
{
	int num_pools = 1;
	num_files = first_num = -1;
	first = true;
	stat = false;

#ifdef THREADS
	//---- create processing threads ----

	num_threads = (sim->read_all_flag) ? 1 : sim->Num_Threads ();

	if (num_threads > 20) num_threads--;
	num_pools = num_threads;

	sim_plan_process = new Sim_Plan_Process * [num_threads];

	if (num_threads > 1) {
		trip_queue.Max_Records (200 + 50 * num_threads);

		for (int i=0; i < num_threads; i++) {
			sim_plan_process [i] = new Sim_Plan_Process (&trip_queue, i);
			threads.push_back (thread (ref (*(sim_plan_process [i]))));
		}
		save_results.Initialize (this);
		threads.push_back (thread (save_results));
	} else {
		*sim_plan_process = new Sim_Plan_Process ();
	}
#endif

	//---- initialize leg pools ----

	if (!sim->read_all_flag) {
		Sim_Leg_Pool sim_leg_pool;
		sim->sim_leg_array.assign (num_pools, sim_leg_pool);
	}
}

//---------------------------------------------------------
//	Sim_Plan_Step -- Start_Processing
//---------------------------------------------------------

bool Sim_Plan_Step::Start_Processing (void)
{
	if (!stat) return (stat);

	Leg_Pool_Itr pool_itr;

	for (pool_itr = sim->sim_leg_array.begin (); pool_itr != sim->sim_leg_array.end (); pool_itr++) {
		pool_itr->Check_Free ();
	}
#ifdef THREADS
	if (num_threads > 1) {
		trip_queue.Start_Work ();

		Read_Plans ();

		trip_queue.Complete_Work ();
		return (stat);
	}
#endif
	return (Read_Plans ());
}

//---------------------------------------------------------
//	Stop_Processing
//---------------------------------------------------------

void Sim_Plan_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_plan_process != 0) {
		if (num_threads > 1) {
			trip_queue.End_of_Work ();
			threads.Join_All ();
		}
		for (int i=0; i < num_threads; i++) {
			delete sim_plan_process [i];
			sim_plan_process [i] = 0;
		}
		delete [] sim_plan_process;
		sim_plan_process = 0;
	}
#endif
}

#ifdef THREADS
//---------------------------------------------------------
//	Save_Results -- operator
//---------------------------------------------------------

void Sim_Plan_Step::Save_Results::operator()()
{
	Sim_Trip_Ptr sim_trip_ptr;

	for (;;) {
		sim_trip_ptr = step_ptr->trip_queue.Get_Result ();
		if (!sim_trip_ptr) break;

		step_ptr->Sim_Plan_Result (sim_trip_ptr);
		step_ptr->trip_queue.Finished ();
	}
}
#endif
