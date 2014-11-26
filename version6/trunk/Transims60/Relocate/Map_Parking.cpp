//*********************************************************
//	Map_Parking.cpp - equate parking lots values
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Map_Parking
//---------------------------------------------------------

void Relocate::Map_Parking (void)
{
	int index1, index, max_parking, best, best_diff, diff, num_map, no_map, best_link, link_diff, no_diff;
	double dx, dy;
	Parking_Itr park_itr;
	Parking_Data *park_ptr;
	Point_Map_Itr pt1_itr, pt2_itr;
	
	if (park_problem_flag) {
		park_problem_file.File () << "PARKING\tLINK\tDIR\tOFFSET\tX\tY";
	}

	max_parking = (int) parking_array.size ();
	if (max_parking == num_parking) return;

	num_map = no_map = 0;
	no_diff = Round (50);

	Show_Message ("Mapping Parking Records");
	Set_Progress ();

	for (index1=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, index1++) {
		Show_Progress ();
		if (park_itr->Type () == 2) break;
		if (park_itr->Type () > 0) continue;

		best = best_link = 0;
		best_diff = link_diff = MAX_INTEGER;

		pt1_itr = parking_pt.find (index1);

		for (index = num_parking; index < max_parking; index++) {
			park_ptr = &parking_array [index];

			pt2_itr = parking_pt.find (index);

<<<<<<< .working
			dx = pt2_itr->second.x - pt1_itr->second.y;
			dy = pt2_itr->second.y - pt1_itr->second.y;

			dx = Round (sqrt (dx * dx + dy * dy));
			if (dx > MAX_INTEGER) continue;
			diff = (int) dx;

			if (diff > max_xy_diff) continue;

			if (park_itr->Link () == park_ptr->Link () && park_itr->Dir () == park_ptr->Dir ()) {
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
=======
			dx = pt2_itr->second.x - pt1_itr->second.x;
			dy = pt2_itr->second.y - pt1_itr->second.y;

			dx = Round (sqrt (dx * dx + dy * dy));
			if (dx > MAX_INTEGER) continue;
			diff = (int) dx;

			if (diff > max_xy_diff) continue;

			if (park_itr->Link () == park_ptr->Link () && park_itr->Dir () == park_ptr->Dir ()) {
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
>>>>>>> .merge-right.r1529
			}
		}
		if (best_link > 0) {
			target_park_map.insert (Int_Map_Data (park_itr->Parking (), best_link));
			park_itr->Type (1);
			num_map++;
		} else if (best > 0) {
			target_park_map.insert (Int_Map_Data (park_itr->Parking (), best));
			park_itr->Type (3);
			num_map++;
		} else {
			if (park_problem_flag) {
				park_problem_file.File () << "\n" << park_itr->Parking () << "\t" << 
					(link_array [park_itr->Link ()].Link ()) << "\t" << park_itr->Dir () << 
					"\t" << pt1_itr->second.x << "\t" << pt1_itr->second.y;
			}
			no_map++;
		}
	}
	End_Progress ();

	Write (1, "Parking Lots Mapped = ") << num_map << "; Not Mapped = " << no_map;
	Show_Message (1);
}

