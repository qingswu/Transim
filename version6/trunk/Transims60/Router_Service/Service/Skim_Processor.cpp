//*********************************************************
//	Skim_Processor.cpp - skim builder thread manager
//*********************************************************

#include "Skim_Processor.hpp"

//---------------------------------------------------------
//	Skim_Processor -- Initialize
//---------------------------------------------------------

void Skim_Processor::Initialize (Router_Service *_exe)
{
	exe = _exe;
	if (exe == 0) return;
#ifdef THREADS
	path_builder = 0;
	skim_queue = 0;

	num_builders = num_threads = exe->Num_Threads ();
	if (num_builders > 4) num_builders--;

	path_builder = new Path_Builder * [num_builders];

	//---- create path builders ----

	if (num_threads > 1) {
		skim_queue = new Skim_Queue (100 + 20 * num_builders);

		for (int i=0; i < num_builders; i++) {
			path_builder [i] = new Path_Builder (skim_queue, exe);
		}
		save_results.Initialize (this);
	} else {
		*path_builder = new Path_Builder (exe);
	}
#else
	num_builders = num_threads = 1;
	path_builder.Initialize (exe);
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- destructor
//---------------------------------------------------------

Skim_Processor::~Skim_Processor (void)
{
#ifdef THREADS
	if (exe && num_threads > 1) {
		if (skim_queue != 0) {
			delete skim_queue;
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
//	Skim_Processor -- Start_Processing
//---------------------------------------------------------

void Skim_Processor::Start_Processing (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		skim_queue->Start_Work ();

		//---- create the path threads ----

		for (int i=0; i < num_builders; i++) {
			threads.push_back (thread (ref (*(path_builder [i]))));
		}
		threads.push_back (thread (save_results));
	}
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Skim_Build
//---------------------------------------------------------

void Skim_Processor::Skim_Build (One_To_Many *ptr) 
{ 
#ifdef THREADS
	if (num_threads > 1) {
		skim_queue->Put_Work (ptr);
	} else {
		(*path_builder)->Skim_Build (ptr);
		exe->Save_Skims (ptr);
	}
#else
	path_builder.Skim_Build (ptr);
	exe->Save_Skims (ptr);
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Stop_Processing
//---------------------------------------------------------

void Skim_Processor::Stop_Processing (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		skim_queue->End_of_Work ();
		threads.Join_All ();
	}
#endif
}

//---------------------------------------------------------
//	Skim_Processor -- Save_Flows
//---------------------------------------------------------

void Skim_Processor::Save_Flows (void)
{
#ifdef THREADS
	if (num_threads > 1) {
		skim_queue->Complete_Work ();
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

void Skim_Processor::Save_Results::operator()()
{
	One_To_Many *skim_ptr;

	while ((skim_ptr = ptr->skim_queue->Get_Result ()) != 0) {
		ptr->exe->Save_Skims (skim_ptr);
		ptr->skim_queue->Finished ();
	}
}
#endif
