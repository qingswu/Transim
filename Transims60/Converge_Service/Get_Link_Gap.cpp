//*********************************************************
//	Get_Link_Gap.cpp - process link gap data
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Get_Link_Gap
//---------------------------------------------------------

double Converge_Service::Get_Link_Gap (bool zero_flag)
{
	int period, index, gap_period, last_period, first_period;
	double gap, vht, diff, old_vht, diff_vht, sum_vht;
	double period_diff, period_sum;

	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Gap_Sum gap_sum;

	gap = diff_vht = sum_vht = period_diff = period_sum = 0.0;
	
	memset (&gap_sum, '\0', sizeof (gap_sum));

	if (reroute_time > 0) {
		first_period = dat->perf_period_array.periods->Period (reroute_time);
	} else {
		first_period = 0;
	}
	last_period = -1;

	//---- process each time period ----

	for (period=0, period_itr = dat->perf_period_array.begin (); period_itr != dat->perf_period_array.end (); period_itr++, period++) {
		if (period < first_period) continue;

		//---- check for a new summary time period ----

		gap_period = dat->sum_periods.Period (dat->perf_period_array.periods->Period_Time (period));

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
				Write_Link_Gap (gap);
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

			if (link_report_flag) {
				gap_sum.count++;
				gap_sum.abs_diff += diff;
				gap_sum.diff_sq += diff * diff;
				gap_sum.current += vht;
				gap_sum.previous += old_vht;

				if (vht > 0) {
					gap = diff / vht;
					if (gap > gap_sum.max_gap) gap_sum.max_gap = gap;
				}
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
		Write_Link_Gap (gap);
	}
	
	//---- overall gap ----

	if (sum_vht > 0.0) {
		gap = diff_vht / sum_vht;
	} else if (diff_vht > 0) {
		gap = 1.0;
	} else {
		gap = 0.0;
	}
	Write_Link_Gap (gap, true);

	if (link_report_flag) {
		link_gap_array.push_back (gap_sum);
	}
	return (gap);
}

