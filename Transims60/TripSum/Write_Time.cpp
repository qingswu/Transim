//*********************************************************
//	Write_Time.cpp - Write the Trip Time Distribution file
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Time_Distribution
//---------------------------------------------------------

void TripSum::Write_Time_Distribution (void)
{
	int p, start_total, mid_total, end_total, num_periods;
	Dtime low, high, low0;

	fstream &file = diurnal_file.File ();

	Show_Message ("Writing Time Distribution File");

	file << "Time1\tTime2\tTrip_Starts\tMid-Trip\tTrip_Ends\n";

	num_periods = sum_periods.Num_Periods ();
	start_total = mid_total = end_total = 0;

	for (p=0; p < num_periods; p++) {
		sum_periods.Period_Range (p, low, high);
		if (p == 0) low0 = low;

		file << low.Time_String () << "\t" << high.Time_String () << "\t" << start_time [p] << "\t" << mid_time [p] << "\t" << end_time [p] << "\n";

		start_total += start_time [p];
		mid_total += mid_time [p];
		end_total += end_time [p];
	}
	file << low0.Time_String () << "\t" << high.Time_String () << "\t" << start_total << "\t" << mid_total << "\t" << end_total << "\n";

	diurnal_file.Close ();
	Show_Message (1);
}
