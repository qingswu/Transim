//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PerfPrep::Execute (void)
{
	//---- read the link file ----

	Data_Service::Execute ();

	//---- smooth the performance and turn data ----

	if (smooth_flag) {
		Smooth_Performance ();

		if (turn_flag) {
			Smooth_Turn_Delays ();
		}
	}

	//---- merge performance records ----

	if (merge_flag) {
		Read_Performance (merge_file, merge_perf_array);

		Merge_Performance ();
	}

	//---- merge turn delay records ----

	if (turn_merge_flag) {
		Read_Turn_Delays (turn_file, merge_turn_array);

		Merge_Turn_Delays ();
	}

	//---- merge transit data records ----

	if (transit_flag) {
		if (!merge_flag) {
			merge_perf_array.Copy_Time_Data (perf_period_array);
		}
		Load_Transit_Data ();

		Merge_Performance ();
	}

	//---- set minimum travel times ----

	if (min_time_flag) {
		Set_Min_Time ();
	}

	//---- save the results ----

	Write_Performance ();

	Exit_Stat (DONE);
}
