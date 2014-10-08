//*********************************************************
//	Map_Locations.cpp - equate location values
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Map_Locations
//---------------------------------------------------------

void Relocate::Map_Locations (void)
{
	int index, max_location, best, best_diff, diff, num_map, no_map;
	Location_Itr loc_itr;
	Location_Data *loc_ptr;

	max_location = (int) location_array.size ();
	if (max_location == num_location) return;

	num_map = no_map = 0;
			
	Show_Message ("Mapping Location Records");
	Set_Progress ();

	for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {
		Show_Progress ();
		if (loc_itr->Zone () == 2) break;
		if (loc_itr->Zone () > 0) continue;

		best = 0;
		best_diff = MAX_INTEGER;

		for (index = num_location; index < max_location; index++) {
			loc_ptr = &location_array [index];

			if (loc_itr->Link () != loc_ptr->Link ()) continue;
			if (loc_itr->Dir () != loc_ptr->Dir ()) continue;

			diff = abs (loc_itr->Offset () - loc_ptr->Offset ());
			if (diff < best_diff) {
				best = index;
				best_diff = diff;
			}
			if (abs (loc_itr->X () - loc_ptr->X ()) <= max_xy_diff &&
				abs (loc_itr->Y () - loc_ptr->Y ()) <= max_xy_diff) {
				best = index;
				break;
			}
		}
		if (best > 0) {
			target_loc_map.insert (Int_Map_Data (loc_itr->Location (), best));
			loc_itr->Zone (3);
			num_map++;
		} else {
			no_map++;
		}
	}
	End_Progress ();

	Write (1, "Locations Mapped = ") << num_map << "; Not Mapped = " << no_map;
	Show_Message (1);
}

