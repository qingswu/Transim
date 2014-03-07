//*********************************************************
//	Write_Link_Trips.cpp - Write a Link Trip End File
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Link_Trips
//---------------------------------------------------------

void TripSum::Write_Link_Trips (void)
{
	int i, j, p, link, num_periods, total, count;

	Int_Map_Itr map_itr;
	Integers *trips;

	Show_Message ("Writing Link Trip Ends -- Link");
	Set_Progress ();

	count = 0;
	num_periods = sum_periods.Num_Periods ();

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link = map_itr->second;

		link_trip_file.Put_Field (0, map_itr->first);

		trips = &link_trip_data [link];

		total = 0;
		j = num_periods * 2;

		for (i=0; i < j; i++) {
			total += trips->at (i);
		}
		if (total == 0) continue;

		//---- process each time period ----

		for (i=1, p=j=0; p < num_periods; p++) {
			link_trip_file.Put_Field (i++, trips->at (j++));
			link_trip_file.Put_Field (i++, trips->at (j++));
		}
		link_trip_file.Write ();
		count++;
	}
	End_Progress ();

	Print (2, "Number of New Link Trip End Records = ") << count;
}
