//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TourData::Execute (void)
{
	//---- get the number of zones and groups ----

	Zone_Groups ();

	//---- read the network data ----

	Data_Service::Execute ();

	//---- calculate zone distances ----

	Zone_Distance ();

	//---- read and normalize the matrix data ----

	Normalize_Origins ();
	
	//---- gather tour data ----

	Read_Tour ();

	//---- process and write zone data ----

	Write_Tour ();

	//---- write the result matrix ----

	if (result_flag) {
		Write (1);
		result_matrix->Write_Matrix ();
	}

	//---- write the attraction file ----

	if (attr_file_flag) {
		Write_Attractions ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case ATTRACTIONS:		//---- Zone Attractions ----
				Attraction_Report ();
				break;
			case GROUP_STATS:		//---- Zone Group Statistics ----
				Group_Stats_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void TourData::Page_Header (void)
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
		case ATTRACTIONS:		//---- Zone Attractions ----
			Attraction_Header ();
			break;
		case GROUP_STATS:		//---- Zone Group Statistics ----
			Group_Stats_Header ();
			break;
		default:
			break;
	}
}
