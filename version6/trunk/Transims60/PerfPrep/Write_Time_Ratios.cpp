//*********************************************************
//	Write_Time_Ratios.cpp - write time ratio file
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Write_Time_Ratios
//---------------------------------------------------------

void PerfPrep::Write_Time_Ratios (void)
{
	int i, period, index;
	Dtime low, high;
	double ratio;

	Integers num_links;

	int num = 25;
	double ratios [25] = {
		1.0, 1.1, 1.2, 1.3, 1.5, 1.75, 2.0, 2.5, 3.0, 4.0, 5.0, 7.5, 10.0, 20.0, 30.0, 50.0, 75.0, 100.0, 200.0, 500.0, 1000., 2000.0, 3000.0, 5000.0, 20000.0
	};

	Perf_Period_Itr period_itr;
	Perf_Itr perf_itr;
	Performance_Data data;

	Show_Message ("Writing Time Ratio File -- Record");
	Set_Progress ();

	time_ratio_file.File () << "PERIOD";

	for (i=0; i < num; i++) {
		time_ratio_file.File () << "\t" << ratios [i];
	}

	//---- process each time period ----

	for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {
		perf_period_array.periods->Period_Range (period, low, high);

		data.Start (low);
		data.End (high);

		time_ratio_file.File () << "\n" << high.Time_String ();
		num_links.assign (num, 0);

		//---- process each link direction ----

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			Show_Progress ();

			if (data.Get_Data (&(*perf_itr), index)) {
				ratio = data.Time_Ratio () / 100.0;

				for (i=0; i < num; i++) {
					if (ratio <= ratios [i]) {
						num_links [i]++;
						break;
					}
				}
			}
		}
		for (i=0; i < num; i++) {
			time_ratio_file.File () << "\t" << num_links [i];
		}
	}
	End_Progress ();
}
