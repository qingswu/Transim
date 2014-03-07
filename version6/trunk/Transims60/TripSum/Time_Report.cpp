//*********************************************************
//	Time_Report.cpp - Print the Trip Time Report
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Trip_Time_Report
//---------------------------------------------------------

void TripSum::Trip_Time_Report (void)
{
	int total;
	Dtime time;
	Int_Map_Itr map_itr;

	//---- print the report ----

	Header_Number (TRIP_TIME);

	if (!Break_Check ((int) time_inc_map.size () + 7)) {
		Print (1);
		Trip_Time_Header ();
	}
	total = 0;

	for (map_itr = time_inc_map.begin (); map_itr != time_inc_map.end (); map_itr++) {
		time = map_itr->first * time_increment;

		total += map_itr->second;

		Print (1, String ("%-12.12s %10d") % time.Time_String () % map_itr->second);
	}
	Print (2, String ("Total        %10d") % total);
		
	Header_Number (0);
}

//---------------------------------------------------------
//	Trip_Time_Header
//---------------------------------------------------------

void TripSum::Trip_Time_Header (void)
{
	Print (1, "Trip Time Report");
	Print (2, "Travel_Time       Trips");
	Print (1);
}
	 
/*********************************************|***********************************************

	Trip Time Report

	Travel_Time       Trips
	
	dd:dd:dd     dddddddddd

	Total        dddddddddd

**********************************************|***********************************************/ 
