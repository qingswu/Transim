//*********************************************************
//	Plan_Processing.cpp - plan processing thread
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Plan_Processing constructor / destructor
//---------------------------------------------------------

PlanSum::Plan_Processing::Plan_Processing (PlanSum *_exe)
{
	exe = _exe;
	thread_flag = (exe->Num_Threads () > 1);
	plan_file = 0;
}

PlanSum::Plan_Processing::~Plan_Processing (void)
{
	if (plan_file != 0) {
		delete plan_file;
	}
}

//---------------------------------------------------------
//	Plan_Processing operator
//---------------------------------------------------------

void PlanSum::Plan_Processing::operator()()
{
	int part = 0;

	if (exe->new_perf_flag) {
		turn_flag = exe->System_File_Flag (CONNECTION);
	}
	plan_file = new Plan_File (exe->plan_file->File_Access (), exe->plan_file->Dbase_Format ());
	plan_file->Part_Flag (exe->plan_file->Part_Flag ());
	plan_file->Pathname (exe->plan_file->Pathname ());
	plan_file->First_Open (false);

	if (thread_flag) {

		//---- initialize link delay data ----

		if (exe->new_perf_flag) {
			perf_period_array.Initialize (&exe->time_periods);
			perf_period_array.Set_Time0 ();

			if (turn_flag) {
				int num = (int) exe->connect_array.size ();

				if (num > 0) {
					turn_period_array.Initialize (&exe->time_periods, num);
				}
			}
		}
	
		//---- initialize time data ----

		if (exe->time_flag) {
			int periods = exe->sum_periods.Num_Periods ();

			start_time.assign (periods, 0);
			mid_time.assign (periods, 0);
			end_time.assign (periods, 0);
		}

		//---- transfer arrays ----

		if (exe->xfer_flag || exe->xfer_detail) {
			Integers rail;

			rail.assign (10, 0);
			total_on_array.assign (10, rail);

			if (exe->xfer_detail) {
				int num = exe->sum_periods.Num_Periods ();
				if (num < 1) num = 1;

				walk_on_array.assign (num, total_on_array);
				drive_on_array.assign (num, total_on_array);
			}
		}

		//---- boarding array ----

		if (exe->boarding_flag) {
			int num = exe->sum_periods.Num_Periods ();
			if (num < 1) num = 1;

			Integers stops;
			stops.assign (exe->stop_map.size (), 0);

			boardings.assign (num, stops);
		}

		//---- initialize summary report data ----

		if (exe->travel_flag) {
			trip_sum_data.Replicate (exe->trip_sum_data);
		}
		if (exe->passenger_flag) {
			pass_sum_data.Replicate (exe->pass_sum_data);
		}

		//---- initialize transit summaries ----

		if (exe->transfer_flag) {
			transfer_array.Replicate (exe->transfer_array);
		}
	}

	//---- process each partition ----
	
	while (exe->partition_queue.Get (part)) {
		Read_Plans (part);
	}
	MAIN_LOCK
	exe->plan_file->Add_Counters (plan_file);
	
	if (thread_flag) {

		//---- combine the link flow data ----

		if (exe->new_perf_flag) {
			exe->perf_period_array.Add_Flows (perf_period_array, true);

			if (turn_flag) {
				exe->turn_period_array.Add_Turns (turn_period_array, true);
			}
		}

		//---- combine the trip time data ----

		if (exe->time_flag) {
			int i, periods;

			periods = exe->sum_periods.Num_Periods ();

			for (i=0; i < periods; i++) {
				exe->start_time [i] += start_time [i];
				exe->mid_time [i] += mid_time [i];
				exe->end_time [i] += end_time [i];
			}
		}

		//---- transfer arrays ----

		if (exe->xfer_flag || exe->xfer_detail) {
			int i, j, p;
			Ints_Itr ints_itr;
			Int_Itr int_itr;
			Board_Itr on_itr;

			for (i=0, ints_itr = total_on_array.begin (); ints_itr != total_on_array.end (); ints_itr, i++) {
				for (j=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, j++) {
					exe->total_on_array [i] [j] += *int_itr;
				}
			}
			if (exe->xfer_detail) {
				for (p=0, on_itr = walk_on_array.begin (); on_itr != walk_on_array.end (); on_itr++, p++) {
					for (i=0, ints_itr = on_itr->begin (); ints_itr != on_itr->end (); ints_itr, i++) {
						for (j=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, j++) {
							exe->walk_on_array [p] [i] [j] += *int_itr;
						}
					}
				}
				for (p=0, on_itr = drive_on_array.begin (); on_itr != drive_on_array.end (); on_itr++, p++) {
					for (i=0, ints_itr = on_itr->begin (); ints_itr != on_itr->end (); ints_itr, i++) {
						for (j=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, j++) {
							exe->drive_on_array [p] [i] [j] += *int_itr;
						}
					}
				}
			}
		}

		//---- boarding arrays ----

		if (exe->boarding_flag) {
			int i, j;
			Ints_Itr ints_itr;
			Int_Itr int_itr;

			for (i=0, ints_itr = boardings.begin (); ints_itr != boardings.end (); ints_itr, i++) {
				for (j=0, int_itr = ints_itr->begin (); int_itr != ints_itr->end (); int_itr++, j++) {
					exe->boardings [i] [j] += *int_itr;
				}
			}
		}

		//---- combine summary report data ----

		if (exe->travel_flag) {
			exe->trip_sum_data.Merge_Data (trip_sum_data);
		}
		if (exe->passenger_flag) {
			exe->pass_sum_data.Merge_Data (pass_sum_data);
		}
		if (exe->transfer_flag) {
			exe->transfer_array.Merge_Data (transfer_array);
		}
	}
	END_LOCK
}
