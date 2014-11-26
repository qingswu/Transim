//*********************************************************
//	Plan_Processor.cpp - path builder thread manager
//*********************************************************

#include "Plan_Processor.hpp"

//---------------------------------------------------------
//	Plan_Processor -- Initialize
//---------------------------------------------------------

void Plan_Processor::Initialize (Router_Service *_exe)
{
	exe = _exe;
	if (exe == 0) return;
#ifdef THREADS
	path_builder = 0;
	plan_queue = 0;
	plan_ptr_queue = 0;

	num_builders = num_threads = exe->Sub_Threads ();

	path_builder = new Path_Builder * [num_builders];

	//---- create path builders ----

	if (num_threads > 1) {
		if (exe->Memory_Flag ()) {
			plan_ptr_queue = new Plan_Ptr_Queue (100 + 50 * num_builders);

			for (int i=0; i < num_builders; i++) {
				path_builder [i] = new Path_Builder (plan_ptr_queue, exe);
			}
		} else {
			plan_queue = new Plan_Queue (100 + 50 * num_builders);

			for (int i=0; i < num_builders; i++) {
				path_builder [i] = new Path_Builder (plan_queue, exe);
			}
			save_results.Initialize (this);
		}
	} else {
		*path_builder = new Path_Builder (exe);
	}
#else
	num_builders = num_threads = 1;
	path_builder.Initialize (exe);
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- destructor
//---------------------------------------------------------

Plan_Processor::~Plan_Processor (void)
{
#ifdef THREADS
	if (exe && num_threads > 1) {
		if (plan_queue != 0) {
			delete plan_queue;
		}
		if (plan_ptr_queue != 0) {
			delete plan_ptr_queue;
		}
		if (path_builder != 0) {
			for (int i=0; i < num_builders; i++) {
				if (path_builder [i] != 0) {
					delete path_builder [i];
					path_builder [i] = 0;
				}
			}
			delete path_builder;
		}
	}
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Start_Processing
//---------------------------------------------------------

void Plan_Processor::Start_Processing (bool update_times, bool zero_flows)
{
#ifdef THREADS
	if (num_threads > 1) {
		if (exe->Memory_Flag ()) {
			plan_ptr_queue->Start_Work ();

			//---- create the path threads ----

			threads.clear ();
			for (int i=0; i < num_builders; i++) {
				Path_Builder *ptr = path_builder [i];

				ptr->Update_Times (update_times);
				ptr->Zero_Flows (zero_flows);
				ptr->Reset_Skim_Gap ();

				threads.push_back (thread (ref (*ptr)));
			}
		} else {
			plan_queue->Start_Work ();

			//---- create the path threads ----

			threads.clear ();
			for (int i=0; i < num_builders; i++) {
				Path_Builder *ptr = path_builder [i];

				ptr->Update_Times (update_times);
				ptr->Zero_Flows (zero_flows);
				ptr->Reset_Skim_Gap ();

				threads.push_back (thread (ref (*ptr)));
			}
			threads.push_back (thread (save_results));
		}
	} else {
		if (zero_flows) (*path_builder)->Zero_Flows ();
		(*path_builder)->Reset_Skim_Gap ();
	}
#else
	update_times = false;
	if (zero_flows) path_builder.Zero_Flows ();
	path_builder.Reset_Skim_Gap ();
#endif
	exe->Reset_Skim_Gap ();
}

//---------------------------------------------------------
//	Plan_Processor -- Plan_Build
//---------------------------------------------------------

void Plan_Processor::Plan_Build (Plan_Ptr_Array *array_ptr) 
{ 
#ifdef THREADS
	if (num_threads > 1) {
		if (exe->Memory_Flag ()) {
			plan_ptr_queue->Put (array_ptr);
		} else {
			plan_queue->Put_Work (array_ptr);
		}
	} else {
		(*path_builder)->Array_Processing (array_ptr);
		exe->Save_Plans (array_ptr);
	}
#else
	path_builder.Array_Processing (array_ptr);
	exe->Save_Plans (array_ptr);
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Stop_Processing
//---------------------------------------------------------

void Plan_Processor::Stop_Processing (bool save_flows)
{
#ifdef THREADS
	if (num_threads > 1) {
		if (exe->Memory_Flag ()) {
			plan_ptr_queue->End_of_Work ();
		} else {
			plan_queue->End_of_Work ();
		}
		threads.Join_All ();

		if (save_flows) {
			for (int i=0; i < num_builders; i++) {
				path_builder [i]->Save_Flows ();
			}
		}
	} else {
		(*path_builder)->Save_Skim_Gap ();
	}
#else
	path_builder.Save_Skim_Gap ();
#endif
	save_flows = false;
}

//---------------------------------------------------------
//	Plan_Processor -- Save_Flows
//---------------------------------------------------------

void Plan_Processor::Save_Flows (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		if (exe->Memory_Flag ()) {
			plan_ptr_queue->Complete_Work ();
		} else {
			plan_queue->Complete_Work ();
		}
		for (int i=0; i < num_builders; i++) {
			path_builder [i]->Save_Flows ();
		}
	} else {
		(*path_builder)->Save_Flows ();
	}
#endif
}

#ifdef THREADS
//---------------------------------------------------------
//	Save_Results -- operator
//---------------------------------------------------------

void Plan_Processor::Save_Results::operator()()
{
	Plan_Ptr_Array *array_ptr;

	while ((array_ptr = ptr->plan_queue->Get_Result ()) != 0) {
		ptr->exe->Save_Plans (array_ptr);
		ptr->plan_queue->Finished ();
	}
}
#endif
