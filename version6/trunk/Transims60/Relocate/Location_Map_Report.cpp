//*********************************************************
//	Location_Map_Report.cpp - print location map
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Location_Map_Report
//---------------------------------------------------------

void Relocate::Location_Map_Report (void)
{
	Location_Itr loc_itr;
	Location_Data *loc_ptr;
	Int_Map_Itr map_itr;

	Header_Number (LOCATION_MAP);

	if (!Break_Check ((int) location_map.size () + 8)) {
		Print (1);
		Location_Map_Header ();
	}

	for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {

		if (loc_itr->Zone () == 2) {
			if (loc_itr->Offset () == -1) continue;

			Print (1, String ("%10d  %10d  %s") % 0 % loc_itr->Location () % "Unused Target Location");

		} else if (loc_itr->Zone () == 0) {

			Print (1, String ("%10d  %10d  %s") % loc_itr->Location () % 0 % "Location Not Mapped");

		} else {
			map_itr = target_loc_map.find (loc_itr->Location ());

			if (map_itr != target_loc_map.end ()) {
				loc_ptr = &location_array [map_itr->second];
				loc_ptr->Offset (-1);

				if (loc_itr->Zone () == 1) {
					Print (1, String ("%10d  %10d  %s") % loc_itr->Location () % loc_ptr->Location () % "Location on the Same Link");
				} else {
					Print (1, String ("%10d  %10d  %s") % loc_itr->Location () % loc_ptr->Location () % "Location Near By");
				}
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Location_Map_Header
//---------------------------------------------------------

void Relocate::Location_Map_Header (void)
{
	Print (1, "Location Map Report");
	Print (1, "  Location      Target  Notes");
	Print (1);
}

/*********************************************|***********************************************

	Location Map Report

	  Location      Target  Notes

	dddddddddd  dddddddddd  sssssssssssssssss

**********************************************|***********************************************/ 
