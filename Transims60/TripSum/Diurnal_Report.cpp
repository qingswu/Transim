//*********************************************************
//	Diurnal_Report.cpp - Print the Time Distribution
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Trip_Diurnal_Report
//---------------------------------------------------------

void TripSum::Trip_Diurnal_Report (void)
{
	int p, start_total, mid_total, end_total, num_periods;
	double start, mid, end;

	//---- get the total number of trips ----

	num_periods = sum_periods.Num_Periods ();
	start_total = mid_total = end_total = 0;

	for (p=0; p < num_periods; p++) {
		start_total += start_time [p];
		mid_total += mid_time [p];
		end_total += end_time [p];
	}
	if (start_total == 0 && mid_total == 0 && end_total == 0) {
		Write (2, "Time Distribution Data is Zero");
		return;
	}
	if (start_total == 0) {
		start = 1.0;
	} else {
		start = start_total;
	}
	if (mid_total == 0) {
		mid = 1.0;
	} else {
		mid = mid_total;
	}
	if (end_total == 0) {
		end = 1.0;
	} else {
		end = end_total;
	}

	//---- print the report ----

	Header_Number (TRIP_DIURNAL);

	if (!Break_Check (num_periods + 7)) {
		Print (1);
		Trip_Time_Header ();
	}		

	for (p=0; p < num_periods; p++) {
		Print (1, String ("%-12.12s   %10d   %6.2lf   %10d   %6.2lf   %10d   %6.2lf") % 
			sum_periods.Range_Format (p) %
			start_time [p] % (100.0 * start_time [p] / start) %
			mid_time [p] % (100.0 * mid_time [p] / mid) %
			end_time [p] % (100.0 * end_time [p] / end));
	}
	Print (2, String ("Total          %10d   100.00   %10d   100.00   %10d   100.00") % start_total % mid_total % end_total);
		
	Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Diurnal_Header
//---------------------------------------------------------

void TripSum::Trip_Diurnal_Header (void)
{
	Print (1, "Time Distribution Report");
	Print (2, "Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent");
	Print (1);
}
	 
/*********************************************|***********************************************

	Time Distribution Report

	Time-of-Day    Start-Trip   Percent    Mid-Trip  Percent     End-Trip  Percent

	dd:dd..dd:dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

	Total          dddddddddd   ddd.dd   dddddddddd   ddd.dd   dddddddddd   ddd.dd

**********************************************|***********************************************/ 
