//*********************************************************
//	Map_Locations.cpp - equate location values
//*********************************************************

#include "Relocate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Map_Locations
//---------------------------------------------------------

void Relocate::Map_Locations (void)
{
	int index, max_location, best, best_diff, diff, num_map, no_map, best_link, link_diff, no_diff;
	double dx, dy;
	Location_Itr loc_itr;
	Location_Data *loc_ptr;

	if (loc_problem_flag) {
		loc_problem_file.File () << "LOCATION\tLINK\tDIR\tOFFSET\tX\tY";
	}

	max_location = (int) location_array.size ();
	if (max_location == num_location) return;

	num_map = no_map = 0;
	no_diff = Round (50);
			
	Show_Message ("Mapping Location Records");
	Set_Progress ();

	for (loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++) {
		Show_Progress ();
		if (loc_itr->Zone () == 2) break;
		if (loc_itr->Zone () > 0) continue;

		best = best_link = 0;
		best_diff = link_diff = MAX_INTEGER;

		for (index = num_location; index < max_location; index++) {
			loc_ptr = &location_array [index];

			dx = loc_itr->X () - loc_ptr->X ();
			dy = loc_itr->Y () - loc_ptr->Y ();

			dx = sqrt (dx * dx + dy * dy);
			if (dx > MAX_INTEGER) continue;
			diff = (int) dx;

			if (diff > max_xy_diff) continue;

			if (loc_itr->Link () == loc_ptr->Link () && loc_itr->Dir () == loc_ptr->Dir ()) {
				if (diff < link_diff) {
					best_link = index;
					link_diff = diff;
					if (diff < no_diff) break;
				}
			} else {
				if (diff < best_diff) {
					best = index;
					best_diff = diff;
				}
			}
		}
		if (best_link > 0) {
			target_loc_map.insert (Int_Map_Data (loc_itr->Location (), best_link));
			loc_itr->Zone (1);
			num_map++;
		} else if (best > 0) {
			target_loc_map.insert (Int_Map_Data (loc_itr->Location (), best));
			loc_itr->Zone (3);
			num_map++;
		} else {
			if (loc_problem_flag) {
				loc_problem_file.File () << "\n" << loc_itr->Location () << "\t" << 
					(link_array [loc_itr->Link ()].Link ()) << "\t" << loc_itr->Dir () << 
					"\t" << UnRound (loc_itr->X ()) << "\t" << UnRound (loc_itr->Y ());
			}
			no_map++;
		}
	}
	End_Progress ();

	Write (1, "Locations Mapped = ") << num_map << "; Not Mapped = " << no_map;
	Show_Message (1);
}

