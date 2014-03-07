//*********************************************************
//	Write_Location_Trips.cpp - Write a Location Trip End File
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Location_Trips
//---------------------------------------------------------

void TripSum::Write_Location_Trips (void)
{
	int i, j, p, location, num_periods, total, count;

	Int_Map_Itr map_itr;
	Integers *trips;

	Show_Message ("Writing Location Trip Ends -- Location");
	Set_Progress ();

	count = 0;
	num_periods = sum_periods.Num_Periods ();

	for (map_itr = location_map.begin (); map_itr != location_map.end (); map_itr++) {
		Show_Progress ();

		location = map_itr->second;

		loc_trip_file.Put_Field (0, map_itr->first);

		trips = &loc_trip_data [location];

		total = 0;
		j = num_periods * 2;

		for (i=0; i < j; i++) {
			total += trips->at (i);
		}
		if (total == 0) continue;

		//---- process each time period ----

		for (i=1, p=j=0; p < num_periods; p++) {
			loc_trip_file.Put_Field (i++, trips->at (j++));
			loc_trip_file.Put_Field (i++, trips->at (j++));
		}
		loc_trip_file.Write ();
		count++;
	}
	End_Progress ();

	Print (2, "Number of New Location Trip End Records = ") << count;
}
