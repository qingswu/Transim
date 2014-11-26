//*********************************************************
//	Parking_Map_Report.cpp - print parking map
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Parking_Map_Report
//---------------------------------------------------------

void Relocate::Parking_Map_Report (void)
{
	Parking_Itr park_itr;
	Parking_Data *park_ptr;
	Int_Map_Itr map_itr;

	Header_Number (PARKING_MAP);

	if (!Break_Check ((int) parking_map.size () + 8)) {
		Print (1);
		Parking_Map_Header ();
	}

	for (park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++) {
		if (park_itr->Type () == 2) {
			if (park_itr->Offset () == -1) continue;

			Print (1, String ("%10d  %10d  %s") % 0 % park_itr->Parking () % "Unused Target Parking");

		} else if (park_itr->Type () == 0) {

			Print (1, String ("%10d  %10d  %s") % park_itr->Parking () % 0 % "Parking Not Mapped");

		} else {
			map_itr = target_park_map.find (park_itr->Parking ());

			if (map_itr != target_park_map.end ()) {
				park_ptr = &parking_array [map_itr->second];
				park_ptr->Offset (-1);

				if (park_itr->Type () == 1) {
					Print (1, String ("%10d  %10d  %s") % park_itr->Parking () % park_ptr->Parking () % "Parking on the Same Link");
				} else {
					Print (1, String ("%10d  %10d  %s") % park_itr->Parking () % park_ptr->Parking () % "Parking Near By");
				}
			}
		}
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Parking_Map_Header
//---------------------------------------------------------

void Relocate::Parking_Map_Header (void)
{
	Print (1, "Parking Map Report");
	Print (1, "   Parking      Target  Notes");
	Print (1);
}

/*********************************************|***********************************************

	Parking Map Report

	   Parking      Target  Notes

	dddddddddd  dddddddddd  sssssssssssssssss

**********************************************|***********************************************/ 
