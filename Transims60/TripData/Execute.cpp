//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TripData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TripData::Execute (void)
{
	//---- set the data files ----

	if (script_flag) {
		Set_Files ();
	}

	//---- read the network data ----

	Data_Service::Execute ();

	//---- read each data file ----

	Show_Message (1);

	if (data_flag) {
		Read_Data ();
	}

	//---- process and write zone data ----

	Write_Trip ();

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TripData::Page_Header (void)
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
