//*********************************************************
//	Plan_Processing.cpp - plan processing thread
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Plan_Processing constructor / destructor
//---------------------------------------------------------

PlanPrep::Plan_Processing::Plan_Processing (PlanPrep *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	num_temp = num_repair = repair_plans = 0;
	plan_file = new_plan_file = merge_file = 0;
}

PlanPrep::Plan_Processing::~Plan_Processing (void)
{
	if (plan_file != 0) {
		delete plan_file;
	}
	if (new_plan_file != 0) {
		delete new_plan_file;
	}
	if (merge_file != 0) {
		delete merge_file;
	}
}

//---------------------------------------------------------
//	Plan_Processing operator
//---------------------------------------------------------

void PlanPrep::Plan_Processing::operator()()
{
	int part = 0;

	plan_file = new Plan_File (exe->plan_file->File_Access (), exe->plan_file->Dbase_Format ());
	plan_file->Part_Flag (exe->plan_file->Part_Flag ());
	plan_file->Pathname (exe->plan_file->Pathname ());
	plan_file->First_Open (false);

	if (exe->new_plan_flag) {
		new_plan_file = new Plan_File (exe->new_plan_file->File_Access (), exe->new_plan_file->Dbase_Format ());
		new_plan_file->File_Type (exe->new_plan_file->File_Type ());
		new_plan_file->Part_Flag (exe->new_plan_file->Part_Flag ());
		new_plan_file->Pathname (exe->new_plan_file->Pathname ());
		new_plan_file->Sort_Type (exe->new_plan_file->Sort_Type ());
		new_plan_file->First_Open (false);
	}
	if (exe->merge_flag) {
		merge_file = new Plan_File (exe->merge_file.File_Access (), exe->merge_file.Dbase_Format ());
		merge_file->Part_Flag (exe->merge_file.Part_Flag ());
		merge_file->Pathname (exe->merge_file.Pathname ());
		merge_file->First_Open (false);
	}

	//---- process each partition ----

	while (exe->partition_queue.Get (part)) {
		Read_Plans (part);

		num_temp = 0;
		plan_ptr_array.clear ();
		traveler_sort.clear ();
		time_sort.clear ();
	}
	MAIN_LOCK
	exe->plan_file->Add_Counters (plan_file);
	plan_file->Close ();

	if (exe->new_plan_flag) {
		exe->new_plan_file->Add_Counters (new_plan_file);
		new_plan_file->Close ();
	}
	if (exe->merge_flag) {
		exe->merge_file.Add_Counters (merge_file);
		merge_file->Close ();
	}
	exe->num_repair += num_repair;
	exe->repair_plans += repair_plans;
	END_LOCK
}
