//*********************************************************
//	Part_Processor.cpp - partition processing thread
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Part_Processor constructor
//---------------------------------------------------------

Router::Part_Processor::Part_Processor (void)
{
	exe = 0;
	num_processors = num_path_builders = 0;
	plan_process = 0;

#ifdef THREADS
	trip_queue = 0;
	part_thread = 0;
#endif
}

//---------------------------------------------------------
//	Part_Processor destructor
//---------------------------------------------------------

Router::Part_Processor::~Part_Processor (void)
{
	if (plan_process != 0) {
		delete plan_process;
	}
#ifdef THREADS
	if (trip_queue != 0) {
		for (int i=0; i < num_processors; i++) {
			if (trip_queue [i] != 0) delete trip_queue [i];
		}
		delete [] trip_queue;
	}
	if (part_thread != 0) {
		for (int i=0; i < num_processors; i++) {
			if (part_thread [i] != 0) delete part_thread [i];
		}
		delete part_thread;
	}
#endif
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

bool Router::Part_Processor::Initialize (Router *_exe)
{
	exe = _exe;
	if (exe == 0) return (false);

	num_path_builders = exe->Num_Threads ();

#ifdef THREADS

	//---- allocate threads ----

	if (exe->Num_Threads () > 1) {
		if (exe->Memory_Flag ()) {
			num_processors = 1;
			num_path_builders = exe->Num_Threads ();
		} else if (exe->Num_Partitions () < 2) {
			num_processors = 1;
			if (exe->Num_Threads () > 2) {
				if (exe->Num_Threads () > 4) {
					num_path_builders = exe->Num_Threads () - 1;
				} else {
					num_path_builders = exe->Num_Threads ();
				}
			} else {
				num_path_builders = 2;
			}
		} else if (exe->Num_Threads () >= exe->Num_Partitions ()) {
			num_processors = exe->Num_Partitions ();
			num_path_builders = exe->Num_Threads () / num_processors;
			if (num_path_builders < 2) {
				if ((num_processors % 2) == 0) {
					num_processors /= 2;
					num_path_builders = 2;
				} else {
					num_path_builders = 1;
				}
			}
		} else if (exe->trip_set_flag) {
			num_processors = exe->Num_Threads ();
			num_path_builders = 1;
		} else if (exe->Num_Threads () > 4) {
			num_processors = exe->Num_Threads () / 3;
			if (num_processors < 2) num_processors = 2;
			num_path_builders = exe->Num_Threads () / num_processors;
			if (num_path_builders < 2) num_path_builders = 2;
		} else {
			num_processors = 1;
			num_path_builders = exe->Num_Threads ();
		}
		exe->Write (2, "Number of File Partition Processors = ") << num_processors;
		exe->Write (1, "Number of Path Builders per Process = ") << num_path_builders;
		exe->Write (1);
	} else {
		num_processors = 1;
		num_path_builders = 1;
	}
	exe->Sub_Threads (num_path_builders);

	//---- create processing processors ----

	if (num_processors > 1) {
		int i, j;
		part_thread = new Part_Thread * [num_processors];

		for (i=0; i < num_processors; i++) {
			part_thread [i] = new Part_Thread (i, this);
		}
		if (exe->trip_gap_map_flag) {
			exe->trip_gap_map_array.Initialize (exe->Num_Partitions ());
		}
		if (exe->trip_flag && !exe->trip_set_flag) {
			for (i=j=0; i < exe->Num_Partitions (); i++, j++) {
				if (j == num_processors) j = 0;
				partition_map.push_back (j);
			}
			trip_queue = new Trip_Queue * [num_processors];

			for (i=0; i < num_processors; i++) {
				trip_queue [i] = new Trip_Queue ();
			}
			return (false);
		}
		return (true);
	} 
#endif
	exe->Sub_Threads (num_path_builders);
	plan_process = new Plan_Processor (exe);
	return (false);
}

//---------------------------------------------------------
//	Part_Processor -- Read_Trips
//---------------------------------------------------------

void Router::Part_Processor::Read_Trips (void)
{
	int p;

#ifdef THREADS
	if (num_processors > 1) {
		if (exe->thread_flag) {
			partition_queue.Reset ();

			for (p=0; p < exe->num_file_sets; p++) {
				partition_queue.Put (p);
			}
			partition_queue.End_of_Queue ();

			for (p=0; p < num_processors; p++) {
				threads.push_back (thread (ref (*(part_thread [p]))));
			}
			threads.Join_All ();
		} else {
			for (p=0; p < num_processors; p++) {
				threads.push_back (thread (ref (*(part_thread [p]))));
			}
			exe->Read_Trips (0);

			for (p=0; p < num_processors; p++) {
				trip_queue [p]->Exit_Queue ();
			}
			threads.Join_All ();
		}
		if (exe->Flow_Updates () || exe->Time_Updates ()) {
			for (p=0; p < num_processors; p++) {
				part_thread [p]->plan_process->Save_Flows ();
			}
		}
		return;
	} else {
		plan_process->Start_Processing (true, true);
	}
#else
	plan_process->Start_Processing ();
#endif

	if (exe->Memory_Flag ()) {
		exe->Memory_Loop (0, plan_process);
	} else {
		if (exe->trip_set_flag) {
			for (p=0; p < exe->num_file_sets; p++) {
				exe->Read_Trips (p, plan_process);
			}
		} else {
			exe->Read_Trips (0, plan_process);
		}
	}
	plan_process->Stop_Processing (exe->Flow_Updates () || exe->Time_Updates ());
}

//---------------------------------------------------------
//	Part_Processor -- Copy_Plans
//---------------------------------------------------------

void Router::Part_Processor::Copy_Plans (void)
{
	int p;

#ifdef THREADS
	if (num_processors > 1) {
		for (p=0; p < exe->num_file_sets; p++) {
			partition_queue.Put (p);
		}
		partition_queue.End_of_Queue ();

		for (p=0; p < num_processors; p++) {
			threads.push_back (thread (ref (*(part_thread [p]))));
		}
		threads.Join_All ();

		if (exe->Flow_Updates () || exe->Time_Updates ()) {
			for (p=0; p < num_processors; p++) {
				part_thread [p]->plan_process->Save_Flows ();
			}
		}
		return;
	}
#endif
	plan_process->Start_Processing ();

	if (exe->Memory_Flag ()) {
        exe->Copy_Plans (0, plan_process);
	} else {
	    if (exe->new_set_flag) {
		    for (p=0; p < exe->num_file_sets; p++) {
			    exe->Copy_Plans (p, plan_process);
		    }
	    } else {
		    for (p=0; ; p++) {
			    if (!exe->plan_file->Open (p)) break;
			    if (exe->new_plan_flag) exe->new_plan_file->Open (p);
			    exe->Copy_Plans (p, plan_process);
		    }
	    }
	}
	plan_process->Stop_Processing ((exe->Flow_Updates () || exe->Time_Updates ()) && exe->System_File_Flag (NEW_PERFORMANCE));
}

//---------------------------------------------------------
//	Part_Processor -- Plan_Build
//---------------------------------------------------------

void Router::Part_Processor::Plan_Build (Plan_Ptr_Array *ptr_array, int partition, Plan_Processor *ptr)
{
#ifdef THREADS
	if (ptr == 0) {
		partition = partition_map [partition];
		trip_queue [partition]->Put (ptr_array);
		return;
	}
#endif
	partition = 0;
	ptr->Plan_Build (ptr_array);
}

//---------------------------------------------------------
//	Part_Processor -- Sum_Ridership
//---------------------------------------------------------

void Router::Part_Processor::Sum_Ridership (Plan_Data &plan, int part)
{
	if (!exe->rider_flag || part < 0) return;

#ifdef THREADS
	if (Thread_Flag ()) {
		if (part >= num_processors) return;
		part_thread [part]->line_array.Sum_Ridership (plan, true);
	} else {
		exe->line_array.Sum_Ridership (plan, true);
	}
#else
	exe->line_array.Sum_Ridership (plan, true);
#endif
}

//---------------------------------------------------------
//	Part_Processor -- Save_Riders
//---------------------------------------------------------

void Router::Part_Processor::Save_Riders (void)
{
#ifdef THREADS
	if (Thread_Flag () && exe->rider_flag) {
		int p, l, s, r;
		Line_Array *ptr;

		for (p=0; p < num_processors; p++) {
			ptr = &part_thread [p]->line_array;

			Line_Itr line_itr;
			Line_Stop_Itr stop_itr;
			Line_Run_Itr run_itr;

			Line_Data *line_ptr;
			Line_Stop *stop_ptr;
			Line_Run *run_ptr;

			for (l=0, line_itr = ptr->begin (); line_itr != ptr->end (); line_itr++, l++) {
				line_ptr = &exe->line_array [l];

				for (s=0, stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++, s++) {
					stop_ptr = &line_ptr->at (s);

					for (r=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, r++) {
						run_ptr = &stop_ptr->at (r);

						run_ptr->Board (run_ptr->Board () + run_itr->Board ());
						run_ptr->Alight (run_ptr->Alight () + run_itr->Alight ());
						run_ptr->Load (run_ptr->Load () + run_itr->Load ());
					}
				}
			}
		}
	}
#endif
}

#ifdef THREADS
//---------------------------------------------------------
//	Part_Thread constructor
//---------------------------------------------------------

Router::Part_Processor::Part_Thread::Part_Thread (int num, Part_Processor *_ptr)
{
	ptr = _ptr;
	number = num;
	plan_process = 0;

	if (ptr) {
		plan_process = new Plan_Processor (ptr->exe);
	}
	if (ptr->Thread_Flag ()) {
		if (ptr->exe->rider_flag) {
			line_array = ptr->exe->line_array;
		}
	}
}

//---------------------------------------------------------
//	Part_Thread operator
//---------------------------------------------------------

void Router::Part_Processor::Part_Thread::operator()()
{
	int part;
	plan_process->Start_Processing ();

	if (ptr->exe->thread_flag) {
		while (ptr->partition_queue.Get (part)) {
			if (ptr->exe->trip_flag) {
				if (ptr->exe->Memory_Flag ()) {
					ptr->exe->Memory_Loop (part, plan_process);
				} else {
					ptr->exe->Read_Trips (part, plan_process);
				}
			} else {
				ptr->exe->Copy_Plans (part, plan_process);
			}		
		}
	} else {
		Plan_Ptr_Array *plan_ptr_array;
		Trip_Queue *queue = ptr->trip_queue [number];
		queue->Start_Work ();

		while (queue->Get (plan_ptr_array)) {
			plan_process->Plan_Build (plan_ptr_array);
		}
	}
	plan_process->Stop_Processing ();
}
#endif
