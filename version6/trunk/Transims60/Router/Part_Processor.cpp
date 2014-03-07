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
#ifdef THREADS
	if (trip_queue != 0) {
		for (int i=0; i < num_processors; i++) {
			if (trip_queue [i] != 0) delete trip_queue [i];
		}
		delete trip_queue;
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
		if (exe->Num_Partitions () < 2) {
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
	plan_processor.Initialize (exe);
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
				trip_queue [p]->End_Queue ();
				trip_queue [p]->Exit_Queue ();
			}
			threads.Join_All ();
		}
		if (exe->Flow_Updates () || exe->Time_Updates ()) {
			for (p=0; p < num_processors; p++) {
				part_thread [p]->plan_processor->Save_Flows ();
			}
		}
		return;
	}

#endif
	plan_processor.Start_Processing ();

	if (exe->trip_set_flag) {
		for (p=0; p < exe->num_file_sets; p++) {
			exe->Read_Trips (p, &plan_processor);
		}
	} else {
		exe->Read_Trips (0, &plan_processor);
	}
	plan_processor.Stop_Processing ();

	if (exe->Flow_Updates () || exe->Time_Updates ()) {
		plan_processor.Save_Flows ();
	}
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
				part_thread [p]->plan_processor->Save_Flows ();
			}
		}
		return;
	}
#endif
	plan_processor.Start_Processing ();

	if (exe->plan_set_flag) {
		for (p=0; p < exe->num_file_sets; p++) {
			exe->Copy_Plans (p, &plan_processor);
		}
	} else {
		for (p=0; ; p++) {
			if (!exe->plan_file->Open (p)) break;
			if (exe->new_plan_flag) exe->new_plan_file->Open (p);
			exe->Copy_Plans (p, &plan_processor);
		}
	}
	plan_processor.Stop_Processing ();

	if ((exe->Flow_Updates () || exe->Time_Updates ()) && exe->System_File_Flag (NEW_LINK_DELAY)) {
		plan_processor.Save_Flows ();
	}
}

//---------------------------------------------------------
//	Part_Processor -- Plan_Build
//---------------------------------------------------------

void Router::Part_Processor::Plan_Build (int partition, Plan_Ptr_Array *plan_ptr_array)
{
#ifdef THREADS
	partition = partition_map [partition];
	trip_queue [partition]->Put (plan_ptr_array);
#else
	delete plan_ptr_array;
	partition = 0;
#endif
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
		part_thread [part]->line_array.Sum_Ridership (plan);
	} else {
		exe->line_array.Sum_Ridership (plan);
	}
#else
	exe->line_array.Sum_Ridership (plan);
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Save_Riders
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

//---------------------------------------------------------
//	Part_Processor -- Sum_Persons
//---------------------------------------------------------

