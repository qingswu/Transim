//*********************************************************
//	ArcRider.cpp - Ridership File Display Utility
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	ArcRider constructor
//---------------------------------------------------------

ArcRider::ArcRider (void) : Data_Service (), Select_Service (), Draw_Service ()
{
	Program ("ArcRider");
	Version (8);
	Title ("Ridership File Display Utility");

	System_File_Type required_files [] = {
		NODE, LINK, TRANSIT_STOP, TRANSIT_ROUTE, TRANSIT_SCHEDULE, TRANSIT_DRIVER, VEHICLE_TYPE, RIDERSHIP, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, STOP_EQUIVALENCE_FILE, LINE_EQUIVALENCE_FILE, 0
	};
	int select_service_keys [] = {
		SELECT_MODES, SELECT_TIME_OF_DAY, SELECT_START_TIMES, SELECT_END_TIMES, 
		SELECT_LINKS, SELECT_ROUTES, SELECT_STOPS, 0
	};
	int draw_service_keys [] = {
		TRANSIT_STOP_SIDE_OFFSET, TRANSIT_DIRECTION_OFFSET, TRANSIT_OVERLAP_FLAG, MAXIMUM_SHAPE_ANGLE, MINIMUM_SHAPE_LENGTH, 
		BANDWIDTH_FIELD, BANDWIDTH_SCALING_FACTOR, MINIMUM_BANDWIDTH_VALUE, MINIMUM_BANDWIDTH_SIZE, MAXIMUM_BANDWIDTH_SIZE, 0
	};

	Control_Key arcrider_keys [] = {  //--- code, key, level, status, type, default, range, help ----
		{ NEW_ARC_LINE_DEMAND_FILE, "NEW_ARC_LINE_DEMAND_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_LINE_GROUP_FILE, "NEW_ARC_LINE_GROUP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_RIDERSHIP_FILE, "NEW_ARC_RIDERSHIP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_STOP_DEMAND_FILE, "NEW_ARC_STOP_DEMAND_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_STOP_GROUP_FILE, "NEW_ARC_STOP_GROUP_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_ARC_RUN_CAPACITY_FILE, "NEW_ARC_RUN_CAPACITY_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		END_CONTROL
	};

	const char *reports [] = {
		"STOP_EQUIVALENCE",
		"LINE_EQUIVALENCE",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Select_Service_Keys (select_service_keys);
	Draw_Service_Keys (draw_service_keys);

	Key_List (arcrider_keys);
	Report_List (reports);

	projection.Add_Keys ();

	num_line = num_sum = num_rider = num_stop = num_group = num_cap = 0;
	width_field = -1;

	type_flag = true;
	rider_flag = on_off_flag = demand_flag = group_flag = false;
	line_flag = sum_flag = false;
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	ArcRider *program = 0;
	try {
		program = new ArcRider ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
