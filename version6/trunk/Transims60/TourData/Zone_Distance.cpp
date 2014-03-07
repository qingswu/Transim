//*********************************************************
//	Zone_Distance.cpp - zone to zone distances
//*********************************************************

#include "TourData.hpp"

#include <math.h>

//---------------------------------------------------------
//	Zone_Distance
//---------------------------------------------------------

void TourData::Zone_Distance (void)
{
	double org_x, org_y, dx, dy;
	Zone_Itr org_itr, des_itr;

	Show_Message ("Calculating Zone Distance -- Record");
	Set_Progress ();

	for (org_itr = zone_array.begin (); org_itr != zone_array.end (); org_itr++) {
		org_x = org_itr->X ();
		org_y = org_itr->Y ();

		for (des_itr = zone_array.begin (); des_itr != zone_array.end (); des_itr++) {
			Show_Progress ();

			dx = org_x - des_itr->X ();
			dy = org_y - des_itr->Y ();

			od_distance [org_itr->Zone ()] [des_itr->Zone ()] = sqrt (dx * dx + dy * dy);
		}
	}
	End_Progress ();
}
