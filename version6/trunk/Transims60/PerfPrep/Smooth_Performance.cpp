//*********************************************************
//	Smooth_Performance.cpp - smooth the performance data
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Smooth_Performance
//---------------------------------------------------------

void PerfPrep::Smooth_Performance (void)
{
	int index, period, num_periods;

	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;

	num_periods = (int) perf_period_array.size ();
	if (num_periods < 3) return;

	Show_Message ("Smoothing Performance Data -- Record");
	Set_Progress ();

	//---- process each link direction ----

	period_itr = perf_period_array.begin (); 

	for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
		Show_Progress ();

		//---- smooth the volume ----

		smooth_data.Input (0, perf_itr->Volume ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &perf_period_array [period];
			perf_ptr = period_ptr->Data_Ptr (index);

			smooth_data.Input (period, (double) perf_ptr->Volume ());
		}
		smooth_data.Smooth ();

		perf_itr->Volume (smooth_data.Output (0));

		for (period=1; period < num_periods; period++) {
			period_ptr = &perf_period_array [period];
			perf_ptr = period_ptr->Data_Ptr (index);

			perf_ptr->Volume (smooth_data.Output (period));
		}

		//---- smooth the travel time ----

		smooth_data.Input (0, (double) perf_itr->Time ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &perf_period_array [period];
			perf_ptr = period_ptr->Data_Ptr (index);

			smooth_data.Input (period, (double) perf_ptr->Time ());
		}
		smooth_data.Smooth ();

		perf_itr->Time (Dtime (smooth_data.Output (0)));

		for (period=1; period < num_periods; period++) {
			period_ptr = &perf_period_array [period];
			perf_ptr = period_ptr->Data_Ptr (index);

			perf_ptr->Time (Dtime (smooth_data.Output (period)));
		}
	}
	End_Progress ();
}
