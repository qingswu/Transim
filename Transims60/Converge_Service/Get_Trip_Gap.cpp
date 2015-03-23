//*********************************************************
//	Get_Trip_Gap.cpp - process trip gap data
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Get_Trip_Gap
//---------------------------------------------------------

double Converge_Service::Get_Trip_Gap (void)
{
	int i, num, period;
	double diff, imp, sum_diff, total, gap;
	bool gap_flag;

	Trip_Gap_Map_Itr itr;
	Gap_Sum gap_sum;
	Doubles period_diff, period_sum;
	
	num = 0;
	sum_diff = total = gap = 0.0;

	memset (&gap_sum, '\0', sizeof (gap_sum));

	gap_flag = (trip_gap_flag || trip_report_flag);

	if (gap_flag) {
		num = dat->sum_periods.Num_Periods ();
		period_diff.assign (num, 0);
		period_sum.assign (num, 0);
	}

	//---- process each trip ----

	if (memory_flag) {
		Gap_Data_Itr gap_itr;

		for (gap_itr = gap_data_array.begin (); gap_itr != gap_data_array.end (); gap_itr++) {

			total += imp = gap_itr->current;
			sum_diff += diff = abs (imp - gap_itr->previous);

			gap_itr->previous = gap_itr->current;
			gap_itr->current = 0;

			if (trip_report_flag) {
				gap_sum.count++;
				gap_sum.abs_diff += diff;
				gap_sum.diff_sq += diff * diff;
				gap_sum.current += imp;

				if (imp > 0) {
					gap = diff / imp;
					if (gap > gap_sum.max_gap) gap_sum.max_gap = gap;
				}
			}
			if (gap_flag) {
				period = dat->sum_periods.Period (gap_itr->time);

				if (period >= 0) {
					period_diff [period] += diff;
					period_sum [period] += imp;
				}
			}
		}

	} else {

		for (i=0; i < num_parts; i++) {
			Trip_Gap_Map *trip_gap_map_ptr = trip_gap_map_array [i];

			for (itr = trip_gap_map_ptr->begin (); itr != trip_gap_map_ptr->end (); itr++) {
				total += imp = itr->second.current;
				sum_diff += diff = abs (imp - itr->second.previous);

				itr->second.previous = itr->second.current;
				itr->second.current = 0;

				if (trip_report_flag) {
					gap_sum.count++;
					gap_sum.abs_diff += diff;
					gap_sum.diff_sq += diff * diff;
					gap_sum.current += imp;

					if (imp > 0) {
						gap = diff / imp;
						if (gap > gap_sum.max_gap) gap_sum.max_gap = gap;
					}
				}
				if (gap_flag) {
					period = dat->sum_periods.Period (itr->second.time);

					if (period >= 0) {
						period_diff [period] += diff;
						period_sum [period] += imp;
					}
				}
			}
		}
	}

	//---- process the iteration data ----

	if (total > 0) {
		gap = sum_diff / total;
	} else if (sum_diff > 0) {
		gap = 1.0;
	} else {
		gap = 0.0;
	}

	//---- write the trip gap file ----

	if (gap_flag) {
		for (i=0; i < num; i++) {
			imp = period_sum [i];
			diff = period_diff [i];

			if (imp > 0) {
				diff = diff / imp;
			} else if (diff > 0) {
				diff = 1.0;
			} else {
				diff = 0.0;
			}
			Write_Trip_Gap (diff);
		}
		Write_Trip_Gap (gap, true);

		if (trip_report_flag) {
			trip_gap_array.push_back (gap_sum);
		}
	}
	return (gap);
}
