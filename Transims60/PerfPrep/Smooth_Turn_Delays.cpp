//*********************************************************
//	Smooth_Turn_Delays.cpp - smooth the turn delays data
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Smooth_Turn_Delays
//---------------------------------------------------------

void PerfPrep::Smooth_Turn_Delays (void)
{
	int index, period, num_periods;

	Turn_Itr turn_itr;
	Turn_Data *turn_ptr;
	Turn_Period_Itr period_itr;
	Turn_Period *period_ptr;

	num_periods = (int) turn_period_array.size ();
	if (num_periods < 3) return;

	Show_Message ("Smoothing Turn Delay Data -- Record");
	Set_Progress ();

	//---- process each link direction ----

	period_itr = turn_period_array.begin (); 

	for (index=0, turn_itr = period_itr->begin (); turn_itr != period_itr->end (); turn_itr++, index++) {
		Show_Progress ();

		//---- smooth the turn ----

		smooth_data.Input (0, turn_itr->Turn ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_period_array [period];
			turn_ptr = period_ptr->Data_Ptr (index);

			smooth_data.Input (period, (double) turn_ptr->Turn ());
		}
		smooth_data.Smooth ();

		turn_itr->Turn (smooth_data.Output (0));

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_period_array [period];
			turn_ptr = period_ptr->Data_Ptr (index);

			turn_ptr->Turn (smooth_data.Output (period));
		}

		//---- smooth the travel time ----

		smooth_data.Input (0, (double) turn_itr->Time ());

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_period_array [period];
			turn_ptr = period_ptr->Data_Ptr (index);

			smooth_data.Input (period, (double) turn_ptr->Time ());
		}
		smooth_data.Smooth ();

		turn_itr->Time (Dtime (smooth_data.Output (0)));

		for (period=1; period < num_periods; period++) {
			period_ptr = &turn_period_array [period];
			turn_ptr = period_ptr->Data_Ptr (index);

			turn_ptr->Time (Dtime (smooth_data.Output (period)));
		}
	}
	End_Progress ();
}
