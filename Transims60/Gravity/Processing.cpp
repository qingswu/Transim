//*********************************************************
//	Processing.cpp - factor trip tables
//*********************************************************

#include "Gravity.hpp"

#include <math.h>

//---------------------------------------------------------
//	Processing
//---------------------------------------------------------

void Gravity::Processing (void)
{
	int i, org, des, period, num_org, num_des, index;
	double trips, skim, prod, attr, friction, hi_diff, hi_percent, factor, diff;
	Doubles attr_fac, prod_tot, attr_tot, prod_bal, attr_bal, *prod_ptr, *attr_ptr;

	new_file->Copy_OD_Map (skim_file);
	new_file->Allocate_Data (true);

	num_org = new_file->Num_Org ();
	num_des = new_file->Num_Des ();

	Show_Message ("Distribute Trips -- Iteration");
	Set_Progress ();

	hi_diff = hi_percent = 0.0;

	prod_ptr = &prod_array;
	attr_ptr = &attr_array;

	for (period = 0; period < num_periods; period++) {
		if (period_flag) {
			prod_ptr = &prod_periods [period];
			attr_ptr = &attr_periods [period];
		}

		//---- balance production and attractions ----

		prod = attr = 0;

		for (org = 0; org < num_org; org++) {
			prod += prod_ptr->at (org);
		}
		for (des = 0; des < num_des; des++) {
			attr += attr_ptr->at (des);
		}
		prod_bal.assign (num_org, 0.0);
		attr_bal.assign (num_des, 0.0);

		if (balance == PROD_BAL) {
			trips = prod;
		} else if (balance == ATTR_BAL) {
			trips = attr;
		} else {
			trips = (prod + attr) / 2;
		}

		if (prod > 0) {
			factor = trips / prod;

			for (org = 0; org < num_org; org++) {
				prod_bal [org] = prod_ptr->at (org) * factor;
			}
		}
		if (attr > 0) {
			factor = trips / attr;

			for (des = 0; des < num_des; des++) {
				attr_bal [des] = attr_ptr->at (des) * factor;
			}
		}
		attr_fac.assign (num_des, 1.0);

		//---- iterate until convergence ----

		for (i=0; i < max_iterations; i++) {
			Show_Progress ();
		
			if (out_len_flag) {
				out_distb.assign ((int) (max_value / increment + 1.5), 0);
			}

			//---- distribute productions to attractions ----
			
			attr_tot.assign (num_des, 0);

			for (org = 0; org < num_org; org++) {
				prod = prod_bal [org];

				trips = 0.0;

				for (des = 0; des < num_des; des++) {
					attr = attr_bal [des];
					
					skim_file->Get_Cell_Index (period, org, des, skim_field, skim);
					if (skim <= 0) continue;

					skim *= units_factor;

					friction = function.Apply_Function (best_function, skim);

					if (friction > 0.0) {
						trips += attr * attr_fac [des] * friction;
					}
				}

				if (trips > 0) {
					factor = prod / trips;
				} else {
					factor = prod;
				}

				for (des = 0; des < num_des; des++) {
					attr = attr_bal [des];

					skim_file->Get_Cell_Index (period, org, des, skim_field, skim);
					if (skim <= 0) continue;

					skim *= units_factor;

					friction = function.Apply_Function (best_function, skim);

					if (friction > 0.0) {
						trips = factor * attr * attr_fac [des] * friction;

						attr_tot [des] += trips;

						new_file->Set_Cell_Index (period, org, des, 0, trips);

						if (out_len_flag) {
							if (skim > max_value) skim = max_value;
							index = (int) (skim / increment + 0.5);
							out_distb [index] += trips;
						}
					}
				}
			}

			//---- absolute difference -----

			hi_diff = hi_percent = 0;

			for (des = 0; des < num_des; des++) {
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
		Print (2, "Number of Iterations = ") << i;
		Print (1, String ("Maximum Percent Difference = %.2lf%%") % (hi_percent * 100.0) % FINISH);
		Print (1, String ("Maximum Trip Difference = %.2lf") % hi_diff);
		Print (1);
	}
	new_file->Write_Matrix ();
}
