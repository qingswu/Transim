//*********************************************************
//	Plan_Output.cpp - plan output thread
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Plan_Output constructor / destructor
//---------------------------------------------------------

PlanCompare::Plan_Output::Plan_Output (PlanCompare *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	time_sort = exe->time_sort;
	plan_file = compare_file = new_plan_file = 0;
}

PlanCompare::Plan_Output::~Plan_Output (void)
{
	if (plan_file != 0) {
		delete plan_file;
	}
	if (compare_file != 0) {
		delete compare_file;
	}
	if (new_plan_file != 0) {
		delete new_plan_file;
	}
}

//---------------------------------------------------------
//	Plan_Output operator
//---------------------------------------------------------

void PlanCompare::Plan_Output::operator()()
{
	int part = 0;

	plan_file = new Plan_File (exe->plan_file->File_Access (), exe->plan_file->Dbase_Format ());
	plan_file->Part_Flag (exe->plan_file->Part_Flag ());
	plan_file->Pathname (exe->plan_file->Pathname ());
	plan_file->Sort_Type (exe->plan_file->Sort_Type ());
	plan_file->First_Open (false);

	if (exe->merge_flag) {
		compare_file = new Plan_File (exe->compare_file.File_Access (), exe->compare_file.Dbase_Format ());
		compare_file->Part_Flag (exe->compare_file.Part_Flag ());
		compare_file->Pathname (exe->compare_file.Pathname ());
		compare_file->Sort_Type (exe->compare_file.Sort_Type ());
		compare_file->First_Open (false);
	}
	new_plan_file = new Plan_File (exe->new_plan_file->File_Access (), exe->new_plan_file->Dbase_Format ());
	new_plan_file->Part_Flag (exe->new_plan_file->Part_Flag ());
	new_plan_file->Pathname (exe->new_plan_file->Pathname ());
	new_plan_file->Sort_Type (exe->new_plan_file->Sort_Type ());
	new_plan_file->First_Open (false);

	while (exe->output_queue.Get (part)) {
		if (exe->merge_flag) {
			Merge_Plans (part);
		} else {
			Write_Plans (part);
		}
	}
	MAIN_LOCK
	exe->new_plan_file->Add_Counters (new_plan_file);
	END_LOCK
}
