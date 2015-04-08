//*********************************************************
//	Best_Destination.cpp - best destination location
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Best_Destination
//---------------------------------------------------------

void Converge_Service::Best_Destination (Plan_Data &plan)
{
	int index, best_loc, origin;
	double x, y, dx, dy, diff, min_diff;

	Loc_Cap_Itr cap_itr;
	Loc_Cap_Data *cap_ptr;
	Location_Data *loc_ptr;

	origin = plan.Origin ();
	loc_ptr = &location_array [origin];
	x = loc_ptr->X ();
	y = loc_ptr->Y ();

	best_loc = -1;
	min_diff = 0.0;

	for (index=0, cap_itr = loc_cap_array.begin (); cap_itr != loc_cap_array.end (); cap_itr++, index++) {
		if (cap_itr->demand >= cap_itr->capacity || index == origin) continue;

		loc_ptr = &location_array [index];

		dx = loc_ptr->X () - x;
		dy = loc_ptr->Y () - y;

		diff = dx * dx + dy * dy;

		if (best_loc < 0 || diff < min_diff) {
			best_loc = index;
			min_diff = diff;
		}
	}
	if (best_loc >= 0) {
		plan.Destination (best_loc);

		cap_ptr = &loc_cap_array [best_loc];
MAIN_LOCK
		cap_ptr->demand++;
END_LOCK
	}
}
