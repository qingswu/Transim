//*********************************************************
//	Time_Change.cpp - Create Travel Time Change Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Travel_Time_Change
//---------------------------------------------------------

void LinkSum::Travel_Time_Change (void)
{
	int i, j, k, k1, bin, percent, time, old_time, index, use_index;
	int percentile [NUM_PERCENTILES];
	double total, sum;
	Dtime low, high;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Perf_Data perf_data;
	Doubles_Itr itr;
	Performance_Data data;

	Show_Message ("Summarize Travel Time Changes -- Record");
	Set_Progress ();

	//---- clear the summary bins -----

	for (itr = sum_bin.begin (); itr != sum_bin.end (); itr++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}	

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		for (i=0; i < 2; i++) {
			if (i == 1) {
				if (link_itr->Use () == -1) continue;
				index = link_itr->BA_Dir ();
			} else {
				if (link_itr->Use () == -2) continue;
				index = link_itr->AB_Dir ();
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];
			use_index = dir_ptr->Use_Index ();

			//---- process each time period ----

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {
				sum_periods.Period_Range (j, low, high);

				data.Start (low);
				data.End (high);

				perf_data = period_itr->Total_Performance (index, use_index);

				if (data.Get_Data (&perf_data, dir_ptr, &(*link_itr), Maximum_Time_Ratio ())) {

					time = data.Time ();

					period_ptr = &compare_perf_array [j];

					perf_data = period_ptr->Total_Performance (index, use_index);
				
					old_time = perf_data.Time ();
					if (perf_data.Volume () < minimum_volume) continue;
					if (old_time == 0) old_time = 1;

					bin = abs (time - old_time) * 10 * RESOLUTION / old_time;

					if (bin < 0 || bin >= NUM_SUM_BINS) bin = NUM_SUM_BINS - 1;

					sum_bin [j] [bin] += data.Lane_Len ();
					sum_bin [num_inc] [bin] += data.Lane_Len ();
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (TIME_CHANGE);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Time_Change_Header ();
	}

	for (i=0; i <= num_inc; i++) {

		//---- get the total observations ----

		total = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			total += sum_bin [i] [j];
		}
		if (total == 0.0) continue;

		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, String ("%-12.12s ") % sum_periods.Range_Format (i));
		}

		//---- identify the percentile values ----

        memset (percentile, '\0', sizeof (percentile));

		k1 = 0;
		sum = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			sum += sum_bin [i] [j];

			percent = (int) (sum * 100.0 / total + 0.5);

			for (k=k1; k < NUM_PERCENTILES; k++) {
				if (percent_break [k] <= percent) {
					percentile [k] = j;
					k1 = k + 1;
				} else {
					break;
				}
			}
		}

		//---- print the percentiles ----

		for (k=0; k < NUM_PERCENTILES; k++) {
			Print (0, String (" %6.1lf") % ((double) percentile [k] / 10.0));
		}
		if (Metric_Flag ()) {
			total /= 1000.0;
		} else {
			total /= MILETOFEET;
		}
		Print (0, String ("  %7.0lf") % total);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Time_Change_Header
//---------------------------------------------------------

void LinkSum::Time_Change_Header (void)
{
	int i;

	Print (1, "Time Change Distribution");
	Print (2, String ("%29cTime Change * 100 / Previous Time") % BLANK);
	Print (1, String ("%15c-------------Percentile Distribution by Time Period----------     Lane") % BLANK);
	Print (1, "Time Period   ");

	for (i=0; i < NUM_PERCENTILES; i++) {
		Print (0, String ("  %3d%% ") % percent_break [i] % FINISH);
	}
	if (Metric_Flag ()) {
		Print (0, "     KM");
	} else {
		Print (0, "     MI");
	}
	Print (1);
}

/*********************************************|***********************************************

	Time Change Distribution

                                 Time Change * 100 / Previous Time
	               -------------Percentile Distribution by Time Period----------    Lane
	Time Period     50%    60%    65%    70%    75%    80%    85%    95%    99%      KM
	
	xx:xx..xx:xx   dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  ddddddd

	Total 

**********************************************|***********************************************/ 
