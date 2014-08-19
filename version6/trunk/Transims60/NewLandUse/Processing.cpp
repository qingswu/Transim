//*********************************************************
//	Processing.cpp - factor trip tables
//*********************************************************

#include "NewLandUse.hpp"

#include <math.h>

//---------------------------------------------------------
//	Processing
//---------------------------------------------------------

void NewLandUse::Processing (int field, Processing_Data &step)
{
	int zone, group;
	double prod;
	bool keep_a, keep_p;
	Int_Set group_codes;
	Int_Set_Itr group_itr;
	Zone_Field_Itr field_itr;

	//---- gather field data ----

	prod_array.assign (num_zones, 0.0);
	attr_array.assign (num_zones, 0.0);

	for (zone=0, field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++, zone++) {

		//---- productions ----

		prod = field_itr->data [step.year0] [field] - field_itr->data [step.year1] [field];
		if (prod < 0) prod = 0;
		if (prod > field_itr->max_change [0] [field]) {
			prod = field_itr->max_change [0] [field];
		}
		if (prod > 0) prod_array [zone] = prod;

		//---- attractions ----

		attr_array [zone] = field_itr->max_change [step.cover_num] [field];
	}

	//---- apply constraints and distribute ----

	if (step.group_num < 0 && step.ptype_num < 0 && step.atype_num < 0) {
		prod_tot = prod_array;
		attr_tot = attr_array;

		Distribute (field, step.step, -1, step.cover_num);

	} else if (step.group_num < 0) {
		prod_tot.assign (num_zones, 0);
		attr_tot.assign (num_zones, 0);

		for (zone=0, field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++, zone++) {
			keep_a = keep_p = true;

			if (step.ptype_num >= 0) {
				if (step.ptype_logic == 1) {
					if (field_itr->type [step.ptype_num] != 0) keep_p = false;
				} else if (step.ptype_logic == 0) {
					if (field_itr->type [step.ptype_num] == 0) keep_p = false;
				}
			}
			if (step.atype_num >= 0) {
				if (step.atype_logic == 1) {
					if (field_itr->type [step.atype_num] != 0) keep_a = false;
				} else if (step.atype_logic == 0) {
					if (field_itr->type [step.atype_num] == 0) keep_a = false;
				}
			}
			if (keep_p) prod_tot [zone] = prod_array [zone];
			if (keep_a) attr_tot [zone] = attr_array [zone];
		}
		Distribute (field, step.step, -1, step.cover_num);

	} else {

		//---- count the number of codes in the group ----

		for (field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++) {
			group_codes.insert (field_itr->group [step.group_num]);
		}
		if (group_codes.size () == 0) return;
		
		for (group_itr = group_codes.begin (); group_itr != group_codes.end (); group_itr++) {
			if (*group_itr < 0) continue;
	
			prod_tot.assign (num_zones, 0.0);
			attr_tot.assign (num_zones, 0.0);

			for (zone=0, field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++, zone++) {
				group = field_itr->group [step.group_num];

				keep_a = keep_p = true;

				if (step.ptype_num >= 0) {
					if (step.ptype_logic == 1) {
						if (field_itr->type [step.ptype_num] != 0) keep_p = false;
					} else if (step.ptype_logic == 0) {
						if (field_itr->type [step.ptype_num] == 0) keep_p = false;
					}
				}
				if (step.atype_num >= 0) {
					if (step.atype_logic == 1) {
						if (field_itr->type [step.atype_num] != 0) keep_a = false;
					} else if (step.atype_logic == 0) {
						if (field_itr->type [step.atype_num] == 0) keep_a = false;
					}
				}

				if (group == *group_itr && keep_a) {
					attr_tot [zone] = attr_array [zone];
				}
				if (step.group_logic == 1) {
					if (group != *group_itr) continue;
				} else if (step.group_logic == 0) {
					if (group == *group_itr) continue;
				}
				if (keep_p) prod_tot [zone] = prod_array [zone];
			}
			if (Distribute (field, step.step, *group_itr, step.cover_num)) {
				for (zone=0; zone < num_zones; zone++) {
					prod_array [zone] -= prod_tot [zone];
					if (prod_array [zone] < 0) prod_array [zone] = 0;

					attr_array [zone] -= attr_tot [zone];
					if (attr_array [zone] < 0) attr_array [zone] = 0;
				}
			}
		}
	}
}
