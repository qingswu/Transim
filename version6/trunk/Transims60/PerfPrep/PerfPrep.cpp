//*********************************************************
//	PerfPrep.cpp - manipulate the performance file
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	PerfPrep constructor
//---------------------------------------------------------

PerfPrep::PerfPrep (void) : Data_Service ()
{
	Program ("PerfPrep");
	Version (2);
	Title ("Manipulate the Performance File");

	System_File_Type required_files [] = {
		NODE, LINK, PERFORMANCE, NEW_PERFORMANCE, END_FILE
	};
	System_File_Type optional_files [] = {
		CONNECTION, TURN_DELAY, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, 
		RIDERSHIP, VEHICLE_TYPE, NEW_TURN_DELAY, END_FILE
	};
	int file_service_keys [] = {
		SAVE_LANE_USE_FLOWS, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key linkdelay_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ MERGE_PERFORMANCE_FILE, "MERGE_PERFORMANCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_PERFORMANCE_FORMAT, "MERGE_PERFORMANCE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ MERGE_TURN_DELAY_FILE, "MERGE_TURN_DELAY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_TURN_DELAY_FORMAT, "MERGE_TURN_DELAY_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, NO_HELP },
		{ PROCESSING_METHOD, "PROCESSING_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "SIMPLE_AVERAGE", COMBINE_RANGE, NO_HELP },
		{ MERGE_WEIGHTING_FACTOR, "MERGE_WEIGHTING_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "0.0, >= 0.5", NO_HELP },
		{ MAX_TRAVEL_TIME_RATIO, "MAX_TRAVEL_TIME_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0", "0.0, > 1.0", NO_HELP },
		{ SET_MIN_TRAVEL_TIME, "SET_MIN_TRAVEL_TIME", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ MERGE_TRANSIT_DATA, "MERGE_TRANSIT_DATA", LEVEL0, OPT_KEY, TEXT_KEY, "", FLOW_RANGE, NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);

	Key_List (linkdelay_keys);

	smooth_data.Add_Keys ();

	smooth_flag = merge_flag = turn_merge_flag = ratio_flag = min_time_flag = false;
	transit_flag = transit_veh_flag = transit_pce_flag = transit_person_flag = false;

	method = SIMPLE_LINK_AVG;
	factor = 1.0;
	time_ratio = 0.0;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	PerfPrep *program = 0;
	try {
		program = new PerfPrep ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
