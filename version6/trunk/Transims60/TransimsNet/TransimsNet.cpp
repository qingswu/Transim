//*********************************************************
//	TransimsNet.cpp - Network Conversion Utility
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	TransimsNet constructor
//---------------------------------------------------------

TransimsNet::TransimsNet (void) : Data_Service ()
{
	Program ("TransimsNet");
	Version (17);
	Title ("Network Conversion Utility");

	System_File_Type required_files [] = {
		NODE, LINK, END_FILE
	};
	System_File_Type optional_files [] = {
		SHAPE, ZONE, LOCATION, PARKING, POCKET, CONNECTION, SIGN, SIGNAL, TURN_PENALTY, LANE_USE, ACCESS_LINK, TRANSIT_STOP, 
		NEW_NODE, NEW_ZONE, NEW_LINK, NEW_SHAPE, NEW_LOCATION, NEW_PARKING, NEW_POCKET, NEW_CONNECTION, NEW_SIGN, NEW_SIGNAL,
		NEW_TURN_PENALTY, NEW_LANE_USE, NEW_ACCESS_LINK, NEW_TRANSIT_STOP, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key transimsnet_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DEFAULT_LINK_SETBACK, "DEFAULT_LINK_SETBACK", LEVEL0, OPT_KEY, FLOAT_KEY, "15 feet", "0..60 feet", NO_HELP },
		{ DEFAULT_LOCATION_SETBACK, "DEFAULT_LOCATION_SETBACK", LEVEL0, OPT_KEY, FLOAT_KEY, "60 feet", "0..300 feet", NO_HELP },
		{ MAXIMUM_CONNECTION_ANGLE, "MAXIMUM_CONNECTION_ANGLE", LEVEL0, OPT_KEY, INT_KEY, "120 degrees", "90..180 degrees", NO_HELP },
		{ ADD_UTURN_TO_DEAD_END_LINKS, "ADD_UTURN_TO_DEAD_END_LINKS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ SIGNAL_ID_AS_NODE_ID, "SIGNAL_ID_AS_NODE_ID", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ EXTERNAL_ZONE_RANGE, "EXTERNAL_ZONE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "0", "0..10000", NO_HELP },
		{ EXTERNAL_STATION_OFFSET, "EXTERNAL_STATION_OFFSET", LEVEL0, OPT_KEY, INT_KEY, "60 feet", "0..300 feet", NO_HELP },
		{ REPLICATE_MPO_NETWORK, "REPLICATE_MPO_NETWORK", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ZONE_BOUNDARY_FILE, "ZONE_BOUNDARY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ ZONE_FIELD_NAME, "ZONE_FIELD_NAME", LEVEL0, OPT_KEY, TEXT_KEY, "", "ZONE, TAZ, Z, ID", NO_HELP },
		{ POCKET_LANE_WARRANT, "POCKET_LANE_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "FROM_TYPES, TO_TYPES, AREA_TYPES, POCKET_TYPE, LENGTH feet, LANES", NO_HELP },
		{ TRAFFIC_CONTROL_WARRANT, "TRAFFIC_CONTROL_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "MAIN_TYPES, OTHER_TYPES, AREA_TYPES, CONTROL_TYPE, SETBACK feet, GROUP", NO_HELP },
		{ FACILITY_ACCESS_WARRANT, "FACILITY_ACCESS_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "TYPES, AREA_TYPES, SETBACK feet, MIN_LEN feet, MAX_PTS", NO_HELP },
		{ PARKING_DETAILS_WARRANT, "PARKING_DETAILS_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "AREA_TYPES, TIME, USE, IN seconds, OUT seconds, HOURLY cents, DAILY cents", NO_HELP },
		{ STREET_PARKING_WARRANT, "STREET_PARKING_WARRANT", LEVEL1, OPT_KEY, TEXT_KEY, "", "TYPES, AREA_TYPES, TIME", NO_HELP },
		{ UPDATE_NODE_RANGE, "UPDATE_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ UPDATE_ZONE_RANGE, "UPDATE_ZONE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ UPDATE_LINK_RANGE, "UPDATE_LINK_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "ALL", RANGE_RANGE, NO_HELP },
		{ UPDATE_NODE_FILE, "UPDATE_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ UPDATE_NODE_DATA_FLAG, "UPDATE_NODE_DATA_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_ZONE_FILE, "UPDATE_ZONE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ UPDATE_ZONE_DATA_FLAG, "UPDATE_ZONE_DATA_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_LINK_FILE, "UPDATE_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ UPDATE_LINK_DATA_FLAG, "UPDATE_LINK_DATA_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_SHAPE_FILE, "UPDATE_SHAPE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ REPAIR_CONNECTIONS, "REPAIR_CONNECTIONS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ DELETE_NODE_RANGE, "DELETE_NODE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_ZONE_RANGE, "DELETE_ZONE_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_LINK_RANGE, "DELETE_LINK_RANGE", LEVEL0, OPT_KEY, LIST_KEY, "NONE", RANGE_RANGE, NO_HELP },
		{ DELETE_NODE_FILE, "DELETE_NODE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETE_ZONE_FILE, "DELETE_ZONE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DELETE_LINK_FILE, "DELETE_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_USE_FILE, "LINK_USE_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ LINK_USE_FORMAT, "LINK_USE_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		{ APPROACH_LINK_FILE, "APPROACH_LINK_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ APPROACH_LINK_FORMAT, "APPROACH_LINK_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"REPORT",
		""
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (transimsnet_keys);
	Report_List (reports);

	proj_service.Add_Keys ();

	zone_flag = zout_flag, turn_flag = shape_flag = name_flag = uturn_flag = signal_id_flag = false;
	update_flag = delete_flag = connect_flag = replicate_flag = boundary_flag = false;
	update_link_flag = update_node_flag = update_zone_flag = update_shape_flag = update_dir_flag = false;
	delete_link_flag = delete_node_flag = delete_zone_flag = false;
	link_data_flag = node_data_flag = zone_data_flag = repair_flag = access_flag = control_flag = false;
	ext_zone_flag = false;

	details_flag = link_use_flag = approach_flag = false;
	location_id = parking_id = 0;
	location_base = parking_base = 0;

	nnode = nlink = nparking = nactivity = naccess = npocket = nconnect = nsign = nsignal = nuse = 0;
	mparking = mactivity = mprocess = mpocket = muse = 0;
	xlink = xnode = xzone = xshape = xparking = xlocation = xaccess = xpocket = xconnect = xsign = xsignal = xuse = xturn = xstop = 0;
	nshort = nlength = nexternal = nzone = nzout = max_splits = 0;
	nfixed1 = nfixed2 = nfixed3 = nactuated1 = nactuated2 = nactuated3 = nstop = nyield = 0;
	min_length = link_setback = 0;
	nshape = nshapes = nturn = 0;

	straight_diff = compass.Num_Points () * 9 / 360;		//---- +/- 9 degrees ----
	thru_diff = compass.Num_Points () * 50 / 360;			//---- +/- 50 degrees ----
	max_angle = compass.Num_Points () * 120 / 360;			//---- +/- 120 degrees ----
	uturn_angle = compass.Num_Points () * 180 / 360;		//---- 180 degress ----
	short_length = Round (200.0);							//---- 200 meters ----
	external_offset = Round (30.0);							//---- 30 meters ----
	loc_setback = Round (30.0);								//---- 30 meters ----
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	TransimsNet *program = 0;
	try {
		program = new TransimsNet ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
