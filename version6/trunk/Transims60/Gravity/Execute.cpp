//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Gravity.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Gravity::Execute (void)
{
	Data_Service::Execute ();

	//---- read the skim file ----

	if (!skim_file->Read_Matrix ()) {
		Error ("Reading the Skim File");
	}

	//---- read trip data ----

	if (trip_flag) {
		Read_Trips ();

		if (calib_flag) {
			Calib_Functions ();
		}
	}

	//---- process matrix files ----

	if (new_flag) {
		Processing ();
	}

	//---- write distribution data ----

	if (in_len_flag) {
		Write_Data (in_len_file, in_distb);
	}
	if (out_len_flag) {
		Write_Data (out_len_file, out_distb);
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Gravity::Page_Header (void)
{
	switch (Header_Number ()) {
		case PRINT_SCRIPT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case PRINT_STACK:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		default:
			break;
	}
}
