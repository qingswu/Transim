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
	int period, type, lanes, lanes0, lanes1, cap, tod_cap, len, index, rec, use_index, gap_period, last_period, first_period;
	Dtime time0, tod1, tod, increment;
	double flow_fac, factor1, vht, vht1, vht2, fac1, fac2, gap, sum_vht, diff_vht, period_diff, period_sum, diff, exit_diff;
	bool report_flag, first_flag, last_flag, gap_flag;

	Perf_Period *period_ptr;
	Perf_Period_Itr period_itr;
	Perf_Data *new_ptr, *old_ptr, perf_rec;
	Perf_Itr perf_itr;
	Turn_Period *turn_period_ptr;
	Turn_Period_Itr turn_period_itr;
	Turn_Data *turn_ptr;
	Turn_Itr turn_itr;
	Gap_Sum gap_sum;
	Dir_Itr dir_itr;
	Link_Data *link_ptr;
	Lane_Use_Period *per_ptr;

	report_flag = Report_Flag (LINK_GAP);
	memset (&gap_sum, '\0', sizeof (gap_sum));

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
		first_period = perf_period_array.periods->Period (reroute_time);
	} else {
		first_period = 0;
	}

	//---- initialize the vht totals ----

	vht1 = vht2 = fac1 = fac2 = 0.0;
	first_flag = true;

	if (first_iteration && (!System_File_Flag (PERFORMANCE) || Clear_Flow_Flag ())) {
		factor = 1.0;
		last_flag = true;
	} else {

		for (period=0, period_itr = old_perf_period_array.begin (); period_itr != old_perf_period_array.end (); period_itr++, period++) {
			if (period < first_period) continue;

			for (perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++) {
				vht2 += perf_itr->Veh_Time ();
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

		for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++, tod += increment) {
			if (period < first_period) continue;

			//---- check for a new summary time period ----

			if (last_flag && gap_flag) {
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
			}

			//---- process each link direction ----

			period_ptr = &old_perf_period_array [period];					

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

				perf_rec.Weight_Flows (old_ptr, factor1, new_ptr, factor);
				perf_rec.Time (old_ptr->Time ());
		
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
					if (perf_rec.Volume () > 0) {
						if (lanes0 != lanes) {
							tod_cap = (cap * lanes0 + lanes0 / 2) / lanes;
						} else {
							tod_cap = cap;
						}
						perf_rec.Update_Time (equation.Apply_Equation (type, time0, (perf_rec.Volume () * flow_fac), tod_cap, len));

						vht += perf_rec.Veh_Time ();
					} else {
						perf_rec.Update_Time (time0);
					}
					if (last_flag) {
						if (gap_flag) {
							vht1 = perf_rec.Veh_Time ();
							vht2 = old_ptr->Veh_Time ();
							diff = fabs (vht1 - vht2);
							period_diff += diff;
							period_sum += vht1;

							if (report_flag) {
								gap_sum.count++;
								gap_sum.abs_diff += diff;
								gap_sum.diff_sq += diff * diff;
								gap_sum.current += vht1;
							}
						}
						//if (zero_flag) {
						//	new_ptr->Clear_Flows ();
						//	new_ptr->Time (perf_rec.Time ());
						//} else {
							*new_ptr = perf_rec;
						//}
						*old_ptr = perf_rec;
					}
				}

				//---- managed lanes ----

				use_index = dir_itr->Use_Index ();

				if (use_index >= 0 && lanes1 > 0) {
					new_ptr = period_itr->Data_Ptr (use_index);
					old_ptr = period_ptr->Data_Ptr (use_index);

					perf_rec.Weight_Flows (old_ptr, factor1, new_ptr, factor);
					perf_rec.Time (old_ptr->Time ());

					if (perf_rec.Volume () > 0.0) {
						if (lanes1 != lanes) {
							tod_cap = (cap * lanes1 + lanes1 / 2) / lanes;
						} else {
							tod_cap = cap;
						}
						perf_rec.Update_Time (equation.Apply_Equation (type, time0, (perf_rec.Volume () * flow_fac), tod_cap, len));

						vht += perf_rec.Veh_Time ();
					} else {
						perf_rec.Update_Time (time0);
					}
					if (last_flag) {
						if (gap_flag) {
							vht1 = perf_rec.Veh_Time ();
							vht2 = old_ptr->Veh_Time ();
							diff = fabs (vht1 - vht2);
							period_diff += diff;
							period_sum += vht1;

							if (report_flag) {
								gap_sum.count++;
								gap_sum.abs_diff += diff;
								gap_sum.diff_sq += diff * diff;
								gap_sum.current += vht1;
							}
						}
						if (zero_flag) {
							new_ptr->Clear_Flows ();
							new_ptr->Time (perf_rec.Time ());
						} else {
							*new_ptr = perf_rec;
						}
						*old_ptr = perf_rec;
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
				Write_Link_Gap (gap);
				if (report_flag && gap > gap_sum.max_gap) gap_sum.max_gap = gap;
			}

			//---- factor the turning movements ----

			if (Turn_Flows ()) {

				for (period=0, turn_period_itr = turn_period_array.begin (); turn_period_itr != turn_period_array.end (); turn_period_itr++, period++) {
					if (period < first_period) continue;

					turn_period_ptr = &old_turn_period_array [period];

					//---- process each turn ----

					for (index=0, turn_itr = turn_period_itr->begin (); turn_itr != turn_period_itr->end (); turn_itr++, index++) {
				
						turn_ptr = turn_period_ptr->Data_Ptr (index);

						turn_ptr->Turn (turn_ptr->Turn () * factor1 + turn_itr->Turn () * factor);

						if (zero_flag) {
							turn_itr->Turn (0);
						} else {
							turn_itr->Turn (turn_ptr->Turn ());
						}
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

	//---- update travel times ----

	//Update_Travel_Times (false, zero_flag);
	//num_time_updates++;

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

