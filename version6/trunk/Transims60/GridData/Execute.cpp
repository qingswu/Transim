//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void GridData::Execute (void)
{
	//---- set the data files ----

	if (script_flag) {
		Set_Files ();
	}
	Data_Service::Execute ();

	//---- read zone data files ----

	if (zone_flag) {
		Read_Data ();
	}
	
	//---- read boundary polygon files ----

	if (polygon_flag) {
		Read_Polygons ();
	}

	//---- read the point data ----

	if (point_flag) {
		Read_Points ();
	}

	if (grid_flag) {
		if (zone_flag) {
			Zone_Grids ();
		}

		//---- grid processings ----

		Write_Grid ();

	} else {

		//---- read zone boundary file ----

		Read_Boundary ();

		//---- create grids ----

		Build_Grid ();
	}

	//---- write the summary data ----

	if (summary_flag) {
		Write_Summary ();
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void GridData::Page_Header (void)
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
