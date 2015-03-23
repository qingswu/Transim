//*********************************************************
//	Trip_Processing.cpp - trip processing thread
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Trip_Processing constructor / destructor
//---------------------------------------------------------

TripPrep::Trip_Processing::Trip_Processing (TripPrep *_exe, int _num)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	num = _num;
	num_temp = 0;
	trip_file = new_trip_file = merge_file = 0;
}

TripPrep::Trip_Processing::~Trip_Processing (void)
{
	if (trip_file != 0) {
		delete trip_file;
	}
	if (new_trip_file != 0) {
		delete new_trip_file;
	}
	if (merge_file != 0) {
		delete merge_file;
	}
}

//---------------------------------------------------------
//	Trip_Processing operator
//---------------------------------------------------------

void TripPrep::Trip_Processing::operator()()
{
	int part = 0;

	if (exe->update_flag && !exe->select_flag) {
		random_part.Seed (exe->random.Seed () + 1000 * (num + 1));
		if (thread_flag) part_count.assign (exe->num_parts, 0);
	}
	if (exe->factor_flag) {
		random_part.Seed (exe->random.Seed () + 1000 * (num + 1));
		random_fac.Seed (exe->random.Seed () + 2000 * (num + 1));
		random_move.Seed (exe->random.Seed () + 3000 * (num + 1));
	}
	trip_file = new Trip_File (exe->trip_file->File_Access (), exe->trip_file->Dbase_Format ());
	trip_file->Part_Flag (exe->trip_file->Part_Flag ());
	trip_file->Pathname (exe->trip_file->Pathname ());
	trip_file->First_Open (false);

	if (exe->new_trip_flag) {
		new_trip_file = new Trip_File (exe->new_trip_file->File_Access (), exe->new_trip_file->Dbase_Format ());
		new_trip_file->File_Type (exe->new_trip_file->File_Type ());
		new_trip_file->Part_Flag (exe->new_trip_file->Part_Flag ());
		new_trip_file->Pathname (exe->new_trip_file->Pathname ());
		new_trip_file->Sort_Type (exe->new_trip_file->Sort_Type ());
		new_trip_file->First_Open (false);
	}
	if (exe->merge_flag) {
		merge_file = new Trip_File (exe->merge_file.File_Access (), exe->merge_file.Dbase_Format ());
		merge_file->Part_Flag (exe->merge_file.Part_Flag ());
		merge_file->Pathname (exe->merge_file.Pathname ());
		merge_file->First_Open (false);
	}

	//---- compile the user script ----

	if (exe->script_flag) {
		Db_Base_Array files;

		files.push_back (trip_file);

		program.Initialize (files, exe->random.Seed () + 100 * (num + 1));
		program.Read_Only (0, false);

		program.Replicate (&exe->program);
	}

	//---- process each partition ----

	while (exe->partition_queue.Get (part)) {
		Read_Trips (part);

		num_temp = 0;
		trip_ptr_array.clear ();
		traveler_sort.clear ();
		time_sort.clear ();
	}
	MAIN_LOCK
	exe->trip_file->Add_Counters (trip_file);
	trip_file->Close ();

	if (exe->new_trip_flag) {
		exe->new_trip_file->Add_Counters (new_trip_file);
		new_trip_file->Close ();
	}
	if (exe->merge_flag) {
		exe->merge_file.Add_Counters (merge_file);
		merge_file->Close ();
	}
	if ((exe->update_flag || exe->new_select_flag) && !exe->select_flag && thread_flag) {
		for (part = 0; part < exe->num_parts; part++) {
			exe->part_count [part] += part_count [part];
		}
		exe->select_map.insert (select_map.begin (), select_map.end ());
	}
	END_LOCK
}
