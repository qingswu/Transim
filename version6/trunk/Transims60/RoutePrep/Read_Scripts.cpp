//*********************************************************
//	Read_Scripts.cpp - compile scripts
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Scripts
//---------------------------------------------------------

void RoutePrep::Read_Scripts (void)
{
	//---- read the conversion script ----

	if (Report_Flag (SCRIPT_REPORT)) {
		Header_Number (SCRIPT_REPORT);

		if (!Break_Check (10)) {
			Print (1);
			Page_Header ();
		}
	}
	Db_Base_Array files;

	files.push_back (route_shape_file);
	files.push_back (new_route_nodes);
	files.push_back (new_link_file);

	convert.Initialize (files, random.Seed () + 1);

	//---- permit writing to input fields ----

	convert.Read_Only (0, false);

	if (!convert.Compile (script_file, Report_Flag (SCRIPT_REPORT))) {
		Error ("Compiling Conversion Script");
	}
	if (Report_Flag (STACK_REPORT)) {
		Header_Number (STACK_REPORT);

		convert.Print_Commands (false);
	}
	Header_Number (0);
}
