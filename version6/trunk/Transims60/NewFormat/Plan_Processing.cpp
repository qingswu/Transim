//*********************************************************
//	Plan_Processing.cpp - plan processing thread
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Plan_Processing constructor / destructor
//---------------------------------------------------------

NewFormat::Plan_Processing::Plan_Processing (NewFormat *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	old_plan = 0;
	new_plan = 0;
}

NewFormat::Plan_Processing::~Plan_Processing (void)
{
	if (old_plan != 0) {
		delete old_plan;
	}
	if (new_plan != 0) {
		delete new_plan;
	}
}

//---------------------------------------------------------
//	Plan_Processing operator
//---------------------------------------------------------

void NewFormat::Plan_Processing::operator()()
{
	int part = 0;

	old_plan = new Old_Plan_File (exe->old_plan.File_Access (), exe->old_plan.Plan_Memory (), exe->old_plan.Plan_Sort (), exe->old_plan.Traveler_Scale ());
	old_plan->File_Format (exe->old_plan.File_Format ());
	old_plan->Part_Flag (exe->old_plan.Part_Flag ());
	old_plan->Extend (exe->old_plan.Extend ());
	old_plan->Pathname (exe->old_plan.Pathname ());
	old_plan->Node_Based_Flag (exe->old_plan.Node_Based_Flag ());
	old_plan->First_Open (false);

	Plan_File *file = (Plan_File *) exe->System_File_Handle (NEW_PLAN);

	new_plan = new Plan_File (file->File_Access (), file->Dbase_Format ());
	new_plan->File_Type (file->File_Type ());
	new_plan->Part_Flag (file->Part_Flag ());
	new_plan->Pathname (file->Pathname ());
	new_plan->First_Open (false);

	//---- process each partition ----

	while (exe->partition_queue.Get (part)) {
		Travel_Plans (part);
	}
	MAIN_LOCK
	exe->old_plan.Add_Counters (old_plan);

	file->Add_Counters (new_plan);
	END_LOCK
}
