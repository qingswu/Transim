//*********************************************************
//	Apply_Density.cpp - calculate maximum change
//*********************************************************

#include "NewLandUse.hpp"

//---------------------------------------------------------
//	Apply_Density
//---------------------------------------------------------

void NewLandUse::Apply_Density (void)
{
	int num, cover, field, num_fields;
	double density, coverage, density0;

	Doubles area;
	Integers cover_num;
	Dbls_Map_Itr target_itr;
	Zone_Field_Itr field_itr;
	Processing_Itr process_itr, proc_itr;
	Processing_Data *proc_ptr;
	Doubles fields;

	//---- identify coverage types ----

	cover_types = 1;
	cover_num.push_back (0);

	for (num=0, process_itr = processing_steps.begin (); process_itr != processing_steps.end (); process_itr++, num++) {
		for (proc_itr = processing_steps.begin (); proc_itr < process_itr; proc_itr++) {
			if (process_itr->cover0 == proc_itr->cover0 && process_itr->cover1 == proc_itr->cover1) {
				process_itr->cover_num = proc_itr->cover_num;
				break;
			}
		}
		if (process_itr->cover_num == 0) {
			process_itr->cover_num = cover_types++;
			cover_num.push_back (num);
		}
	}

	//---- initialize change array ----

	num_fields = (int) data_names.size ();

	fields.assign (num_fields, 0);

	for (field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++) {
		field_itr->max_change.assign (cover_types, fields);
		field_itr->change.assign (cover_types, fields);
	}

	//---- calculate maximum land use ----

	for (field_itr = zone_field_array.begin (); field_itr != zone_field_array.end (); field_itr++) {
		target_itr = target_density.find (field_itr->target);

		if (target_itr == target_density.end () || field_itr->area <= 0) {
			for (field=0; field < num_fields; field++) {
				field_itr->max_change [0] [field] = field_itr->data [0] [field];
			}
		} else {

			//---- find each coverage type ----

			area.clear ();
			area.push_back (field_itr->area);

			for (cover=1; cover < cover_types; cover++) {
				proc_ptr = &processing_steps [cover_num [cover]];

				coverage = field_itr->coverage [proc_ptr->cover0];
				if (proc_ptr->cover1 >= 0) {
					coverage -= field_itr->coverage [proc_ptr->cover1];
				}
				if (area [0] < coverage) {
					coverage = area [0];
					if (coverage / field_itr->area > 0.05) {
						Warning (String ("Zone %s has More Coverage than Area") % field_itr->zone);
					}
				}
				area.push_back (coverage);
				area [0] -= coverage;
			}

			//---- calculate the maximum land use in each coverage type ----
				
			for (field=0; field < num_fields; field++) {
				density0 = field_itr->data [0] [field] / field_itr->area;
				density = target_itr->second [field];

				if (density > density0) {
					field_itr->max_change [0] [field] = area [0] * density0;
					density -= density0;
					for (cover=1; cover < cover_types; cover++) {
						field_itr->max_change [cover] [field] = density * area [cover];
					}
				} else {
					field_itr->max_change [0] [field] = field_itr->data [0] [field];

					for (cover=1; cover < cover_types; cover++) {
						field_itr->max_change [cover] [field] = density * area [cover];
						field_itr->max_change [0] [field] -= field_itr->max_change [cover] [field];
					}
				}
			}
		}
	}
}
