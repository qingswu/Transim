//*********************************************************
//	Distribute - allocate productions to attractions
//*********************************************************

#include "NewLandUse.hpp"

#include <math.h>

//---------------------------------------------------------
//	Distribute
//---------------------------------------------------------

bool NewLandUse::Distribute (int field, int step, int group, int cover)
{
	int i, org, des;
	double trips, prod, attr, friction, hi_diff, hi_percent, factor, diff, dx, dy, dist;
	Doubles attr_fac;
	Zone_Field_Itr field_itr;
	Zone_Data *org_zone, *des_zone;

	hi_diff = hi_percent = 0.0;

	//---- balance production and attractions ----

	prod = attr = 0;

	for (org = 0; org < num_zones; org++) {
		prod += prod_tot [org];
	}
	for (des = 0; des < num_zones; des++) {
		attr += attr_tot [des];
	}
	if (prod <= 0 || attr <= 0) return (false);

	if (group >= 0) {
		Show_Message (String ("\tDistribute Land Use %d, Step %d, Group %d -- Iteration") % (field+1) % step % group);
	} else {
		Show_Message (String ("\tDistribute Land Use %d, Step %d -- Iteration") % (field+1) % step);
	}
	Set_Progress ();

	trips = MIN (prod, attr);

	factor = trips / prod;

	for (org = 0; org < num_zones; org++) {
		prod_tot [org] *= factor;
	}

	factor = trips / attr;

	for (des = 0; des < num_zones; des++) {
		attr_bal [des] = attr_tot [des] * factor;
	}
	attr_fac.assign (num_zones, 1.0);

	//---- iterate until convergence ----

	for (i=0; i < max_iterations; i++) {
		Show_Progress ();

		//---- distribute productions to attractions ----
			
		attr_tot.assign (num_zones, 0);

		for (org = 0; org < num_zones; org++) {
			prod = prod_tot [org];
			if (prod == 0) continue;

			org_zone = &zone_array [org];

			trips = 0.0;

			for (des = 0; des < num_zones; des++) {
				//if (des == org) continue;

				attr = attr_bal [des];
				if (attr == 0) continue;

				if (des == org) {
					dist = 1000;
				} else {
					des_zone = &zone_array [des];

					dx = org_zone->X () - des_zone->X ();
					dy = org_zone->Y () - des_zone->Y ();

					dist = sqrt (dx * dx + dy * dy);
				}
				friction = function.Apply_Function (best_function, dist);

				if (friction > 0.0) {
					trips += attr * attr_fac [des] * friction;
				}
			}

			if (trips > 0) {
				factor = prod / trips;
			} else {
				factor = prod;
			}

			for (des = 0; des < num_zones; des++) {
				//if (des == org) continue;

				attr = attr_bal [des];
				if (attr == 0) continue;

				if (des == org) {
					dist = 1000;
				} else {
					des_zone = &zone_array [des];

					dx = org_zone->X () - des_zone->X ();
					dy = org_zone->Y () - des_zone->Y ();

					dist = sqrt (dx * dx + dy * dy);
				}

				friction = function.Apply_Function (best_function, dist);

				if (friction > 0.0) {
					trips = factor * attr * attr_fac [des] * friction;

					attr_tot [des] += trips;
				}
			}
		}

		//---- absolute difference -----

		hi_diff = hi_percent = 0;

		for (des = 0; des < num_zones; des++) {
			attr = attr_bal [des];
			trips = attr_tot [des];

			diff = abs (attr - trips);

			if (attr > 0) {
				factor = diff / attr;
				if (factor > hi_percent) hi_percent = factor;
			}
			if (diff > hi_diff) hi_diff = diff;

			if (trips > 0) {
				attr_fac [des] *= attr / trips;
			} else {
				attr_fac [des] *= 1.0;
			}
		}
		if (hi_diff <= max_diff && hi_percent <= max_percent) break;
	}
	End_Progress ();

	if (i < max_iterations) i++;

	Break_Check (5);
	if (group >= 0) {
		Print (2, String ("\tLand Use %s, Step %d, Group %d, Number of Iterations = %d") % data_names [field] % step % group % i);
	} else {
		Print (2, String ("\tLand Use %s, Step %d, Number of Iterations = %d") % data_names [field] % step % i);
	}
	Print (1, String ("\tMaximum Percent Difference = %.2lf%%") % (hi_percent * 100.0) % FINISH);
	Print (1, String ("\tMaximum Value Difference = %.2lf") % hi_diff);

	//---- apply the changes ----

	for (i=0, field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++, i++) {

		diff = MIN (field_itr->max_change [0] [field], prod_tot [i]);
		field_itr->max_change [0] [field] -= diff;
		field_itr->change [0] [field] += diff;

		diff = MIN (field_itr->max_change [cover] [field], attr_tot [i]);
		field_itr->max_change [cover] [field] -= diff;
		field_itr->change [cover] [field] += diff;
	}
	return (true);
}
