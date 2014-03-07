//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LinkDelay.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LinkDelay::Execute (void)
{
	//---- read the link file ----

	Data_Service::Execute ();

	//---- smooth the link delay data ----

	if (smooth_flag) {
		Smooth_Delay ();
	}

	//---- merge link delay records ----

	if (merge_flag) {
		Read_Link_Delays (merge_file, merge_link_array, merge_turn_array);

		Merge_Delay ();
	}

	//---- merge transit data records ----

	if (transit_flag) {
		if (!merge_flag) {
			merge_link_array.Replicate (link_delay_array);
			merge_turn_array.Replicate (turn_delay_array);
		}
		Load_Transit_Data ();

		Merge_Delay ();
	}

	//---- set minimum travel times ----

	if (min_time_flag) {
		Set_Min_Time ();
	}

	//---- save the results ----

	Write_Link_Delays ();

	Exit_Stat (DONE);
}
