//*********************************************************
//	RandomSelect.cpp - randomly distribute to partitions
//*********************************************************

#include "RandomSelect.hpp"

//---------------------------------------------------------
//	RandomSelect constructor
//---------------------------------------------------------

RandomSelect::RandomSelect (void) : Data_Service (), Select_Service ()
{
	Program ("RandomSelect");
	Version (0);
	Title ("Randomly Distribute to Partitions");

	System_File_Type required_files [] = {
		NEW_SELECTION, END_FILE
	};
	System_File_Type optional_files [] = {
		HOUSEHOLD, TRIP, END_FILE
	};
	int select_service_keys [] = {
		SELECT_HOUSEHOLDS, SELECTION_PERCENTAGE, 0
	};
	Control_Key keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ NUMBER_OF_PARTITIONS, "NUMBER_OF_PARTITIONS", LEVEL0, REQ_KEY, INT_KEY, "8", "1..999", NO_HELP },
		END_CONTROL
	};
	Required_System_Files (required_files);
	Optional_System_Files (optional_files);
	Select_Service_Keys (select_service_keys);

	Key_List (keys);

	trip_flag = false;
	num_parts = 0;

	System_Data_Reserve (HOUSEHOLD, 0);
	System_Data_Reserve (TRIP, 0);
}
#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	RandomSelect *program = 0;
	try {
		program = new RandomSelect ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
