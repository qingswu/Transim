//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PerfPrep::Execute (void)
{
	//---- read the 

	//---- read the performance file ----

	Data_Service::Execute ();

	if (deleted_flag) {
		Header_Number (0);
	}

	//---- read the constraints file ----

	if (constraint_flag) {
		Read_Time_Constraints ();
	}

	//---- check the performance data ----

	if (method == UPDATE_TIMES) {
		Check_Performance ();
	}

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

		if (base_flag) {
			Read_Performance (base_file, base_perf_array);
		}
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

	//---- calculate the travel times ----

	if (new_file_flag && Time_Updates ()) {
		Update_Travel_Times ();
	}

	//---- write the time ratio file ----

	if (time_ratio_flag) {
		Write_Time_Ratios ();
	}

	//---- save the results ----

	if (new_file_flag) {
		Write_Performance ();
	}

	//---- save the results ----

	if (turn_flag && System_File_Flag (NEW_TURN_DELAY)) {
		Write_Turn_Delays ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case DELETED_RECORDS:		//---- Deleted Performance Records ----
//				Deleted_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PerfPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case DELETED_RECORDS:		//---- Deleted Performance Records ----
			Deleted_Header ();
			break;
		default:
			break;
	}
}
