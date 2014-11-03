//*********************************************************
//	Merge_Delay.cpp - merge link delay data
//*********************************************************

#include "Router.hpp"

#include <math.h>

//---------------------------------------------------------
//	Merge_Delay
//---------------------------------------------------------

double Router::Merge_Delay (double factor, bool zero_flag)
{
	int period, record, gap_period, last_period, first_period;
	double flow, merge_flow, factor1, gap, vht, diff, merge_vht, diff_vht, sum_vht;
	double period_diff, period_sum;
	bool report_flag;

	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Turn_Itr turn_itr;
	Turn_Data *turn_ptr;
	Turn_Period_Itr turn_period_itr;
	Turn_Period *turn_period_ptr;
	Gap_Sum gap_sum;

	gap = diff_vht = sum_vht = period_diff = period_sum = 0.0;
	
	report_flag = Report_Flag (LINK_GAP);
	memset (&gap_sum, '\0', sizeof (gap_sum));

	if (reroute_flag) {
		first_period = perf_period_array.periods->Period (reroute_time);
	} else {
		first_period = 0;
	}

	//---- factor the volume data ----

	if (factor != 0.0) {

		//---- process each time period ----

		for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {
			if (period < first_period) continue;

			//---- process each link direction ----

			period_ptr = &old_perf_period_array [period];

			for (record=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, record++) {
			
				perf_ptr = period_ptr->Data_Ptr (record);

				if (first_iteration) {
					if (!System_File_Flag (PERFORMANCE) || dat->Clear_Flow_Flag ()) {
						perf_itr->Set_Flows (perf_ptr);
					}
				} else {
					perf_itr->Weight_Flows (perf_ptr, factor);
				}
			}
		}

		//---- factor the turning movements ----

		if (Turn_Flows ()) {
			factor1 = factor + 1.0;

			for (period=0, turn_period_itr = turn_period_array.begin (); turn_period_itr != turn_period_array.end (); turn_period_itr++, period++) {
				if (period < first_period) continue;

				turn_period_ptr = &old_turn_period_array [period];

				//---- process each turn ----

				for (record=0, turn_itr = turn_period_itr->begin (); turn_itr != turn_period_itr->end (); turn_itr++, record++) {
				
					turn_ptr = turn_period_ptr->Data_Ptr (record);

					flow = turn_itr->Turn ();
					merge_flow = turn_ptr->Turn ();

					if (first_iteration) {
						if (!System_File_Flag (TURN_DELAY) || dat->Clear_Flow_Flag ()) {
							merge_flow = flow;
						}
					}
					flow = (merge_flow * factor + flow) / factor1;

					turn_ptr->Turn (flow);

					if (zero_flag) turn_itr->Turn (0.0);
				}
			}
		}
	}

	//---- update the travel time ----

	Update_Travel_Times (1, reroute_time);
	num_time_updates++;
	
	//---- calculate the link gap ----
	
	if (link_gap_flag || link_gap > 0.0 || report_flag) {
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
					Write_Link_Gap (gap);
					if (report_flag && gap > gap_sum.max_gap) gap_sum.max_gap = gap;
				}
				period_diff = period_sum = 0.0;
				last_period = gap_period;
			}

			//---- process each link direction ----

			period_ptr = &old_perf_period_array [period];

			for (record=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, record++) {
			
				perf_ptr = period_ptr->Data_Ptr (record);

				vht = perf_itr->Veh_Time ();
				merge_vht = perf_ptr->Veh_Time ();
				diff = fabs (vht - merge_vht);
				period_diff += diff;
				period_sum += vht;

				if (report_flag) {
					gap_sum.count++;
					gap_sum.abs_diff += diff;
					gap_sum.diff_sq += diff * diff;
					gap_sum.current += vht;
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
			if (report_flag && gap > gap_sum.max_gap) gap_sum.max_gap = gap;
		}

	} else {

		old_perf_period_array.Copy_Flow_Data (perf_period_array, zero_flag);
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

	if (report_flag) {
		link_gap_array.push_back (gap_sum);
	}
	return (gap);
}

