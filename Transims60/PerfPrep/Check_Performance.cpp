//*********************************************************
//	Check_Performance.cpp - check performance values
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Check_Performance
//---------------------------------------------------------

void PerfPrep::Check_Performance (void)
{
	int period, index, max_ratio;
	Dtime low, high;

	Perf_Period_Itr period_itr;
	Perf_Itr perf_itr;
	Performance_Data data;
	Constraint_Itr itr;

	Show_Message ("Checking Performance -- Record");
	Set_Progress ();

	//---- process each time period ----

	for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {
		perf_period_array.periods->Period_Range (period, low, high);

		data.Start (low);
		data.End (high);

		max_ratio = Maximum_Time_Ratio ();

		if (constraint_flag) {
			for (itr = constraint_array.begin (); itr != constraint_array.end (); itr++) {
				if (high <= itr->period) {
					max_ratio = itr->max_ratio;
					break;
				}
			}
		}

		//---- process each link direction ----

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			Show_Progress ();

			if (data.Get_Data (&(*perf_itr), index, max_ratio)) {
				perf_itr->Time (data.Time ());
				perf_itr->Persons (data.Persons ());
				perf_itr->Volume (data.Volume ());
				perf_itr->Enter (data.Enter ());
				perf_itr->Exit (data.Exit ());
				perf_itr->Max_Volume (data.Volume ());
				perf_itr->Queue (data.Queue ());
				perf_itr->Max_Queue (data.Max_Queue ());
				perf_itr->Failure (data.Failure ());
				perf_itr->Veh_Dist (Round (data.Veh_Dist ()));
				perf_itr->Veh_Time (data.Veh_Time ());
				perf_itr->Ratio_Dist (Round (data.Ratio_Dist ()));
				perf_itr->Ratio_Time (data.Ratio_Time ());
				perf_itr->Ratios (data.Ratios ());
				perf_itr->Count (data.Count ());
			} else {
				perf_itr->Clear ();
			}
		}
	}

	if (merge_flag) {
		//---- process each time period ----

		for (period=0, period_itr = merge_perf_array.begin (); period_itr != merge_perf_array.end (); period_itr++, period++) {
			merge_perf_array.periods->Period_Range (period, low, high);

			data.Start (low);
			data.End (high);

			max_ratio = Maximum_Time_Ratio ();

			if (constraint_flag) {
				for (itr = constraint_array.begin (); itr != constraint_array.end (); itr++) {
					if (high <= itr->period) {
						max_ratio = itr->max_ratio;
						break;
					}
				}
			}

			//---- process each link direction ----

			for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
				Show_Progress ();

				if (data.Get_Data (&(*perf_itr), index, max_ratio)) {
					perf_itr->Time (data.Time ());
					perf_itr->Persons (data.Persons ());
					perf_itr->Volume (data.Volume ());
					perf_itr->Enter (data.Enter ());
					perf_itr->Exit (data.Exit ());
					perf_itr->Max_Volume (data.Volume ());
					perf_itr->Queue (data.Queue ());
					perf_itr->Max_Queue (data.Max_Queue ());
					perf_itr->Failure (data.Failure ());
					perf_itr->Veh_Dist (Round (data.Veh_Dist ()));
					perf_itr->Veh_Time (data.Veh_Time ());
					perf_itr->Ratio_Dist (Round (data.Ratio_Dist ()));
					perf_itr->Ratio_Time (data.Ratio_Time ());
					perf_itr->Ratios (data.Ratios ());
					perf_itr->Count (data.Count ());
				} else {
					perf_itr->Clear ();
				}
			}
		}
	}
	End_Progress ();
}
