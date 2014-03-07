//*********************************************************
//	Read_Scripts.cpp - compile scripts
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Read_Scripts
//---------------------------------------------------------

void NetPrep::Read_Scripts (void)
{
	//---- read the node script ----

	if (node_script_flag) {
		Db_Base_Array files;

		if (node_shape_flag) {
			files.push_back (&node_shape_file);
		} else {
			files.push_back (&node_file);
		}
		files.push_back (System_File_Base (NEW_NODE));

		node_convert.Initialize (files, random.Seed () + 100);

		//---- permit writing to input fields ----

		node_convert.Read_Only (0, false);

		if (!node_convert.Compile (node_script_file, false)) {
			Error ("Compiling Input Node Script");
		}
	}

	//---- read the zone script ----

	if (zone_script_flag) {
		Db_Base_Array files;

		if (zone_shape_flag) {
			files.push_back (&zone_shape_file);
		} else {
			files.push_back (&zone_file);
		}
		files.push_back (System_File_Base (NEW_ZONE));

		zone_convert.Initialize (files, random.Seed () + 200);

		//---- permit writing to input fields ----

		zone_convert.Read_Only (0, false);

		if (!zone_convert.Compile (zone_script_file, false)) {
			Error ("Compiling Input Zone Script");
		}
	}

	//---- read the conversion script ----

	if (convert_flag) {
		if (Report_Flag (SCRIPT_REPORT)) {
			Header_Number (SCRIPT_REPORT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		Db_Base_Array files;

		if (link_shape_flag) {
			files.push_back (&link_shape_file);
		} else if (link_flag) {
			files.push_back (&link_file);
		} else {
			files.push_back (System_File_Base (LINK));
		}
		files.push_back (System_File_Base (NEW_LINK));
		if (link_use_flag) {
			files.push_back (&link_use_file);
		}
		if (approach_flag) {
			files.push_back (&approach_file);
		}
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
}
