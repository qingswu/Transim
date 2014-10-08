//*********************************************************
//	Progression.cpp - Signal Progression Utility
//*********************************************************

#include "Progression.hpp"

//---------------------------------------------------------
//	Progression constructor
//---------------------------------------------------------

Progression::Progression (void) : Data_Service ()
{
	Program ("Progression");
	Version (0);
	Title ("Signal Progression Offset");

	System_File_Type required_files [] = {
		NODE, LINK, CONNECTION, SIGNAL, 
		TIMING_PLAN, PHASING_PLAN, NEW_TIMING_PLAN, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, SIGN, PERFORMANCE, END_FILE
	};
	int file_service_keys [] = {
		LINK_EQUIVALENCE_FILE, 0
	};
	int data_service_keys [] = {
		DAILY_WRAP_FLAG, SUMMARY_TIME_RANGES, SUMMARY_TIME_INCREMENT, 0
	};
	Control_Key progression_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ CLEAR_EXISTING_OFFSETS, "CLEAR_EXISTING_OFFSETS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ EVALUATE_EXISTING_OFFSETS, "EVALUATE_EXISTING_OFFSETS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ PROGRESSION_TIME_PERIODS, "PROGRESSION_TIME_PERIODS", LEVEL0, OPT_KEY, TEXT_KEY, "ALL", TIME_RANGE, NO_HELP },
		{ PROGRESSION_PERIOD_SPEED, "PROGRESSION_PERIOD_SPEED", LEVEL0, OPT_KEY, LIST_KEY, "0.0 MPH", "0..50 MPH", NO_HELP },
		{ OPTIMIZATION_METHOD, "OPTIMIZATION_METHOD", LEVEL0, OPT_KEY, TEXT_KEY, "PERCENT_THRU", "PERCENT_THRU, NETWORK_SPEED, VEHICLE_HOURS", NO_HELP },
		{ GROUP_PERIOD_WEIGHT_FILE, "GROUP_PERIOD_WEIGHT_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ KEEP_LINK_GROUP_ORDER, "KEEP_LINK_GROUP_ORDER", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ NEW_ARC_PROGRESSION_FILE, "NEW_ARC_PROGRESSION_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_DIRECTION_OFFSET, "LINK_DIRECTION_OFFSET", LEVEL0, OPT_KEY, FLOAT_KEY, "0.0 feet", "0.0..45.0 feet", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"GROUP_PERIOD_WEIGHTS",
		"LINK_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Data_Service_Keys (data_service_keys);

	Key_List (progression_keys);
	Report_List (reports);
	
	projection.Add_Keys ();

	fixed = updated = progression_time = period = mid_period = method = max_period = 0;
	progression_speed = 0.0;
	link_offset = 5.0;
	clear_flag = speed_flag = period_flag = arcview_flag = delay_flag = false;
	weight_flag = order_flag = eval_flag = false;
}

//---------------------------------------------------------
//	Progression destructor
//---------------------------------------------------------

Progression::~Progression (void)
{
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	Progression *program = 0;
	try {
		program = new Progression ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif

