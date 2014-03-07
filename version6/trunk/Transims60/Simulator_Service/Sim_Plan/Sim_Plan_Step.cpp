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
//	Sim_Plan_Step -- Stop_Processing
//---------------------------------------------------------

void Sim_Plan_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_plan_process != 0) {
		if (num_threads > 1) {
			plan_queue.End_of_Work ();
			threads.Join_All ();
		}
		for (int i=0; i < num_threads; i++) {
			delete sim_plan_process [i];
			sim_plan_process [i] = 0;
		}
		delete sim_plan_process;
		sim_plan_process = 0;
	}
#endif
}

//---------------------------------------------------------
//	Sim_Plan_Step -- Initialize
//---------------------------------------------------------

void Sim_Plan_Step::Initialize (void)
{
	num_files = first_num = -1;
	first = true;
	stat = false;

#ifdef THREADS
	//---- create processing threads ----

	num_threads = sim->Num_Threads ();
	sim_plan_process = new Sim_Plan_Process * [num_threads];

	if (num_threads > 1) {
		plan_queue.Max_Records (100 + 50 * num_threads);

		for (int i=0; i < num_threads; i++) {
			sim_plan_process [i] = new Sim_Plan_Process (plan_queue);
			threads.push_back (thread (*(sim_plan_process [i])));
		}
		save_results.Initialize (this);
		threads.push_back (thread (save_results));

		plan_queue.Start_Work ();
	} else {
		*sim_plan_process = new Sim_Plan_Process ();
	}
#else
	sim_plan_process.Initialize ();
#endif
}

#ifdef THREADS
//---------------------------------------------------------
//	Save_Results -- operator
//---------------------------------------------------------

void Sim_Plan_Step::Save_Results::operator()()
{
	Sim_Travel_Ptr sim_travel_ptr;

	for (;;) {
		sim_travel_ptr = ptr->plan_queue.Get_Result ();
		if (!sim_travel_ptr) break;

		ptr->Sim_Plan_Result (sim_travel_ptr);
		ptr->plan_queue.Result_Done ();
	}
}
#endif
