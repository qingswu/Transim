//*********************************************************
//	TcadImport.cpp - create TransCAD Matrix from text file
//*********************************************************

#include "TcadImport.hpp"

int tc_status;

//---------------------------------------------------------
//	TcadImport constructor
//---------------------------------------------------------

TcadImport::TcadImport (void) : Execution_Service () 
{
	Program ("TcadImport");
	Version (0);
	Title ("Create TransCAD Matrix from Text File");

	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ HIGHEST_ZONE_NUMBER, "HIGHEST_ZONE_NUMBER", LEVEL0, REQ_KEY, INT_KEY, "2000", "1..10000", NO_HELP },
		{ INPUT_MATRIX_FILE, "INPUT_MATRIX_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ INPUT_MATRIX_FORMAT, "INPUT_MATRIX_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "DBASE", FORMAT_RANGE, NO_HELP },
		{ INPUT_ORIGIN_FIELD, "INPUT_ORIGIN_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "ORG", "ORIGIN, ORG, O, I, LOCATION", NO_HELP },
		{ INPUT_DESTINATION_FIELD, "INPUT_DESTINATION_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "DES", "DESTINATION, DES, D, J", NO_HELP },
		{ INPUT_TABLE_FIELDS, "INPUT_TABLE_FIELDS", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NEW_TRANSCAD_MATRIX, "NEW_TRANSCAD_MATRIX", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_MATRIX_LABEL, "NEW_MATRIX_LABEL", LEVEL0, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ NEW_TABLE_NAMES, "NEW_TABLE_NAMES", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		END_CONTROL
	};
	Key_List (keys);

	num_zones = tables = 0;
	org_field = des_field = -1;
	trips = 0;

	int ignore_keys [] = {
		MODEL_START_TIME, MODEL_END_TIME, MODEL_TIME_INCREMENT, RANDOM_NUMBER_SEED,
		MAX_WARNING_MESSAGES, MAX_WARNING_EXIT_FLAG, MAX_PROBLEM_COUNT, NUMBER_OF_THREADS,  0
	};
	Ignore_Keys (ignore_keys);
}

//---------------------------------------------------------
//	TcadImport destructor
//---------------------------------------------------------

TcadImport::~TcadImport (void)
{
	int t, z;
	
	if (trips != 0) {
		for (t=0; t < tables; t++) {
			for (z=0; z < num_zones; z++) {
				delete [] trips [t][z];
			}
			delete [] trips [t];
		}
		delete [] trips;
	}
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int __cdecl main (int commands, char *control [])
{
	int stat = 0;
	TcadImport *program = 0;
	try {
		program = new TcadImport ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
