//*********************************************************
//	Set_Min_Time.cpp - set the minimum travel time
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Set_Min_Time
//---------------------------------------------------------

void PerfPrep::Set_Min_Time (void)
{
	int index;

	Perf_Period_Itr period_itr;
	Perf_Data *perf_ptr;
	Perf_Itr perf_itr;

	Perf_Period min_time;
	Perf_Data perf_rec;

	Show_Message ("Setting Minimum Travel Time -- Record");
	Set_Progress ();

	//---- allocate work space ----

	period_itr = perf_period_array.begin ();
	index = (int) period_itr->size ();

	if (index == 0) {
		index = (int) dir_array.size ();
	}
	min_time.assign (index, perf_rec);

	//---- process each time period ----

	for (; period_itr != perf_period_array.end (); period_itr++) {

		//---- process each link direction ----

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			Show_Progress ();

			perf_ptr = &min_time [index];

			if (perf_ptr->Time () > perf_itr->Time () || perf_ptr->Time () == 0) {
				perf_ptr->Time (perf_itr->Time ());
			}
		}
	}

	//---- reset the period times ----

	for (period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++) {

		//---- process each link direction ----

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			Show_Progress ();

			if (perf_itr->Volume () < 0.1) {
				perf_ptr = &min_time [index];
				perf_itr->Time (perf_ptr->Time ());
			}
		}
	}
	End_Progress ();
}
