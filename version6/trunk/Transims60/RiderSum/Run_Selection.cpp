//*********************************************************
//	Run_Selection - set run selection flags
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Run_Selection
//---------------------------------------------------------

bool RiderSum::Run_Selection (Line_Data *line_ptr)
{
	int run, period, num_periods;
	double time;
	Dtime low, high;
	bool run_flags;

	Line_Stop_Itr stop_itr, end_itr;
	Line_Run_Itr run_itr;
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	//---- set the run flags ----

	run_flag.assign (line_array.Max_Runs (), 1);
	run_period.assign (line_array.Max_Runs (), -1);
	period_flag.assign (num_periods, 0);
	run_flags = false;

	if (select_start_times) {
		stop_itr = line_ptr->begin ();

		for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
			if (!start_range.In_Range (run_itr->Schedule ())) {
				run_flag [run] = 0;
			}
			run_flags = true;
		}
	}
	if (select_end_times) {
		stop_itr = --line_ptr->end ();

		for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
			if (!end_range.In_Range (run_itr->Schedule ())) {
				run_flag [run] = 0;
			}
			run_flags = true;
		}
	}
	if (select_time_of_day || num_periods > 0) {
		stop_itr = line_ptr->begin ();
		end_itr = --line_ptr->end ();

		for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
			low = run_itr->Schedule ();
			high = (end_itr->at (run)).Schedule ();
			time = (low + high) / 2;

			if (select_time_of_day) {
				if (!time_range.In_Range (time)) {
					run_flag [run] = 0;
				}
				run_flags = true;
			}
			if (num_periods > 0) {
				period = sum_periods.Period (low, high);
				run_period [run] = period;
				if (period >= 0) {
					period_flag [period] = 1;
				}
			}
		}
	}
	if ((select_start_times || select_end_times || select_time_of_day) && !run_flags) return (false);
	return (true);
}
