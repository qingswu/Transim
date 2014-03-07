//*********************************************************
//	TourSkim.cpp - build tour-based skim matrices
//*********************************************************

#include "TourSkim.hpp"

int tc_status;

//---------------------------------------------------------
//	TourSkim constructor
//---------------------------------------------------------

TourSkim::TourSkim (void) : Execution_Service ()
{
	Program ("TourSkim");
	Version (0);
	Title ("Build Tour-Based Skim Matrices");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ DRIVE_TO_TRANSIT_SKIM, "DRIVE_TO_TRANSIT_SKIM", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ PARKING_NODE_SKIM, "PARKING_NODE_SKIM", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_DRIVE_TO_TRANSIT_SKIM, "NEW_DRIVE_TO_TRANSIT_SKIM", LEVEL1, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ RETURN_PERIOD_OPTIONS, "RETURN_PERIOD_OPTIONS", LEVEL1, REQ_KEY, LIST_KEY, "1", "", NO_HELP },
		{ WALK_TO_TRANSIT_SKIM, "WALK_TO_TRANSIT_SKIM", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DRIVE_FROM_PARKING_SKIM, "DRIVE_FROM_PARKING_SKIM", LEVEL1, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ RETURN_PERIOD_PREFIX, "RETURN_PERIOD_PREFIX", LEVEL1, OPT_KEY, TEXT_KEY, "", "AM_Ret, MD_Ret, PM_Ret, EL_Ret", NO_HELP },
		{ PARKING_ZONE_MAP_FILE, "PARKING_ZONE_MAP_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DRIVE_TO_TRANSIT_TABLE, "DRIVE_TO_TRANSIT_TABLE", LEVEL1, REQ_KEY, TEXT_KEY, "", "Fare, In-Vehicle Time, Total Walk Time", NO_HELP },
		{ PARKING_NODE_TABLE, "PARKING_NODE_TABLE", LEVEL0, REQ_KEY, TEXT_KEY, "", "Parking Nodes", NO_HELP },
		{ WALK_TO_TRANSIT_TABLE, "WALK_TO_TRANSIT_TABLE", LEVEL1, REQ_KEY, TEXT_KEY, "", "Fare, In-Vehicle Time, WALK TIME (Walk)", NO_HELP },
		{ DRIVE_FROM_PARKING_TABLE, "DRIVE_FROM_PARKING_TABLE", LEVEL0, REQ_KEY, TEXT_KEY, "", "TIME (Skim)", NO_HELP },
		{ RETURN_TABLE_MAP, "RETURN_TABLE_MAP", LEVEL1, OPT_KEY, TEXT_KEY, "", "WALK_#, DRIVE_FROM", NO_HELP },
		END_CONTROL
	};
	Key_List (keys);

#ifdef THREADS
	Enable_Threads (true);
#endif

	num_zones = num_tabs = num_pnr = num_return = 0;
	walk_data = 0;
	drive_data = 0;
}

//---------------------------------------------------------
//	TourSkim destructor
//---------------------------------------------------------

TourSkim::~TourSkim (void)
{
	int r, c, z, p;
	
	if (walk_data != 0) {
		int num_cores = (int) walk_table.size ();

		for (r=0; r < num_return; r++) {
			if (walk_data [r] == 0) continue;

			for (c=0; c < num_cores; c++) {
				for (z=0; z < num_zones; z++) {
					delete [] walk_data [r] [c] [z];
				}
				delete [] walk_data [r] [c];
			}
			delete [] walk_data [r];
		}
		delete [] walk_data;
	}
	if (drive_data != 0) {
		for (r=0; r < num_return; r++) {
			if (drive_data [r] == 0) continue;

			for (p=0; p < num_pnr; p++) {
				delete [] drive_data [r] [p];
			}
			delete [] drive_data [r];
		}
		delete [] drive_data;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int __cdecl main (int commands, char *control [])
{
	int stat = 0;
	TourSkim *program = 0;
	try {
		program = new TourSkim ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
