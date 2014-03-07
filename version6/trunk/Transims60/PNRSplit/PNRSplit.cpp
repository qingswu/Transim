//*********************************************************
//	PNRSplit.cpp - split drive and walk legs
//*********************************************************

#include "PNRSplit.hpp"

int tc_status;

//---------------------------------------------------------
//	TourSkim constructor
//---------------------------------------------------------

PNRSplit::PNRSplit (void) : Execution_Service (), Select_Service () 
{
	Program ("PNRSplit");
	Version (0);
	Title ("Split Drive and Walk Legs");

	int select_service_keys [] = {
		SELECT_ORIGIN_ZONES, SELECT_DESTINATION_ZONES, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DRIVE_TO_TRANSIT_TRIPS, "DRIVE_TO_TRANSIT_TRIPS", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PARKING_NODE_SKIM, "PARKING_NODE_SKIM", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PARKING_ZONE_MAP_FILE, "PARKING_ZONE_MAP_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DRIVE_TO_TRANSIT_TRIPS, "NEW_DRIVE_TO_TRANSIT_TRIPS", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ DRIVE_TO_TRANSIT_TABLE, "DRIVE_TO_TRANSIT_TABLE", LEVEL0, REQ_KEY, TEXT_KEY, "Trips", "", NO_HELP },
		{ PARKING_NODE_TABLE, "PARKING_NODE_TABLE", LEVEL0, REQ_KEY, TEXT_KEY, "", "Parking Nodes", NO_HELP },
		{ WALK_TO_TRANSIT_TABLE, "WALK_TO_TRANSIT_TABLE", LEVEL0, REQ_KEY, TEXT_KEY, "", "", NO_HELP },
		{ DRIVE_TO_PARKING_TABLE, "DRIVE_TO_PARKING_TABLE", LEVEL0, REQ_KEY, TEXT_KEY, "", "", NO_HELP },
		END_CONTROL
	};
	Select_Service_Keys (select_service_keys);
	Key_List (keys);

	num_zones = num_pnr = 0;
	pnr_id = node_id = -1;
	walk_data = 0;
	drive_data = 0;
	pnr_data = 0;
	node_data = 0;
	row_id = 0;
	col_id = 0;
}

//---------------------------------------------------------
//	PNRSplit destructor
//---------------------------------------------------------

PNRSplit::~PNRSplit (void)
{
	int z;
	
	if (walk_data != 0) {
		for (z=0; z < num_zones; z++) {
			delete [] walk_data [z];
		}
		delete [] walk_data;
	}
	if (drive_data != 0) {
		for (z=0; z < num_zones; z++) {
			delete [] drive_data [z];
		}
		delete [] drive_data;
	}
	if (pnr_data != 0) {
		delete [] pnr_data;
	}
	if (node_data != 0) {
		delete [] node_data;
	}
	if (row_id != 0) {
		delete [] row_id;
	}
	if (col_id != 0) {
		delete [] col_id;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int __cdecl main (int commands, char *control [])
{
	int stat = 0;
	PNRSplit *program = 0;
	try {
		program = new PNRSplit ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
