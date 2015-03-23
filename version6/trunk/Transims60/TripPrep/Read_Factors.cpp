//*********************************************************
//	Read_Factors.cpp - read factor file into memory
//*********************************************************

#include "TripPrep.hpp"

//---------------------------------------------------------
//	Read_Factors
//---------------------------------------------------------

void TripPrep::Read_Factors (void)
{
	int org, des, period, num_periods, zone;
	double factor, sum_fac;

	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr, zone_itr;
	Doubles factors, *row_ptr;
	Location_Itr loc_itr;

	//---- read the factor matrix into memory ----

	if (!factor_file->Read_Matrix ()) {
		Error ("Reading Factor Matrix");
	}

	if (!move_flag) return;

	num_periods = factor_file->Num_Periods ();
	if (num_periods < 1) num_periods = 1;

	org_map = factor_file->Org_Map ();
	des_map = factor_file->Des_Map ();

	factors.assign (zone_map.size (), 0.0);
	row_factor.assign (num_periods, factors);

	//---- process each period ----

	for (period=0; period < num_periods; period++) {

		row_ptr = &row_factor [period];

		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
			org = org_itr->second;
			zone = org_itr->first;

			zone_itr = zone_map.find (zone);
			if (zone_itr == zone_map.end ()) continue;

			zone = zone_itr->second;

			sum_fac = 0.0;

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				des = des_itr->second;

				factor_file->Get_Cell_Index (period, org, des, 0, factor);

				if (factor > 1.0) {
					sum_fac += (factor - 1.0);
				}
			}
			(*row_ptr) [zone] = sum_fac;
		}
	}

	//---- prepare zone location map ----

	zone_location.assign (zone_array.size (), -1);
	next_location.assign (location_array.size (), -1);

	for (org=0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, org++) {
		if (loc_itr->Zone () >= 0) {
			next_location [org] = zone_location [loc_itr->Zone ()];
			zone_location [loc_itr->Zone ()] = org;
		}
	}
}
