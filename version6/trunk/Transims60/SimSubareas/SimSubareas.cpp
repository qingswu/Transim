//*********************************************************
//	SimSubareas.cpp - generate geographic subareas
//*********************************************************

#include "SimSubareas.hpp"

//---------------------------------------------------------
//	SimSubareas constructor
//---------------------------------------------------------

SimSubareas::SimSubareas (void) : Data_Service ()
{
	Program ("SimSubareas");
	Version (2);
	Title ("Generate Geographic Subareas");

	System_File_Type required_files [] = {
		NODE, NEW_NODE, END_FILE
	};
	System_File_Type optional_files [] = {
		END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NUMBER_OF_SUBAREAS, "NUMBER_OF_SUBAREAS", LEVEL0, OPT_KEY, INT_KEY, "8", "1..1000", NO_HELP },
		{ CENTER_NODE_NUMBER, "CENTER_NODE_NUMBER", LEVEL0, OPT_KEY, INT_KEY, "0", ">=0", NO_HELP },
		{ SUBAREA_BOUNDARY_FILE, "SUBAREA_BOUNDARY_FILE", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ SUBAREA_DATA_FIELD, "SUBAREA_DATA_FIELD", LEVEL0, OPT_KEY, TEXT_KEY, "SUBAREA", "SUBAREA, ID, NUMBER, PARTITION, SUB", NO_HELP },
		{ SUBAREA_UPDATE_FLAG, "SUBAREA_UPDATE_FLAG", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);

	Key_List (keys);

	num_subareas = center = 0;
	subarea_field = -1;
	boundary_flag = false;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	SimSubareas *program = 0;
	try {
		program = new SimSubareas ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
