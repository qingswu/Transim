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
	Strings list;
	Str_Itr str_itr;

	//---- open network files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();
	Read_Flow_Time_Keys ();

	Print (2, String ("%s Control Keys:") % Program ());

	new_file_flag = System_File_Flag (NEW_PERFORMANCE);

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

		if (!new_file_flag) {
			Error ("A New Performance File is required for Merge Processing");
		}
	}

	//---- open the base performance ----
	
	key = Get_Control_String (BASE_PERFORMANCE_FILE);

	if (!key.empty ()) {
		base_file.File_Type ("Base Performance File");
		Print (1);

		if (Check_Control_Key (BASE_PERFORMANCE_FORMAT)) {
			base_file.Dbase_Format (Get_Control_String (BASE_PERFORMANCE_FORMAT));
		}
		base_file.Open (Project_Filename (key));
		base_flag = true;

		if (!new_file_flag) {
			Error ("A New Performance File is required for Base Processing");
		}
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

			if (!System_File_Flag (NEW_TURN_DELAY)) {
				Error ("A New Turn Delay File is required for Merge Processing");
			}
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
	} else {
		key = Get_Control_String (PROCESSING_METHOD);

		if (!key.empty ()) {
			Print (1);
			Get_Control_Text (PROCESSING_METHOD);

			if (!key.Starts_With ("CHECK")) {
				if (Combine_Code (key)) {
					Error (String ("Processing Method %s requires Merge Files") % key);
				} else {
					Error (String ("Processing Method %s was Not Recognized") % key);
				}
			}
			method = -1;
		}
	}

	//---- read the smoothing parameters ----

	smooth_flag = smooth_data.Read_Control ();

	if (smooth_flag) {
		if (!smooth_data.Num_Input (time_periods.Num_Periods ())) {
			Error ("Smooth Parameters are Illogical");
		}
		if (!new_file_flag) {
			Error ("A New Performance File is required for Data Smoothing");
		}
	}

	//---- set min travel time ----

	min_time_flag = Get_Control_Flag (SET_MIN_TRAVEL_TIME);

	if (min_time_flag && !new_file_flag) {
		Error ("A New Performance File is required for Minimum Travel Times");
	}

	//---- set merge transit data ----

	key = Get_Control_Text (MERGE_TRANSIT_DATA);

	if (!key.empty ()) {
		transit_flag = true;

		if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) ||
			!System_File_Flag (TRANSIT_SCHEDULE) || !System_File_Flag (TRANSIT_DRIVER)) {

			Error ("Transit Network Files are Required for Transit Loading");
		}
		if (!new_file_flag) {
			Error ("A New Performance File is required to Merge Transit Data");
		}
		key.Parse (list);
		for (str_itr = list.begin (); str_itr != list.end (); str_itr++) {
			if (str_itr->Starts_With ("VEH")) {
				transit_veh_flag = true;
			} else if (str_itr->Starts_With ("PER")) {
				transit_person_flag = true;
			} else if (str_itr->Starts_With ("PCE") || str_itr->Starts_With ("CAR_EQ")) {
				transit_pce_flag = true;
			}
		}
		if (!System_File_Flag (VEHICLE_TYPE) && transit_pce_flag) {
			Warning ("Vehicle Type File is Required for Transit PCE Loading");
		}
		if (!System_File_Flag (RIDERSHIP) && transit_person_flag) {
			Warning ("Ridership File is Required for Transit Person Loading");
		}
		if (!merge_flag) {
			method = Combine_Code (Get_Control_Text (PROCESSING_METHOD));
		}
	}

	//---- open the time constraint file ----
	
	key = Get_Control_String (TIME_CONSTRAINT_FILE);

	if (!key.empty ()) {
		constraint_file.File_Type ("Time Constraint File");
		Print (1);

		if (Check_Control_Key (TIME_CONSTRAINT_FORMAT)) {
			constraint_file.Dbase_Format (Get_Control_String (TIME_CONSTRAINT_FORMAT));
		}
		constraint_file.Open (Project_Filename (key));
		constraint_flag = true;
	}

	//---- open the time ratio file ----
	
	key = Get_Control_String (NEW_TIME_RATIO_FILE);

	if (!key.empty ()) {
		time_ratio_file.File_Type ("New Time Ratio File");

		time_ratio_file.Create (Project_Filename (key));
		time_ratio_flag = true;
	}

	//---- open the deleted record file ----
	
	key = Get_Control_String (NEW_DELETED_RECORD_FILE);

	if (!key.empty ()) {
		deleted_file.File_Type ("New Deleted Record File");
		Print (1);

		if (Check_Control_Key (NEW_DELETED_RECORD_FORMAT)) {
			deleted_file.Dbase_Format (Get_Control_String (NEW_DELETED_RECORD_FORMAT));
		}
		deleted_file.Create (Project_Filename (key));
		del_file_flag = true;
	}

	List_Reports ();

	first_delete = deleted_flag = (Report_Flag (DELETED_RECORDS) || del_file_flag);
}
