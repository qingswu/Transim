//*********************************************************
//	Write_Zone_Trips.cpp - Write a Zone Trip End File
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Zone_Trips
//---------------------------------------------------------

void TripSum::Write_Zone_Trips (void)
{
	int i, i1, j, p, zone, num_periods, total, count;
	bool flag;

	Int_Map_Itr map_itr;
	Integers *trips;
	Zone_Data *zone_ptr;

	Show_Message ("Writing Zone Trip Ends -- Zone");
	Set_Progress ();

	if (System_File_Flag (ZONE)) {
		i1 = 3;
		flag = true;
	} else {
		i1 = 1;
		flag = false;
	}
	count = 0;
	num_periods = sum_periods.Num_Periods ();

	for (map_itr = zone_map.begin (); map_itr != zone_map.end (); map_itr++) {
		Show_Progress ();

		zone = map_itr->second;

		zone_trip_file.Put_Field (0, map_itr->first);

		if (flag) {
			zone_ptr = &zone_array [zone];
			zone_trip_file.Put_Field (1, UnRound (zone_ptr->X ()));
			zone_trip_file.Put_Field (2, UnRound (zone_ptr->Y ()));
		}
		trips = &zone_trip_data [zone];

		total = 0;
		j = num_periods * 2;

		for (i=0; i < j; i++) {
			total += trips->at (i);
		}
		if (total == 0) continue;

		//---- process each time period ----

		for (i=i1, p=j=0; p < num_periods; p++) {
			zone_trip_file.Put_Field (i++, trips->at (j++));
			zone_trip_file.Put_Field (i++, trips->at (j++));
		}
		zone_trip_file.Write ();
		count++;
	}
	End_Progress ();

	Print (2, "Number of New Zone Trip End Records = ") << count;
}
