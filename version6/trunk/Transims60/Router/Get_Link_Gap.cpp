//*******************ga**************************************
//	Get_Link_Gap.cpp - process link gap data
//*********************************************************

#include "Router.hpp"

#include <math.h>

//---------------------------------------------------------
//	Get_Link_Gap
//---------------------------------------------------------

double Router::Get_Link_Gap (bool zero_flag)
{
	int period, index, gap_period, last_period, first_period;
	double gap, vht, diff, old_vht, diff_vht, sum_vht;
	double period_diff, period_sum;
	bool report_flag;

	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Gap_Sum gap_sum;

	gap = diff_vht = sum_vht = period_diff = period_sum = 0.0;
	
	report_flag = Report_Flag (LINK_GAP);
	memset (&gap_sum, '\0', sizeof (gap_sum));

	if (reroute_flag) {
		first_period = perf_period_array.periods->Period (reroute_time);
	} else {
		first_period = 0;
	}
	last_period = -1;

	//---- process each time period ----

	for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {
		if (period < first_period) continue;

		//---- check for a new summary time period ----

		gap_period = sum_periods.Period (perf_period_array.periods->Period_Time (period));

		if (gap_period != last_period) {
			diff_vht += period_diff;
			sum_vht += period_sum;

			if (last_period >= 0) {
				if (period_sum > 0.0) {
					gap = period_diff / period_sum;
				} else if (period_diff > 0.0) {
					gap = 1.0;
				} else {
					gap = 0.0;
				}
				if (link_gap_flag) link_gap_file.File () << "\t" << gap;
				if (report_flag && gap > gap_sum.max_gap) gap_sum.max_gap = gap;
			}
			period_diff = period_sum = 0.0;
			last_period = gap_period;
		}

		//---- process each link direction ----

		period_ptr = &old_perf_period_array [period];

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			
			perf_ptr = period_ptr->Data_Ptr (index);

			vht = perf_itr->Veh_Time ();
			old_vht = perf_ptr->Veh_Time ();
			diff = fabs (vht - old_vht);
			period_diff += diff;
			period_sum += vht;

			if (report_flag) {
				gap_sum.count++;
				gap_sum.abs_diff += diff;
				gap_sum.diff_sq += diff * diff;
				gap_sum.current += vht;
				gap_sum.previous += old_vht;
			}
			*perf_ptr = *perf_itr;
			if (zero_flag) perf_itr->Clear_Flows ();
		}
	}
	
	//---- process the last summary time period ----

	diff_vht += period_diff;
	sum_vht += period_sum;

	if (last_period >= 0) {
		if (period_sum > 0.0) {
			gap = period_diff / period_sum;
		} else if (period_diff > 0.0) {
			gap = 1.0;
		} else {
			gap = 0.0;
		}
		if (link_gap_flag) link_gap_file.File () << "\t" << gap;
		if (report_flag && gap > gap_sum.max_gap) gap_sum.max_gap = gap;
	}
	
	//---- overall gap ----

	if (sum_vht > 0.0) {
		gap = diff_vht / sum_vht;
	} else if (diff_vht > 0) {
		gap = 1.0;
	} else {
		gap = 0.0;
	}
	if (link_gap_flag) {
		link_gap_file.File () << "\t" << gap << endl;
	}
	if (report_flag) {
		link_gap_array.push_back (gap_sum);
	}
	return (gap);
}
