//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PerfPrep::Program_Control (void)
{
	String key;

	//---- open network files ----

	Data_Service::Program_Control ();
	
	Print (2, String ("%s Control Keys:") % Program ());	

	//---- open the merge performance ----
	
	key = Get_Control_String (MERGE_PERFORMANCE_FILE);

	if (!key.empty ()) {
		merge_file.File_Type ("Merge Performance File");
		Print (1);

		if (Check_Control_Key (MERGE_PERFORMANCE_FORMAT)) {
			merge_file.Dbase_Format (Get_Control_String (MERGE_PERFORMANCE_FORMAT));
		}
		merge_file.Open (Project_Filename (key));
		merge_flag = true;
	}

	//---- open the merge turn delay ----

	turn_flag = System_File_Flag (TURN_DELAY);

	if (turn_flag) {
		key = Get_Control_String (MERGE_TURN_DELAY_FILE);

		if (!key.empty ()) {
			turn_file.File_Type ("Merge Turn Delay File");
			Print (1);

			if (Check_Control_Key (MERGE_TURN_DELAY_FORMAT)) {
				turn_file.Dbase_Format (Get_Control_String (MERGE_TURN_DELAY_FORMAT));
			}
			turn_file.Open (Project_Filename (key));
			turn_merge_flag = true;
		}
	}

	if (merge_flag || turn_merge_flag) {
		Print (1);

		//---- processing method ----

		method = Combine_Code (Get_Control_Text (PROCESSING_METHOD));

		if (method == WEIGHTED_LINK_AVG || method == REPLACE_AVERAGE) {

			//---- merge weighting factor ----

			factor = Get_Control_Double (MERGE_WEIGHTING_FACTOR);
		}
	}

	//---- read the smoothing parameters ----

	smooth_flag = smooth_data.Read_Control ();

	if (smooth_flag) {
		if (!smooth_data.Num_Input (time_periods.Num_Periods ())) {
			Error ("Smooth Parameters are Illogical");
		}
	}

	//---- max travel time ratio ----

	if (Check_Control_Key (MAX_TRAVEL_TIME_RATIO)) {
		Print (1);
		time_ratio = Get_Control_Double (MAX_TRAVEL_TIME_RATIO);
		ratio_flag = (time_ratio > 1.0);
	}

	//---- set min travel time ----

	min_time_flag = Get_Control_Flag (SET_MIN_TRAVEL_TIME);

	//---- set merge transit data ----

	key = Get_Control_Text (MERGE_TRANSIT_DATA);

	if (!key.empty ()) {
		transit_flag = true;

		if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) ||
			!System_File_Flag (TRANSIT_SCHEDULE) || !System_File_Flag (TRANSIT_DRIVER)) {

			Error ("Transit Network Files are Required for Transit Loading");
		}
		if (!System_File_Flag (VEHICLE_TYPE)) {
			Warning ("Vehicle Type File is Required for Transit PCE Loading");
		}
		if (!System_File_Flag (RIDERSHIP)) {
			Warning ("Ridership File is Required for Transit Person Loading");
		}
		if (!merge_flag) {
			method = Combine_Code (Get_Control_Text (PROCESSING_METHOD));
		}
	}
}
