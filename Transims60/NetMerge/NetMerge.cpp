//*********************************************************
//	NetMerge.cpp - merge two networks
//*********************************************************

#include "NetMerge.hpp"

//---------------------------------------------------------
//	NetMerge constructor
//---------------------------------------------------------

NetMerge::NetMerge (void) : Data_Service ()
{
	Program ("NetMerge");
	Version (1);
	Title ("Merge Two Networks");
	
	System_File_Type required_files [] = {
		NODE, LINK, NEW_NODE, NEW_LINK, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, POCKET, LANE_USE, CONNECTION, LOCATION, PARKING, ACCESS_LINK, TRANSIT_STOP, 
		SIGN, SIGNAL, TIMING_PLAN, PHASING_PLAN, DETECTOR, ZONE, 
		PERFORMANCE, TURN_DELAY, 
		NEW_SHAPE, NEW_POCKET, NEW_LANE_USE, NEW_CONNECTION, NEW_LOCATION, NEW_PARKING, 
		NEW_ACCESS_LINK, NEW_TRANSIT_STOP, 
		NEW_SIGN, NEW_SIGNAL, NEW_TIMING_PLAN, NEW_PHASING_PLAN, NEW_DETECTOR, NEW_ZONE,
		NEW_PERFORMANCE, NEW_TURN_DELAY, END_FILE
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ MERGE_DIRECTORY, "MERGE_DIRECTORY", LEVEL0, OPT_KEY, PATH_KEY, "", "", NO_HELP },
		{ MERGE_NODE_FILE, "MERGE_NODE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_SHAPE_FILE, "MERGE_SHAPE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_LINK_FILE, "MERGE_LINK_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_POCKET_FILE, "MERGE_POCKET_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_LANE_USE_FILE, "MERGE_LANE_USE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_LOCATION_FILE, "MERGE_LOCATION_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_CONNECTION_FILE, "MERGE_CONNECTION_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_PARKING_FILE, "MERGE_PARKING_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_ACCESS_FILE, "MERGE_ACCESS_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_STOP_FILE, "MERGE_STOP_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_SIGN_FILE, "MERGE_SIGN_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_SIGNAL_FILE, "MERGE_SIGNAL_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_TIMING_PLAN_FILE, "MERGE_TIMING_PLAN_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_PHASING_PLAN_FILE, "MERGE_PHASING_PLAN_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_DETECTOR_FILE, "MERGE_DETECTOR_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_ZONE_FILE, "MERGE_ZONE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_PERFORMANCE_FILE, "MERGE_PERFORMANCE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ MERGE_TURN_DELAY_FILE, "MERGE_TURN_DELAY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);

	Key_List (keys);
	Report_List (reports);

	shape_flag = pocket_flag = lane_use_flag = location_flag = parking_flag = access_flag = stop_flag = merge_flag = false;
	sign_flag = signal_flag = timing_flag = phasing_flag = detector_flag = performance_flag = turn_delay_flag = false;
	zone_flag = false;

	System_Read_False (PERFORMANCE);
	System_Read_False (TURN_DELAY);
}

//---------------------------------------------------------
//	NetMerge destructor
//---------------------------------------------------------

NetMerge::~NetMerge (void)
{
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	NetMerge *program = 0;
	try {
		program = new NetMerge ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
