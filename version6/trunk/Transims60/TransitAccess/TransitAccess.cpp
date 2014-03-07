//*********************************************************
//	TransitAccess.cpp - generate transit access links
//*********************************************************

#include "TransitAccess.hpp"

//---------------------------------------------------------
//	TransitAccess constructor
//---------------------------------------------------------

TransitAccess::TransitAccess (void) : Execution_Service (), Select_Service ()
{
	Program ("TransitAccess");
	Version (0);
	Title ("Generate Transit Access Links");
	
	int select_service_keys [] = {
		SELECT_ORIGIN_ZONES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ ZONE_FILE, "ZONE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_FORMAT, "ZONE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ ZONE_NUMBER_FIELD, "ZONE_NUMBER_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "ZONE", "", NO_HELP },
		{ ZONE_X_COORD_FIELD, "ZONE_X_COORD_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "X_COORD", "", NO_HELP },
		{ ZONE_Y_COORD_FIELD, "ZONE_Y_COORD_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "Y_COORD", "", NO_HELP },
		{ ZONE_LOCATION_FIELD, "ZONE_LOCATION_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "JUR", "", NO_HELP },
		{ ZONE_AREA_FIELD, "ZONE_AREA_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "AREA", "", NO_HELP },
		{ ZONE_LONG_WALK_FIELD, "ZONE_LONG_WALK_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "LONG", "", NO_HELP },
		{ ZONE_PEF_FIELD, "ZONE_PEF_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "PEF", "", NO_HELP },
		{ PEF_DISTANCE_WEIGHTS, "PEF_DISTANCE_WEIGHTS", LEVEL0, OPT_KEY, LIST_KEY, "30:2.0", "MaxPEF:Factor (e.g., 10:3.0, 30:2.0, 75:1.5)", NO_HELP },
		{ ZONE_AREA_FACTORS, "ZONE_AREA_FACTORS", LEVEL0, OPT_KEY, LIST_KEY, "1.0", "1.0..5.0", NO_HELP },
		{ ZONE_STATION_MODE_CODES, "ZONE_STATION_MODE_CODES", LEVEL0, OPT_KEY, LIST_KEY, "", "M, C, L, B, N", NO_HELP },

		{ NODE_FILE, "NODE_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NODE_FORMAT, "NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ NODE_NUMBER_FIELD, "NODE_NUMBER_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "NODE", "", NO_HELP },
		{ NODE_X_COORD_FIELD, "NODE_X_COORD_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "X_COORD", "", NO_HELP },
		{ NODE_Y_COORD_FIELD, "NODE_Y_COORD_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "Y_COORD", "", NO_HELP },

		{ WALK_LINK_FILE, "WALK_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ WALK_LINK_FORMAT, "WALK_LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ WALK_LINK_ANODE_FIELD, "WALK_LINK_ANODE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ANODE", "", NO_HELP },
		{ WALK_LINK_BNODE_FIELD, "WALK_LINK_BNODE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "BNODE", "", NO_HELP },
		{ WALK_LINK_LENGTH_FIELD, "WALK_LINK_LENGTH_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "LENGTH", "", NO_HELP },
		{ WALK_LINK_ZONE_FIELD, "WALK_LINK_ZONE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ZONE", "", NO_HELP },
		{ WALK_LINK_TYPE_FIELD, "WALK_LINK_TYPE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "TYPE", "", NO_HELP },
		{ WALK_LINK_TYPE_RANGE, "WALK_LINK_TYPE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },

		{ EXTRA_LINK_FILE, "EXTRA_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ EXTRA_LINK_FORMAT, "EXTRA_LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ EXTRA_LINK_ACTION_FIELD, "EXTRA_LINK_ACTION_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ACTION", "", NO_HELP },
		{ EXTRA_LINK_ANODE_FIELD, "EXTRA_LINK_ANODE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ANODE", "", NO_HELP },
		{ EXTRA_LINK_BNODE_FIELD, "EXTRA_LINK_BNODE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "BNODE", "", NO_HELP },

		{ STATION_FILE, "STATION_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ STATION_FORMAT, "STATION_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ STATION_MODE_FIELD, "STATION_MODE_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "MODE", "", NO_HELP },
		{ STATION_TYPE_FIELD, "STATION_TYPE_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "TYPE", "", NO_HELP },
		{ STATION_PNR_FLAG_FIELD, "STATION_PNR_FLAG_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "PNR", "", NO_HELP },
		{ STATION_USE_FLAG_FIELD, "STATION_USE_FLAG_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "USE", "", NO_HELP },
		{ STATION_SKIM_FIELD, "STATION_SKIM_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "SKIM", "", NO_HELP },
		{ STATION_ZONE_FIELD, "STATION_ZONE_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "ZONE", "", NO_HELP },
		{ STATION_STOP_FIELD, "STATION_STOP_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "STOP", "", NO_HELP },
		{ STATION_PARKING_FIELD, "STATION_PARKING_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "PARKING", "", NO_HELP },
		{ STATION_NODE_FIELDS, "STATION_NODE_FIELDS", LEVEL1, REQ_KEY, LIST_KEY, "NODE", "", NO_HELP },
		{ STATION_ACCESS_FIELD, "STATION_ACCESS_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ACCESS", "", NO_HELP },
		{ STATION_CAPACITY_FIELD, "STATION_CAPACITY_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "CAPACITY", "", NO_HELP },
		{ STATION_COST_FIELDS, "STATION_COST_FIELDS", LEVEL1, OPT_KEY, LIST_KEY, "COST", "", NO_HELP },
		{ STATION_TIME_FIELDS, "STATION_TIME_FIELDS", LEVEL1, OPT_KEY, LIST_KEY, "TIME", "", NO_HELP },
		{ STATION_X_COORD_FIELD, "STATION_X_COORD_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "X_COORD", "", NO_HELP },
		{ STATION_Y_COORD_FIELD, "STATION_Y_COORD_FIELD", LEVEL0, REQ_KEY, TEXT_KEY, "Y_COORD", "", NO_HELP },
		{ STATION_NAME_FIELD, "STATION_NAME_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "NAME", "", NO_HELP },

		{ SKIM_FILE, "SKIM_FILE", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SKIM_FORMAT, "SKIM_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TPPLUS", MATRIX_RANGE, FORMAT_HELP },
		{ SKIM_TIME_TABLE, "SKIM_TIME_TABLE", LEVEL1, REQ_KEY, INT_KEY, "1", ">0", NO_HELP },
		{ SKIM_DISTANCE_TABLE, "SKIM_DISTANCE_TABLE", LEVEL1, REQ_KEY, INT_KEY, "2", ">0", NO_HELP },

		{ VALUE_OF_TIME, "VALUE_OF_TIME", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 dollars/hour", "0..100.0 dollars/hour", NO_HELP },
		{ CBD_ZONE_NUMBER, "CBD_ZONE_NUMBER", LEVEL0, REQ_KEY, INT_KEY, "1", ">0", NO_HELP },
		{ MAX_DIVERSION_RATIO, "MAX_DIVERSION_RATIO", LEVEL0, OPT_KEY, FLOAT_KEY, "1.5", ">1.0", NO_HELP },
		{ MAX_DIVERSION_DISTANCE, "MAX_DIVERSION_DISTANCE", LEVEL0, OPT_KEY, FLOAT_KEY, "10.0 miles", ">1.0 miles", NO_HELP },
		{ BARRIER_LOCATION_MAP, "BARRIER_LOCATION_MAP", LEVEL1, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ PROHIBITED_INTERCHANGES, "PROHIBITED_INTERCHANGES", LEVEL0, OPT_KEY, LIST_KEY, "", "From1-To1, From2-To2", NO_HELP },

		{ SLUG_ZONE_FILE, "SLUG_ZONE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SLUG_ZONE_FORMAT, "SLUG_ZONE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ SLUG_ZONE_FIELD, "SLUG_ZONE_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ZONE", "", NO_HELP },
		{ SLUG_STATION_TYPE, "SLUG_STATION_TYPE", LEVEL0, OPT_KEY, INT_KEY, "8", ">0", NO_HELP },

		{ NEW_SIDEWALK_FILE, "NEW_SIDEWALK_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_WALK_ACCESS_FILE, "NEW_WALK_ACCESS_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_WALK_LINK_FILE, "NEW_WALK_LINK_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_NODE_FILE, "NEW_NODE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_NODE_FORMAT, "NEW_NODE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },

		{ NEW_STOP_ZONE_FILE, "NEW_STOP_ZONE_FILE", LEVEL0, OPT_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ STOP_ZONE_MODE_CODE, "STOP_ZONE_MODE_CODE", LEVEL0, OPT_KEY, TEXT_KEY, "M", ">=A and <= Z", NO_HELP },
		{ STOP_ZONE_OFFSET, "STOP_ZONE_OFFSET", LEVEL0, OPT_KEY, INT_KEY, "8000", ">0", NO_HELP }, 

		{ NEW_MODE_FILE, "NEW_MODE_FILE", LEVEL1, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ MODE_CODE, "MODE_CODE", LEVEL1, REQ_KEY, TEXT_KEY, "M", ">=A and <= Z", NO_HELP },
		{ MODE_SKIM, "MODE_SKIM", LEVEL1, REQ_KEY, INT_KEY, "1", ">=0", NO_HELP },
		{ MODE_PNR_FLAG, "MODE_PNR_FLAG", LEVEL1, REQ_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ MODE_CONNECTION, "MODE_CONNECTION", LEVEL1, REQ_KEY, TEXT_KEY, "PARKING", "PARKING, STOP, NODE", NO_HELP },
		{ MODE_TYPE_DISTANCES, "MODE_TYPE_DISTANCES", LEVEL1, OPT_KEY, LIST_KEY, "5.0 miles", "0.0..50.0 miles", NO_HELP },
		{ MODE_WALK_TIMES, "MODE_WALK_TIMES", LEVEL1, OPT_KEY, LIST_KEY, "2.0 minutes", "0.0..10.0 minutes", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"ZONE_DATA_REPORT", "STATION_DATA_REPORT",
		""
	};
	Select_Service_Keys (select_service_keys);

	Key_List (keys);
	Report_List (reports);

	slug_flag = node_flag = cost_flag = stop_zone_flag = zone_walk_flag = walk_link_flag = extra_flag = station_acc_flag = false;
	nzones = ndes = cbd_zone = slug_type = missing_skims = num_default = highest_skim = 0;

	max_ratio = 1.5;
	max_dist = 10.0;
	time_value = 10.0;

	zone_fld = zone_x_fld = zone_y_fld = zone_loc_fld = zone_area_fld = zone_long_fld = zone_pef_fld = -1;		
	new_node_fld = new_x_fld = new_y_fld = slug_fld = -1;
	link_anode_fld = link_bnode_fld = link_len_fld = link_zone_fld = link_type_fld = -1;
	extra_action_fld = extra_anode_fld = extra_bnode_fld = -1;;

	station_mode_fld = station_type_fld = station_pnr_fld = station_use_fld = -1;
	station_skim_fld = station_zone_fld = station_stop_fld = station_park_fld = -1;
	station_cap_fld = station_acc_fld = station_x_fld = station_y_fld = station_name_fld = -1;

	int ignore_keys [] = {
		TIME_OF_DAY_FORMAT, MODEL_START_TIME, MODEL_END_TIME, MODEL_TIME_INCREMENT, UNITS_OF_MEASURE, 
		RANDOM_NUMBER_SEED,	MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, NUMBER_OF_THREADS, 0
	};
	Ignore_Keys (ignore_keys);
}

//---------------------------------------------------------
//	TransitAccess destructor
//---------------------------------------------------------

TransitAccess::~TransitAccess (void)
{
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	TransitAccess *program = 0;
	try {
		program = new TransitAccess ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
