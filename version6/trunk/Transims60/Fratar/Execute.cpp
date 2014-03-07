//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Fratar.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Fratar::Execute (void)
{
	//---- read the margins data ----

	Read_Margins ();

	//---- read the trip table ----

	if (in_flag) {
		Read_Table ();
	}

	//---- process matrix files ----

	Processing ();

	//---- write the table ----

	Write_Table ();

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Fratar::Page_Header (void)
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
