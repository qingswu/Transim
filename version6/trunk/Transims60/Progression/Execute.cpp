//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Progression::Execute (void)
{
	int group, ngroup, best_group, best_perf, perf, nperiod;
	Dtime low, high;

	//---- read the network files ----

	Data_Service::Execute ();

	//---- group period weights ----

	if (weight_flag) {
		Read_Weights ();
	}

	//---- prepare signal data ----

	Signal_Setup ();

	//---- find the best group for setting offsets ----

	old_thru = tot_thru = ngroup = 0;
	nperiod = progress_time.Num_Periods ();

	for (period=0; period < nperiod; period++) {
		Show_Message ("Progressing Time Period ") << (period + 1);
		Show_Message (1);

		progress_time.Period_Range (period, low, high);

		progression_time = high;
		perf_period_ptr = perf_period_array.Period_Ptr ((high + low + 1) / 2);

		progression_speed = progress_speed.Best (period);
		speed_flag = (progression_speed != 0);

		//---- prepare the group data ----

		group_data_array.clear ();
		signal_offset_map.clear ();

		if (Link_Equiv_Flag ()) {
			User_Groups ();
		} else {
			Signal_Groups ();
		}
		ngroup = (int) group_data_array.size ();

		//---- evaluate the existing offsets ----

		if (eval_flag) {
			Set_Offset (1, true, true);
		}
		if (order_flag) {

			//---- use link group order ----

			Set_Offset (1, true);

		} else {

			Show_Message ("Setting Signal Offsets -- Group");
			Set_Progress ();

			best_group = 0;
			best_perf = -(MAX_INTEGER-1);

			for (group=1; group <= ngroup; group++) {
				Show_Progress (group);

				perf = Set_Offset (group);

				if (perf > best_perf) {
					best_perf = perf;
					best_group = group;
				}
			}
			End_Progress ();
	
			//---- reset the offsets for the best group ----

			Set_Offset (best_group, true);
		}
	}
	Write_Timing_Plans ();

	//---- summarize results ----

	Break_Check (5);
	Write (2, "Number of Fixed Time Signals = ") << fixed;	
	Write (1, "Number of Updated Offsets = ") << updated;
	Write (1, "Number of Signal Groups = ") << ngroup;
	Write (1, "Number of Time Periods = ") << nperiod;
	if (nperiod > 0) {
		Write (1, String ("Non-Stop Thru Movements = %.1lf%%") % ((double) tot_thru / (10.0 * nperiod)) % FINISH);

		if (eval_flag) {
			Write (1, String ("Original Non-Stop Thru Movements = %.1lf%%") % ((double) old_thru / (10.0 * nperiod)) % FINISH);
		}
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Progression::Page_Header (void)
{
	switch (Header_Number ()) {
		case WEIGHT_REPORT:		//---- group period weights ----
			Weight_Header ();
			break;
		default:
			break;
	}
}
