//*********************************************************
//	Merge_Performance.cpp - merge performance data
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Merge_Performance
//---------------------------------------------------------

void PerfPrep::Merge_Performance (void)
{
	int period, index, rec;
	double volume, merge_vol, ratio;
	bool factor_flag;

	Perf_Itr perf_itr;
	Perf_Data *perf_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Dir_Data *dir_ptr;

	//---- merge performance files ----

	Show_Message ("Merging Performance Data -- Record");
	Set_Progress ();

	if (method == SIMPLE_LINK_AVG) {
		factor = 1.0;
		factor_flag = false;
	} else {
		factor_flag = (factor == 0.0);
	}

	//---- process each time period ----

	for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {

		period_ptr = &merge_perf_array [period];

		//---- process each link direction ----

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			Show_Progress ();

			volume = perf_itr->Volume ();

			if (method == REPLACE_LINKS && volume > 0.0) continue;

			perf_ptr = period_ptr->Data_Ptr (index);

			if (method == ADD_FLOWS) {
				perf_itr->Add_Flows (perf_ptr);
				perf_itr->Time (perf_ptr->Time ());
			} else if (method == REPLACE_LINKS || (method == REPLACE_AVERAGE && volume == 0.0)) {
				perf_itr->Set_Flows (perf_ptr);
				perf_itr->Time (perf_ptr->Time ());
			} else if (factor_flag) {
				merge_vol = perf_ptr->Volume ();

				if (volume > 0) {
					factor = merge_vol / volume;
				} else {
					factor = merge_vol;
				}
				perf_itr->Weight_Flows (perf_ptr, factor);
				perf_itr->Weight_Time (perf_ptr->Time (), factor);
			} else {
				perf_itr->Average_Flows (perf_ptr);
				perf_itr->Average_Time (perf_ptr->Time ());
			}
		}
	}
	End_Progress ();

	//---- apply max travel time ratio ----

	if (!ratio_flag) return;

	Show_Message ("Checking Travel Time Ratio -- Record");
	Set_Progress ();

	for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {

		//---- process each link direction ----

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			Show_Progress ();

			if (index < (int) dir_array.size ()) {
				rec = index;
			} else {
				rec = lane_use_flow_index [index - (int) dir_array.size ()];
			}
			dir_ptr = &dir_array [rec];
			
			if (dir_ptr->Time0 () <= 0) continue;

			ratio = (double) perf_itr->Time () / dir_ptr->Time0 ();
			if (ratio <= time_ratio) continue;

			ratio = time_ratio / ratio;

			perf_itr->Time (DTOI (perf_itr->Time () * ratio));
		}
	}
	End_Progress ();
}

