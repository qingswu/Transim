//*********************************************************
//	Read_Plans.cpp - Read the Plan File
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void Relocate::Read_Plans (void)
{
	int part, part_num, first;

	Plan_File *file = (Plan_File *) System_File_Handle (PLAN);
	Plan_File *new_file = (Plan_File *) System_File_Handle (NEW_PLAN);

	Plan_Data *plan_ptr;

	first = 0;

#ifdef THREADS
	num_threads = Num_Threads ();
	
	if (num_threads > 1) {
		plan_processors = new Plan_Processor * [num_threads];

		for (int i=0; i < num_threads; i++) {
			plan_processors [i] = new Plan_Processor (this);
			threads.push_back (thread (ref (*(plan_processors [i]))));
		}
		save_plan.Initialize (this);
		threads.push_back (thread (save_plan));
	}
#endif

	//---- check the partition number ----

	if (file->Part_Flag () && First_Partition () != file->Part_Number ()) {
		file->Open (0);
	} else if (First_Partition () >= 0) {
		first = First_Partition ();
	}

	plan_ptr = new Plan_Data ();

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}
	
		//---- store the plan data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read_Plan (*plan_ptr)) {
			Show_Progress ();
#ifdef THREADS
			if (num_threads > 1) {
				plan_queue.Put_Work (plan_ptr);
				plan_ptr = new Plan_Data ();
				continue;
			}
#endif
			if (Process_Plan (plan_ptr)) {
				new_file->Write_Plan (*plan_ptr);
			}
		}
		End_Progress ();
	}
	file->Close ();
	file->Print_Summary ();

#ifdef THREADS
	if (num_threads > 1) {
		plan_queue.End_of_Work ();
		threads.Join_All ();

		for (int i=0; i < num_threads; i++) {
			delete plan_processors [i];
		}
		delete plan_processors;
	}
	new_file->Print_Summary ();
#endif
}
