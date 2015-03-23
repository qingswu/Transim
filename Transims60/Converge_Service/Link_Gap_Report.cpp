//*********************************************************
//	Link_Gap_Report.cpp - print link gap report
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Link_Gap_Report
//---------------------------------------------------------

void Converge_Service::Link_Gap_Report (int report)
{
	int i;
	double gap, max, diff, vht, std_dev, rmse, num, hour;

	Gap_Sum_Itr gap_itr;

	hour = Dtime (1.0, HOURS);

	Header_Number (report);

	if (!Break_Check ((int) link_gap_array.size () + 7)) {
		Print (1);
		Link_Gap_Header ();
	}

	for (i=1, gap_itr = link_gap_array.begin (); gap_itr != link_gap_array.end (); gap_itr++, i++) {
		if (gap_itr->count == 0) continue;

		num = gap_itr->count;
		vht = gap_itr->current;
		diff = gap_itr->abs_diff;
		max = gap_itr->max_gap;

		if (vht > 0.0) {
			gap = diff / vht;
			rmse = 100.0 * sqrt (gap_itr->diff_sq / num) * num / vht;

			if (num > 1) {		
				std_dev = (gap_itr->diff_sq - diff * diff / num) / (num - 1.0);
				std_dev = (std_dev > 0.0) ? sqrt (std_dev) : 0.0;
				std_dev = std_dev * num / vht;
			} else {
				std_dev = 0.0;
			}
			vht /= hour;
			diff /= hour;
		} else {
			gap = 1.0;
			std_dev = rmse = 0.0;
		}

		//---- print the data record ----

		Print (1, String ("%6d   %13.6lf %13.6lf %13.6lf %9.1lf %13.0lf %13.0lf") %
			i % gap % std_dev % max % rmse % diff % vht);
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Link_Gap_Header
//---------------------------------------------------------

void Converge_Service::Link_Gap_Header (void)
{
	Print (1, "Link Gap Report");
	Print (2, "          --------------- Link Gap ---------------      %     ------- PCE Hours -------");
	Print (1, "Iteration        Total       Std.Dev       Maximum     RMSE     Difference         Total");
	Print (1);
}

/*********************************************|***********************************************

	Link Gap Report

              -------------- Link Gap --------------     %     ------- PCE Hours -------
	Iteration        Total      Std.Dev      Maximum    RMSE     Difference        Total

	dddddd    fffff.ffffff fffff.ffffff fffff.ffffff  fffff.f  ffffffffffff ffffffffffff

**********************************************|***********************************************/ 
