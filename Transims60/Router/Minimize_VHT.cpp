//*********************************************************
//	Minimize_VHT.cpp - minimize vehicle hours of travel
//*********************************************************

#include "Router.hpp"

#include <math.h>

//---------------------------------------------------------
//	Minimize_VHT
//---------------------------------------------------------

double Router::Minimize_VHT (double &factor, bool zero_flag)
{
	int period, type, lanes, lanes0, lanes1, cap, tod_cap, len, index, rec, flow_index, gap_period, last_period, first_period;
	Dtime time, time0, tod1, tod, increment;
	double flow, flow_fac, factor1, vht, vht1, vht2, fac1, fac2, gap, sum_vht, diff_vht, period_diff, period_sum, diff, exit_diff;
	bool report_flag, first_flag, last_flag, gap_flag;

	Flow_Time_Array *period_ptr, *delay_ptr;
	Flow_Time_Data *new_ptr, *old_ptr, *record_ptr;
	Flow_Time_Period_Itr period_itr;
	Flow_Time_Itr record_itr;
	Gap_Data gap_data;
	Dir_Itr dir_itr;
	Link_Data *link_ptr;
	Lane_Use_Period *per_ptr;

	report_flag = Report_Flag (LINK_GAP);
	memset (&gap_data, '\0', sizeof (gap_data));

	increment = time_periods.Increment ();
	if (increment < 1) increment = 1;

	tod1 = Model_Start_Time () + increment / 2;
	flow_fac = (double) Dtime (60, MINUTES) / increment;
	
	gap = diff_vht = sum_vht = period_diff = period_sum = 0.0;
	last_period = -1;	
	
	gap_flag = (link_gap_flag || link_gap > 0.0 || report_flag);

	if (link_gap > 0.0) {
		exit_diff = MIN (link_gap, 0.005);
	} else {
		exit_diff = 0.005;
	}
	if (reroute_flag) {
		first_period = link_delay_array.periods->Period (reroute_time);
	} else {
		first_period = 0;
	}

	//---- initialize the vht totals ----

	vht1 = vht2 = fac1 = fac2 = 0.0;
	first_flag = true;

	if (first_iteration && (!System_File_Flag (LINK_DELAY) || Clear_Flow_Flag ())) {
		factor = 1.0;
		last_flag = true;
	} else {

		for (period=0, period_itr = old_link_array.begin (); period_itr != old_link_array.end (); period_itr++, period++) {
			if (period < first_period) continue;

			for (record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++) {
				vht2 += record_itr->Flow () * record_itr->Time ();
			}
		}
		fac1 = factor * 1.5;

		if (fac1 > 1.0 || fac1 <= 0.0) {
			fac1 = 1.0;
		}
		factor = fac1;
		last_flag = false;
	}
	
	//---- bisection search to find the minimum vht ----

	for (;;) {

		factor1 = 1.0 - factor;
		vht = 0.0;

		for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++, tod += increment) {
			if (period < first_period) continue;

			//---- check for a new summary time period ----

			if (last_flag && gap_flag) {
				gap_period = sum_periods.Period (link_delay_array.periods->Period_Time (period));

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
						if (report_flag && gap > gap_data.max_gap) gap_data.max_gap = gap;
					}
					period_diff = period_sum = 0.0;
					last_period = gap_period;
				}
			}

			//---- process each link direction ----

			period_ptr = &old_link_array [period];					

			for (index=0, dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, index++) {
				link_ptr = &link_array [dir_itr->Link ()];

				len = link_ptr->Length ();
				type = link_ptr->Type ();

				time0 = dir_itr->Time0 ();
				cap = dir_itr->Capacity ();
				lanes = dir_itr->Lanes ();
				if (lanes < 1) lanes = 1;
				tod = tod1;

				new_ptr = period_itr->Data_Ptr (index);
				old_ptr = period_ptr->Data_Ptr (index);

				flow = old_ptr->Flow () * factor1 + new_ptr->Flow () * factor;
			
				//---- check for time of day restrictions ----

				lanes0 = lanes;
				lanes1 = 0;
				rec = dir_itr->First_Lane_Use ();

				if (rec >= 0) {
					for (;; rec++) {
						per_ptr = &use_period_array [rec];

						if (per_ptr->Start () <= tod && tod < per_ptr->End ()) {
							lanes0 = per_ptr->Lanes0 ();
							lanes1 = per_ptr->Lanes1 ();
							break;
						}
						if (per_ptr->Periods () == 0) break;
					}
				}

				//---- general purpose lanes ----

				if (lanes0 > 0) {
					if (flow > 0) {
						if (lanes0 != lanes) {
							tod_cap = (cap * lanes0 + lanes0 / 2) / lanes;
						} else {
							tod_cap = cap;
						}
						time = equation.Apply_Equation (type, time0, (flow * flow_fac), tod_cap, len);
					
						vht += flow * time;
					} else {
						time = time0;
					}
					if (last_flag) {
						if (gap_flag) {
							vht1 = flow * time;
							vht2 = old_ptr->Flow () * old_ptr->Time ();
							diff = fabs (vht1 - vht2);
							period_diff += diff;
							period_sum += vht1;

							if (report_flag) {
								gap_data.count++;
								gap_data.diff += diff;
								gap_data.diff_sq += diff * diff;
								gap_data.total += vht1;
							}
						}
						new_ptr->Flow ((zero_flag) ? 0 : flow);
						new_ptr->Time (time);

						old_ptr->Flow (flow);
						old_ptr->Time (time);
					}
				}

				//---- managed lanes ----

				flow_index = dir_itr->Flow_Index ();

				if (flow_index >= 0 && lanes1 > 0) {
					new_ptr = period_itr->Data_Ptr (flow_index);
					old_ptr = period_ptr->Data_Ptr (flow_index);

					flow = old_ptr->Flow () * factor1 + new_ptr->Flow () * factor;

					if (flow > 0.0) {
						if (lanes1 != lanes) {
							tod_cap = (cap * lanes1 + lanes1 / 2) / lanes;
						} else {
							tod_cap = cap;
						}
						time = equation.Apply_Equation (type, time0, (flow * flow_fac), tod_cap, len);

						vht += flow * time;
					} else {
						time = time0;
					}
					if (last_flag) {
						if (gap_flag) {
							vht1 = flow * time;
							vht2 = old_ptr->Flow () * old_ptr->Time ();
							diff = fabs (vht1 - vht2);
							period_diff += diff;
							period_sum += vht1;

							if (report_flag) {
								gap_data.count++;
								gap_data.diff += diff;
								gap_data.diff_sq += diff * diff;
								gap_data.total += vht1;
							}
						}
						new_ptr->Flow ((zero_flag) ? 0 : flow);
						new_ptr->Time (time);

						old_ptr->Flow (flow);
						old_ptr->Time (time);
					}
				}
			}
		}
		if (last_flag) {
	
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
				if (report_flag && gap > gap_data.max_gap) gap_data.max_gap = gap;
			}

			//---- factor the turning movements ----

			if (Turn_Flows ()) {

				for (period=0, period_itr = turn_delay_array.begin (); period_itr != turn_delay_array.end (); period_itr++, period++) {
					if (period < first_period) continue;

					period_ptr = &old_turn_array [period];

					//---- process each turn ----

					for (index=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, index++) {
				
						record_ptr = period_ptr->Data_Ptr (index);

						flow = record_ptr->Flow () * factor1 + record_itr->Flow () * factor;

						record_ptr->Flow (flow);

						record_itr->Flow ((zero_flag) ? 0.0 : flow);
					}
				}
			}

			//---- factor the link persons ----

			if (link_person_flag) {
				for (period=0, period_itr = link_person_array.begin (); period_itr != link_person_array.end (); period_itr++, period++) {
					if (period < first_period) continue;

					period_ptr = &old_person_array [period];
					delay_ptr = &link_delay_array [period];

					//---- process each turn ----

					for (index=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, index++) {
				
						record_ptr = period_ptr->Data_Ptr (index);
						new_ptr = delay_ptr->Data_Ptr (index);

						flow = record_ptr->Flow () * factor1 + record_itr->Flow () * factor;

						record_ptr->Flow (flow);

						record_itr->Flow ((zero_flag) ? 0.0 : flow);
						record_itr->Time (new_ptr->Time ());
					}
				}				
			}

			//---- factor the link vehicles ----

			if (link_vehicle_flag) {
				for (period=0, period_itr = link_vehicle_array.begin (); period_itr != link_vehicle_array.end (); period_itr++, period++) {
					if (period < first_period) continue;

					period_ptr = &old_vehicle_array [period];
					delay_ptr = &link_delay_array [period];

					//---- process each turn ----

					for (index=0, record_itr = period_itr->begin (); record_itr != period_itr->end (); record_itr++, index++) {
				
						record_ptr = period_ptr->Data_Ptr (index);
						new_ptr = delay_ptr->Data_Ptr (index);

						flow = record_ptr->Flow () * factor1 + record_itr->Flow () * factor;

						record_ptr->Flow (flow);

						record_itr->Flow ((zero_flag) ? 0.0 : flow);
						record_itr->Time (new_ptr->Time ());
					}
				}				
			}
			break;
		}
		if (first_flag) {
			vht1 = vht;
			first_flag = false;
		} else if (vht1 > vht2) {
			vht1 = vht;
			fac1 = factor;
		} else {
			vht2 = vht;
			fac2 = factor;
		}
		factor = (fac1 + fac2) / 2.0;

		last_flag = ((fac1 - fac2) < exit_diff);
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
		link_gap_array.push_back (gap_data);
	}
	return (gap);
}