void Router::Part_Processor::Sum_Persons (Plan_Data &plan, int part)
{
	if (!exe->link_person_flag || part < 0) return;

#ifdef THREADS
	if (Thread_Flag ()) {
		if (part >= num_processors) return;
		Sum_Link_Data (part_thread [part]->link_person_array, plan, true);
	} else {
		Sum_Link_Data (exe->link_person_array, plan, true);
	}
#else
	Sum_Link_Data (exe->link_person_array, plan, true);
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Save_Persons
//---------------------------------------------------------

void Router::Part_Processor::Save_Persons (void)
{
#ifdef THREADS
	if (Thread_Flag () && exe->link_person_flag) {
		for (int p=0; p < num_processors; p++) {
			exe->link_person_array.Add_Flow_Times (part_thread [p]->link_person_array);
		}
	}
#endif
}

//---------------------------------------------------------
//	Part_Processor -- Sum_Vehicles
//---------------------------------------------------------

void Router::Part_Processor::Sum_Vehicles (Plan_Data &plan, int part)
{
	if (!exe->link_vehicle_flag || part < 0) return;

#ifdef THREADS
	if (Thread_Flag ()) {
		if (part >= num_processors) return;
		Sum_Link_Data (part_thread [part]->link_vehicle_array, plan, false);
	} else {
		Sum_Link_Data (exe->link_vehicle_array, plan, false);
	}
#else
	Sum_Link_Data (exe->link_vehicle_array, plan, false);
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Save_Vehicles
//---------------------------------------------------------

void Router::Part_Processor::Save_Vehicles (void)
{
#ifdef THREADS
	if (Thread_Flag () && exe->link_vehicle_flag) {
		for (int p=0; p < num_processors; p++) {
			exe->link_vehicle_array.Add_Flow_Times (part_thread [p]->link_vehicle_array);
		}
	}
#endif
}

//---------------------------------------------------------
//	Plan_Processor -- Sum_Link_Data
//---------------------------------------------------------

void Router::Part_Processor::Sum_Link_Data (Flow_Time_Period_Array &array, Plan_Data &plan, bool person_flag)
{
	int index;
	Dtime time, time2;
	double factor, fac_len;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Plan_Leg_Itr leg_itr;
	Int_Map_Itr map_itr;
	Veh_Type_Data *veh_type_ptr;
	Flow_Time_Array *link_flow_ptr;
	Flow_Time_Data *flow_ptr;

	if (plan.Mode () == WAIT_MODE || plan.Mode () == WALK_MODE || plan.Mode () == BIKE_MODE || 
		plan.Mode () == TRANSIT_MODE || plan.Mode () == RIDE_MODE || plan.Mode () == OTHER_MODE) return;

	factor = 1.0;

	if (person_flag && exe->System_File_Flag (VEHICLE_TYPE) && plan.Veh_Type () > 0) {
		map_itr = exe->veh_type_map.find (plan.Veh_Type ());
		if (map_itr != exe->veh_type_map.end ()) {
			veh_type_ptr = &exe->veh_type_array [map_itr->second];
			factor = veh_type_ptr->Occupancy () / 100.0;
			if (factor <= 0.0) factor = 1.0;
		}
	}

	time = plan.Depart ();
	time2 = 0;

	for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++, time += time2) {
		time2 = leg_itr->Time ();

		if (leg_itr->Link_Type () && leg_itr->Mode () == DRIVE_MODE) {
			map_itr = exe->link_map.find (leg_itr->Link_ID ());
			if (map_itr == exe->link_map.end ()) continue;

			link_ptr = &exe->link_array [map_itr->second];

			if (leg_itr->Link_Dir () == 0) {
				index = link_ptr->AB_Dir ();
			} else {
				index = link_ptr->BA_Dir ();
			}
			dir_ptr = &exe->dir_array [index];

			if (leg_itr->Type () == USE_AB || leg_itr->Type () == USE_BA) {
				index = dir_ptr->Flow_Index ();
				if (index < 0) continue;
			}
			link_flow_ptr = array.Period_Ptr (time);
			if (link_flow_ptr == 0) continue;

			flow_ptr = &link_flow_ptr->at (index);

			fac_len = factor * leg_itr->Length () / link_ptr->Length ();

			flow_ptr->Add_Flow_Time (fac_len, leg_itr->Time ());
		}
	}
}

#ifdef THREADS
//---------------------------------------------------------
//	Part_Thread constructor
//---------------------------------------------------------

Router::Part_Processor::Part_Thread::Part_Thread (int num, Part_Processor *_ptr)
{
	ptr = _ptr;
	number = num;
	plan_processor = 0;

	if (ptr) {
		plan_processor = new Plan_Processor (ptr->exe);
	}
	if (ptr->Thread_Flag ()) {
		if (ptr->exe->rider_flag) {
			line_array = ptr->exe->line_array;
		}
		if (ptr->exe->link_person_flag) {
			link_person_array.Replicate (ptr->exe->link_person_array);
		}
		if (ptr->exe->link_vehicle_flag) {
			link_vehicle_array.Replicate (ptr->exe->link_vehicle_array);
		}
	}
}

//---------------------------------------------------------
//	Part_Thread operator
//---------------------------------------------------------

void Router::Part_Processor::Part_Thread::operator()()
{
	int part;
	plan_processor->Start_Processing ();

	if (ptr->exe->thread_flag) {
		while (ptr->partition_queue.Get (part)) {
			if (ptr->exe->trip_flag) {
				ptr->exe->Read_Trips (part, plan_processor);
			} else {
				ptr->exe->Copy_Plans (part, plan_processor);
			}		
		}
	} else {
		Plan_Ptr_Array *plan_ptr_array;
		Trip_Queue *queue = ptr->trip_queue [number];
		queue->Reset ();

		while (queue->Get (plan_ptr_array)) {
			plan_processor->Plan_Build (plan_ptr_array);
		}
	}
	plan_processor->Stop_Processing ();
}
#endif
