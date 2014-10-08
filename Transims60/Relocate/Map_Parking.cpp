//*********************************************************
//	Map_Parking.cpp - equate parking lots values
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Map_Parking
//---------------------------------------------------------

void Relocate::Map_Parking (void)
{
	int index1, index, max_parking, best, best_diff, diff, num_map, no_map;
	Parking_Itr park_itr;
	Parking_Data *park_ptr;
	Point_Map_Itr pt1_itr, pt2_itr;

	max_parking = (int) parking_array.size ();
	if (max_parking == num_parking) return;

	num_map = no_map = 0;
			
	Show_Message ("Mapping Parking Records");
	Set_Progress ();

	for (index1=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, index1++) {
		Show_Progress ();
		if (park_itr->Type () == 2) break;
		if (park_itr->Type () > 0) continue;

		best = 0;
		best_diff = MAX_INTEGER;

		pt1_itr = parking_pt.find (index1);

		for (index = num_parking; index < max_parking; index++) {
			park_ptr = &parking_array [index];

			if (park_itr->Link () != park_ptr->Link ()) continue;
			if (park_itr->Dir () != park_ptr->Dir ()) continue;

			diff = abs (park_itr->Offset () - park_ptr->Offset ());
			if (diff < best_diff) {
				best = index;
				best_diff = diff;
			}
			pt2_itr = parking_pt.find (index);
			if (abs (pt2_itr->second.x - pt1_itr->second.x) <= max_xy_diff &&
				abs (pt2_itr->second.y - pt1_itr->second.y) <= max_xy_diff) {
				best = index;
				break;
			}
		}
		if (best > 0) {
			target_loc_map.insert (Int_Map_Data (park_itr->Parking (), best));
			park_itr->Type (3);
			num_map++;
		} else {
			no_map++;
		}
	}
	End_Progress ();

	Write (1, "Parking Lots Mapped = ") << num_map << "; Not Mapped = " << no_map;
	Show_Message (1);
}

