//*********************************************************
//	TripData.cpp - Trip Table Data Processing
//*********************************************************

#include "TripData.hpp"

//---------------------------------------------------------
//	ZoneData constructor
//---------------------------------------------------------

TripData::TripData (void) : Data_Service ()
{
	Program ("TripData");
	Version (0);
	Title ("Trip Table Data Processor");

	System_File_Type optional_files [] = {
		ZONE, END_FILE
	};
	int file_service_keys [] = {
		NOTES_AND_NAME_FIELDS, 0
	};
	Control_Key tripdata_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ TRIP_FILE, "TRIP_FILE", LEVEL0, REQ_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ TRIP_FORMAT, "TRIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TPPLUS", MATRIX_RANGE, FORMAT_HELP },
		{ NEW_TRIP_FILE, "NEW_TRIP_FILE", LEVEL0, REQ_KEY, OUT_KEY, "", FILE_RANGE, NO_HELP },
		{ NEW_TRIP_FORMAT, "NEW_TRIP_FORMAT", LEVEL0, OPT_KEY, TEXT_KEY, "TPPLUS", MATRIX_RANGE, FORMAT_HELP },

		{ COPY_EXISTING_FIELDS, "COPY_EXISTING_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ CLEAR_ALL_FIELDS, "CLEAR_ALL_FIELDS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ ZONE_FILE_HEADER, "ZONE_FILE_HEADER", LEVEL0, OPT_KEY, BOOL_KEY, "TRUE", BOOL_RANGE, NO_HELP },
		{ NEW_ZONE_FIELD, "NEW_ZONE_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "NAME, INTEGER, 10", NO_HELP },
		{ CONVERSION_SCRIPT, "CONVERSION_SCRIPT", LEVEL0, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FILE, "DATA_FILE", LEVEL1, OPT_KEY, IN_KEY, "", FILE_RANGE, NO_HELP },
		{ DATA_FORMAT, "DATA_FORMAT", LEVEL1, OPT_KEY, TEXT_KEY, "TAB_DELIMITED", FORMAT_RANGE, FORMAT_HELP  },
		{ DATA_JOIN_FIELD, "DATA_JOIN_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		{ ZONE_JOIN_FIELD, "ZONE_JOIN_FIELD", LEVEL1, OPT_KEY, TEXT_KEY, "", "", NO_HELP },
		END_CONTROL
	};
	const char *reports [] = {
		"CONVERSION_SCRIPT",
		"CONVERSION_STACK",
		""
	};
	Optional_System_Files (optional_files);
	File_Service_Keys (file_service_keys);
	Key_List (tripdata_keys);
	Report_List (reports);

	num_data_files = 0;
	copy_flag = script_flag = data_flag = false;
}

#ifdef _CONSOLE
//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	int stat = 0;
	TripData *program = 0;
	try {
		program = new TripData ();
		stat = program->Start_Execution (commands, control);
	}
	catch (exit_exception &e) {
		stat = e.Exit_Code ();
	}
	if (program != 0) delete program;
	return (stat);
}
#endif
