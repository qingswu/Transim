//*********************************************************
//	Plan_Processing.cpp - plan processing thread
//*********************************************************

#include "PlanCompare.hpp"

//---------------------------------------------------------
//	Plan_Processing constructor / destructor
//---------------------------------------------------------

PlanCompare::Plan_Processing::Plan_Processing (PlanCompare *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	time_sort = exe->time_sort;
	num_trips = num_compare = 0;
	plan_file = compare_file = 0;
	time_diff = cost_diff = 0;
	time_gap = cost_gap = 0;
}

PlanCompare::Plan_Processing::~Plan_Processing (void)
{
	if (plan_file != 0) {
		delete plan_file;
	}
	if (compare_file != 0) {
		delete compare_file;
	}
	if (time_diff != 0) {
		delete time_diff;
	}
	if (cost_diff != 0) {
		delete cost_diff;
	}
	if (time_gap != 0) {
		delete time_gap;
	}
	if (cost_gap != 0) {
		delete cost_gap;
	}
}

//---------------------------------------------------------
//	Plan_Processing operator
//---------------------------------------------------------

void PlanCompare::Plan_Processing::operator()()
{
	int part = 0;

	plan_file = new Plan_File (exe->plan_file->File_Access (), exe->plan_file->Dbase_Format ());
	plan_file->Part_Flag (exe->plan_file->Part_Flag ());
	plan_file->Pathname (exe->plan_file->Pathname ());
	plan_file->Sort_Type (exe->plan_file->Sort_Type ());
	plan_file->First_Open (false);

	compare_file = new Plan_File (exe->compare_file.File_Access (), exe->compare_file.Dbase_Format ());
	compare_file->Part_Flag (exe->compare_file.Part_Flag ());
	compare_file->Pathname (exe->compare_file.Pathname ());
	compare_file->Sort_Type (exe->compare_file.Sort_Type ());
	compare_file->First_Open (false);

	if (thread_flag) {
		if (exe->time_diff.Active_Flag ()) {
			time_diff = new Difference_Data ();
			time_diff->Replicate (exe->time_diff);
		}
		if (exe->cost_diff.Active_Flag ()) {
			cost_diff = new Difference_Data ();
			cost_diff->Replicate (exe->cost_diff);
		}
		if (exe->time_gap.Active_Flag ()) {
			time_gap = new Trip_Gap_Data ();
			time_gap->Replicate (exe->time_gap);
		}
		if (exe->cost_gap.Active_Flag ()) {
			cost_gap = new Trip_Gap_Data ();
			cost_gap->Replicate (exe->cost_gap);
		}
	}

	//---- process each partition ----

	while (exe->partition_queue.Get (part)) {
		Read_Plans (part);
	}
	MAIN_LOCK
	exe->plan_file->Add_Counters (plan_file);
	exe->compare_file.Add_Counters (compare_file);

	exe->num_trips += num_trips;
	exe->num_compare += num_compare;

	if (thread_flag) {
		Select_Map_Itr map_itr;
		Select_Map_Stat map_stat;
		
		for (map_itr = selected.begin (); map_itr != selected.end (); map_itr++) {
			map_stat = exe->selected.insert (*map_itr);

			if (!map_stat.second) {
				Trip_Index index = map_stat.first->first;
				exe->Warning (String ("Duplicate Plan Record %d-%d-%d-%d") % 
					index.Household () % index.Person () % index.Tour () % index.Trip ());
			}
		}
		if (exe->match_flag) {
			exe->matched.insert (matched.begin (), matched.end ());
		}
		if (exe->time_diff.Active_Flag ()) exe->time_diff.Merge_Data (*time_diff);
		if (exe->cost_diff.Active_Flag ()) exe->cost_diff.Merge_Data (*cost_diff);
		if (exe->time_gap.Active_Flag ()) exe->time_gap.Merge_Data (*time_gap);
		if (exe->cost_gap.Active_Flag ()) exe->cost_gap.Merge_Data (*cost_gap);
	}
	END_LOCK
}
