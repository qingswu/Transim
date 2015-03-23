//*********************************************************
//	Calib_Functoins.cpp - calibration trip length functions
//*********************************************************

#include "Gravity.hpp"

#include <math.h>

//---------------------------------------------------------
//	Calib_Functions
//---------------------------------------------------------

void Gravity::Calib_Functions (void)
{
	int i, j, org, des, period, index, num, sum_num, max_func;
	double trips, skim, prod, attr, friction, hi_diff, hi_percent, factor, diff, sum_sq;
	double best_value, tot_distb, sum_min, sum_max, ratio1, ratio2;
	Doubles attr_tot, distb, diff2, *a_ptr, *b_ptr, *c_ptr;
	Dbls_Array *dbls_ptr;
	Integers count;
	Int_Map *org_ptr, *des_ptr;
	Int_Map_Itr org_itr, des_itr;
	Function func;

	org_ptr = skim_file->Org_Map ();
	des_ptr = skim_file->Des_Map ();

	Show_Message ("Calibration Functions -- Iteration");
	Set_Progress ();

	hi_diff = hi_percent = 0.0;
	num = (int) (max_value / increment + 1.5);
	best_value = 0.0;

	for (period = 0; period < num_periods; period++) {

		max_func = 1;
		func = function.Get_Function (max_func);

		dbls_ptr = &constraints [0];

		a_ptr = &dbls_ptr->at (0);
		b_ptr = &dbls_ptr->at (1);
		c_ptr = &dbls_ptr->at (2);

		for (func.a = a_ptr->at (0); func.a <= a_ptr->at (1); func.a += a_ptr->at (2)) {
			for (func.b = b_ptr->at (0); func.b <= b_ptr->at (1); func.b += b_ptr->at (2)) {
				for (func.c = c_ptr->at (0); func.c <= c_ptr->at (1); func.c += c_ptr->at (2)) {
					if (max_func == 1) {
						function.Update_Function (max_func, func);
					} else {
						function.Add_Function (max_func, func);
					}
					max_func++;
				}
			}
		}

		//---- iterate until convergence ----

		for (i=1; i < max_func; i++) {

			Show_Progress ();

			tot_distb = 0.0;
			distb.assign (num, 0.0);
			diff2.assign (num, 0.0);
			count.assign (num, 0);
			sum_num = 0;
			sum_sq = 0;

			//---- distribute productions to attractions ----
			
			attr_tot.assign (des_ptr->size (), 0);

			for (org=0, org_itr = org_ptr->begin (); org_itr != org_ptr->end (); org_itr++, org++) {
				prod = org_tot [org];

				trips = 0.0;

				for (des=0, des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++, des++) {
					attr = des_tot [des];

					skim_file->Get_Cell_Index (period, org_itr->second, des_itr->second, skim_field, skim);
					if (skim <= 0) continue;

					skim *= units_factor;

					friction = function.Apply_Function (i, skim);

					if (friction > 0.0) {
						trips += attr * friction;
					}
				}

				if (trips > 0) {
					factor = prod / trips;
				} else {
					factor = prod;
				}

				for (des=0, des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++, des++) {
					attr = des_tot [des];

					skim_file->Get_Cell_Index (period, org_itr->second, des_itr->second, skim_field, skim);
					if (skim <= 0) continue;

					skim *= units_factor;

					friction = function.Apply_Function (i, skim);

					if (friction > 0.0) {
						trips = factor * attr * friction;

						attr_tot [des] += trips;
					
						if (skim > max_value) skim = max_value;
						index = (int) (skim / increment + 0.5);
						distb [index] += trips;
						tot_distb += trips;

						trip_file->Get_Cell (des_itr->first, trip_field, diff);
						diff = abs (diff - trips);

						diff2 [index] += diff * diff;
						count [index]++;

						sum_num++;
						sum_sq += diff * diff;
					}
				}
			}

			//---- absolute difference -----

			hi_diff = hi_percent = 0;

			for (des=0, des_itr = des_ptr->begin (); des_itr != des_ptr->end (); des_itr++, des++) {
				attr = des_tot [des];
				trips = attr_tot [des];

				diff = abs (attr - trips);

				if (attr > 0) {
					factor = diff / attr;
					if (factor > hi_percent) hi_percent = factor;
				}
				if (diff > hi_diff) hi_diff = diff;
			}
			Break_Check (4);
			Print (2, "Iteration = ") << i;
			Print (1, String ("Maximum Percent Difference = %.2lf%%") % (hi_percent * 100.0) % FINISH);
			Print (1, String ("Maximum Trip Difference = %.2lf") % hi_diff);

			if (sum_num > 1) {
				sum_sq /= sum_num;
			}
			Print (1, String ("Least Squares Error = %.2lf") % sum_sq);

			//---- coincidence ratio ----

			sum_min = sum_max = 0.0;

			for (j=0; j < num; j++) {
				ratio1 = distb [j] / tot_distb;
				ratio2 = in_distb [j] / total_in;

				if (ratio1 > ratio2) {
					sum_max += ratio1;
					sum_min += ratio2;
				} else {
					sum_max += ratio2;
					sum_min += ratio1;
				}
			}
			if (sum_max == 0.0) break;

			ratio1 = sum_min / sum_max;
			Print (1, String ("Coincidence Ratio = %.3lf") % ratio1);

			if (ratio1 > best_value) {
				best_value = ratio1;
				best_function = i;
			}

			func = function.Get_Function (i);
			Print (1, String ("Function Type = %s, A = %.3lf, B = %.3lf, C = %.3lf, D = %.3lf") %
				Function_Code (func.type) % func.a % func.b % func.c % func.d);
		}
		End_Progress ();
	}
	Print (1);
	Write (1, String ("Best Coincidence Ratio = %.3lf, Function=%d") % best_value % best_function);
	
	func = function.Get_Function (best_function);
	Print (1, String ("Function Type = %s, A = %.3lf, B = %.3lf, C = %.3lf, D = %.3lf") %
		Function_Code (func.type) % func.a % func.b % func.c % func.d);
	Show_Message (1);
}
