//*********************************************************
//	VC_Report.cpp - Create Volume Capacity Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Volume_Capacity_Report
//---------------------------------------------------------

void LinkSum::Volume_Capacity_Report (void)
{
	int i, j, k, k1, vc, percent, index, use_index;
	int percentile [NUM_PERCENTILES];
	double total, sum;
	Dtime low, high;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Data perf_data;
	Doubles_Itr itr;
	Performance_Data data;

	Show_Message ("Creating the V/C Ratio Distribution -- Record");
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
				perf_data = period_itr->Total_Performance (index, use_index);

				if (perf_data.Volume () < minimum_volume) continue;

				sum_periods.Period_Range (j, low, high);

				data.Start (low);
				data.End (high);

				data.Get_Data (&perf_data, dir_ptr, &(*link_itr));

				//---- save the vc ratio ----

				vc = DTOI (data.VC_Ratio () * RESOLUTION);
				if (vc < 0 || vc >= NUM_SUM_BINS) vc = NUM_SUM_BINS - 1;

				sum_bin [j] [vc] += data.Lane_Len ();
				sum_bin [num_inc] [vc] += data.Lane_Len ();
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (VC_RATIOS);

	if (!Break_Check (num_inc + 7)) {
		Print (1);
		Volume_Capacity_Header ();
	}

	for (i=0; i <= num_inc; i++) {

		if (i == num_inc) {
			Print (2, "Total        ");
		} else {
			Print (1, String ("%-12.12s ") % sum_periods.Range_Format (i));
		}

		//---- get the total observations ----

		total = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			total += sum_bin [i] [j];
		}
		if (total == 0.0) continue;

		//---- identify the percentile values ----

        memset (percentile, '\0', sizeof (percentile));

		k1 = 0;
		sum = 0;

		for (j=0; j < NUM_SUM_BINS; j++) {
			sum += sum_bin [i] [j];

			percent = (int) (sum * 100 / total + 0.5);

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
			Print (0, String (" %6.2lf") % ((double) percentile [k] / RESOLUTION));
		}
		Print (0, String ("  %7.0lf") % (total / 1000.0));
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Volume_Capacity_Header
//---------------------------------------------------------

void LinkSum::Volume_Capacity_Header (void)
{
	int i;

	Print (1, "Volume Capacity Ratio Distribution");
	Print (2, String ("%31cVolume / Capacity Ratio") % BLANK);
	Print (1, String ("%15c-------------Percentile Distribution by Time Period----------     Lane") % BLANK);
	Print (1, "Time Period   ");

	for (i=0; i < NUM_PERCENTILES; i++) {
		Print (0, String ("  %3d%% ") % percent_break [i] % FINISH);
	}
	Print (0, "     KM");
	Print (1);
}

/*********************************************|***********************************************

	Volume Capacity Ratio Distribution

                                   Volume / Capacity Ratio
	               -------------Percentile Distribution by Time Period----------    Lane
 	Time Period     50%    60%    65%    70%    75%    80%    85%    95%    99%      KM
	
	xx:xx..xx:xx   dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  dd.dd  ddddddd

	Total       
**********************************************|***********************************************/ 
