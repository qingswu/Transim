//*********************************************************
//	Processing.cpp - factor trip tables
//*********************************************************

#include "Fratar.hpp"

//---------------------------------------------------------
//	Processing
//---------------------------------------------------------

void Fratar::Processing (void)
{
	int i, org, des, period, num_org, num_des, num_periods;
	double trips, total, tot_org, tot_des, factor, fac1, fac2, fac3, diff, abs_diff, max_fac;
	bool flag;

	Margin_Group *org_ptr, *des_ptr, *period_ptr;
	Dbl_Itr dbl_itr;

	num_org = new_file.Num_Org ();
	num_des = new_file.Num_Des ();
	num_periods = new_file.Num_Periods ();

	org_ptr = &margin_group [0];
	des_ptr = &margin_group [1];
	period_ptr = &margin_group [2];

	//---- normalize the targets -----

	tot_org = 0;

	for (dbl_itr = org_ptr->target.begin (); dbl_itr != org_ptr->target.end (); dbl_itr++) {
		tot_org += *dbl_itr;
	}
	tot_des = 0;

	for (dbl_itr = des_ptr->target.begin (); dbl_itr != des_ptr->target.end (); dbl_itr++) {
		tot_des += *dbl_itr;
	}
	total = 0;

	for (dbl_itr = period_ptr->target.begin (); dbl_itr != period_ptr->target.end (); dbl_itr++) {
		total += *dbl_itr;
	}
	trips = (tot_org + tot_des) / 2;

	if (trips == 0) Error ("Trip Margins are Zero");

	factor = trips / tot_org;

	for (dbl_itr = org_ptr->target.begin (); dbl_itr != org_ptr->target.end (); dbl_itr++) {
		*dbl_itr *= factor;
	}
	factor = trips / tot_des;

	for (dbl_itr = des_ptr->target.begin (); dbl_itr != des_ptr->target.end (); dbl_itr++) {
		*dbl_itr *= factor;
	}
	factor = trips / total;

	for (dbl_itr = period_ptr->target.begin (); dbl_itr != period_ptr->target.end (); dbl_itr++) {
		*dbl_itr *= factor;
	}
	total = 0;

	for (period=0; period < num_periods; period++) {
		for (org=0; org < num_org; org++) {
			for (des=0; des < num_des; des++) {
				total += odt_array [period] [org] [des];
			}
		}
	}
	factor = trips / total;

	org_ptr->total.assign (num_org, 0);
	des_ptr->total.assign (num_des, 0);
	period_ptr->total.assign (num_periods, 0);

	for (period=0; period < num_periods; period++) {
		for (org=0; org < num_org; org++) {
			for (des=0; des < num_des; des++) {
				trips = odt_array [period] [org] [des] *= factor;

				period_ptr->total [period] += trips;
				org_ptr->total [org] += trips;
				des_ptr->total [des] += trips;
			}
		}
	}
	Show_Message (1, "Iteration -- Record");
	Set_Progress ();

	max_fac = abs_diff = 0.0;

	for (i=0, flag=false; i < max_iterations && !flag; i++) {
		Show_Progress ();

		total = abs_diff = max_fac = 0;
		flag = true;

		//---- initialize the trip table ----

		for (period=0; period < num_periods; period++) {
			if (period_ptr->total [period] > 0) {
				fac1 = period_ptr->target [period] / period_ptr->total [period];
				factor = fabs (1.0 - fac1);
				if (factor > max_fac) max_fac = factor;
			} else {
				fac1 = factor = 0;
			}
			diff = fabs (period_ptr->target [period] - period_ptr->total [period]);
			if (diff > abs_diff) abs_diff = diff;

			if ((diff > min_diff && factor > percent_diff) || diff > max_diff) flag = false;

			for (org=0; org < num_org; org++) {
				if (org_ptr->total [org] > 0) {
					fac2 = org_ptr->target [org] / org_ptr->total [org];
					factor = fabs (1.0 - fac2);
					if (factor > max_fac) max_fac = factor;
				} else {
					fac2 = factor = 0;
				}
				diff = fabs (org_ptr->target [org] - org_ptr->total [org]);
				if (diff > abs_diff) abs_diff = diff;

				if ((diff > min_diff && factor > percent_diff) || diff > max_diff) flag = false;

				for (des=0; des < num_des; des++) {
					if (des_ptr->total [des] > 0) {
						fac3 = des_ptr->target [des] / des_ptr->total [des];
						factor = fabs (1.0 - fac3);
						if (factor > max_fac) max_fac = factor;
					} else {
						fac3 = factor = 0;
					}
					diff = fabs (des_ptr->target [des] - des_ptr->total [des]);
					if (diff > abs_diff) abs_diff = diff;

					if ((diff > min_diff && factor > percent_diff) || diff > max_diff) flag = false;

					odt_array [period] [org] [des] *= fac1 * fac2 * fac3;
				}
			}
		}

		//---- update the totals ----

		org_ptr->total.assign (num_org, 0);
		des_ptr->total.assign (num_des, 0);
		period_ptr->total.assign (num_periods, 0);

		for (period=0; period < num_periods; period++) {
			for (org=0; org < num_org; org++) {
				for (des=0; des < num_des; des++) {
					trips = odt_array [period] [org] [des];

					period_ptr->total [period] += trips;
					org_ptr->total [org] += trips;
					des_ptr->total [des] += trips;
				}
			}
		}
	}
	End_Progress ();

	Print (1);
	Write (1, "Number of Iterations = ") << i;
	Write (1, String ("Maximum Percent Difference = %.1lf") % (max_fac * 100.0));
	Write (1, String ("Maximum Absolute Difference = %.1lf") % abs_diff);
	Show_Message (1);
}
