//*********************************************************
//	Trip_Processing.cpp - trip processing thread
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Trip_Processing constructor / destructor
//---------------------------------------------------------

TripSum::Trip_Processing::Trip_Processing (TripSum *_exe, int _thread_num)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	thread_num = _thread_num;
	trip_file = 0;
}

TripSum::Trip_Processing::~Trip_Processing (void)
{
	if (trip_file != 0) {
		delete trip_file;
	}
}

//---------------------------------------------------------
//	Trip_Processing operator
//---------------------------------------------------------

void TripSum::Trip_Processing::operator()()
{
	int num_periods, part;

	part = 0;
	num_periods = exe->sum_periods.Num_Periods ();	

	if (!exe->select_flag) {
		random_part.Seed (exe->random.Seed () + 1000 * (thread_num + 1));
	}
	if (thread_flag) {
		trip_file = new Trip_File (exe->trip_file->File_Access (), exe->trip_file->Dbase_Format ());
		trip_file->Part_Flag (exe->trip_file->Part_Flag ());
		trip_file->Pathname (exe->trip_file->Pathname ());
		trip_file->First_Open (false);

		if (exe->time_flag) {
			start_time.assign (num_periods, 0);
			mid_time.assign (num_periods, 0);
			end_time.assign (num_periods, 0);
		}
		if (exe->link_flag || exe->loc_trip_flag || exe->zone_trip_flag) {
			Integers data;
			data.assign (2 * num_periods, 0);

			if (exe->link_flag) {
				link_trip_data.assign (exe->link_map.size (), data);
			}
			if (exe->loc_trip_flag) {
				loc_trip_data.assign (exe->location_map.size (), data);
			}
			if (exe->zone_trip_flag) {
				zone_trip_data.assign (exe->zone_map.size (), data);
			}
		}
		if (exe->trip_len_flag) trip_length.Replicate (exe->trip_length);
		if (exe->trip_purp_flag) trip_purpose.Replicate (exe->trip_purpose);
		if (exe->mode_len_flag) mode_length.Replicate (exe->mode_length);
		if (exe->mode_purp_flag) mode_purpose.Replicate (exe->mode_purpose);

		file = trip_file;
		start_ptr = &start_time;
		end_ptr = &end_time;
		mid_ptr = &mid_time;
		time_inc_ptr = &time_inc_map;
		len_inc_ptr = &len_inc_map;
		link_trip_ptr = &link_trip_data;
		loc_trip_ptr = &loc_trip_data;
		zone_trip_ptr = &zone_trip_data;
		trip_len_ptr = &trip_length;
		trip_purp_ptr = &trip_purpose;
		mode_len_ptr = &mode_length;
		mode_purp_ptr = &mode_purpose;
	} else {
		file = exe->trip_file;
		start_ptr = &exe->start_time;
		end_ptr = &exe->end_time;
		mid_ptr = &exe->mid_time;
		time_inc_ptr = &exe->time_inc_map;
		len_inc_ptr = &exe->len_inc_map;
		link_trip_ptr = &exe->link_trip_data;
		loc_trip_ptr = &exe->loc_trip_data;
		zone_trip_ptr = &exe->zone_trip_data;
		trip_len_ptr = &exe->trip_length;
		trip_purp_ptr = &exe->trip_purpose;
		mode_len_ptr = &exe->mode_length;
		mode_purp_ptr = &exe->mode_purpose;
	}

	//---- process each partition ----

	while (exe->partition_queue.Get (part)) {
		Read_Trips (part);
	}

	if (thread_flag) {
		MAIN_LOCK
		exe->trip_file->Add_Counters (trip_file);
		trip_file->Close ();

		if (exe->time_flag) {
			for (int p=0; p < num_periods; p++) {
				exe->start_time [p] += start_time [p];
				exe->mid_time [p] += mid_time [p];
				exe->end_time [p] += end_time [p];
			}
		}
		int i, j;
		Ints_Itr ints_itr;
		Int_Itr itr;
		Int_Map_Itr map_itr;
		Int_Map_Stat map_stat;

		if (exe->time_file_flag || exe->time_report) {
			for (map_itr = time_inc_map.begin (); map_itr != time_inc_map.end (); map_itr++) {
				map_stat = exe->time_inc_map.insert (*map_itr);
				if (!map_stat.second) {
					map_stat.first->second += map_itr->second;
				}
			}
		}
		if (exe->len_file_flag) {
			for (map_itr = len_inc_map.begin (); map_itr != len_inc_map.end (); map_itr++) {
				map_stat = exe->len_inc_map.insert (*map_itr);
				if (!map_stat.second) {
					map_stat.first->second += map_itr->second;
				}
			}
		}
		if (exe->link_flag) {
			for (i=0, ints_itr = link_trip_data.begin (); ints_itr != link_trip_data.end (); ints_itr++, i++) {
				for (j=0, itr = ints_itr->begin (); itr != ints_itr->end (); itr++, j++) {
					if (*itr > 0) exe->link_trip_data [i] [j] += *itr;
				}
			}
		}
		if (exe->loc_trip_flag) {
			for (i=0, ints_itr = loc_trip_data.begin (); ints_itr != loc_trip_data.end (); ints_itr++, i++) {
				for (j=0, itr = ints_itr->begin (); itr != ints_itr->end (); itr++, j++) {
					if (*itr > 0) exe->loc_trip_data [i] [j] += *itr;
				}
			}
		}
		if (exe->zone_trip_flag) {
			for (i=0, ints_itr = zone_trip_data.begin (); ints_itr != zone_trip_data.end (); ints_itr++, i++) {
				for (j=0, itr = ints_itr->begin (); itr != ints_itr->end (); itr++, j++) {
					if (*itr > 0) exe->zone_trip_data [i] [j] += *itr;
				}
			}
		}
		if (exe->trip_len_flag) exe->trip_length.Merge_Data (trip_length);
		if (exe->trip_purp_flag) exe->trip_purpose.Merge_Data (trip_purpose);
		if (exe->mode_len_flag) exe->mode_length.Merge_Data (mode_length);
		if (exe->mode_purp_flag) exe->mode_purpose.Merge_Data (mode_purpose);
		END_LOCK
	}
}
