//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void FileFormat::Execute (void)
{
	//---- process data files ----

	if (data_flag) {
		Format_Data ();
	}

	//---- process matrix files ----

	if (matrix_flag) {
		Format_Matrix ();
	}


	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case STATS_REPORT:		//---- Field Statistics ----
				Statistics_Report ();
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

void FileFormat::Page_Header (void)
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
		case STATS_REPORT:		//---- Field Statistics ----
			Statistics_Header ();
			break;
		default:
			break;
	}
}
