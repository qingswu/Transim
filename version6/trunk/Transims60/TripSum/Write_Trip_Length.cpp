//*********************************************************
//	Write_Trip_Length.cpp - Write the Trip Length file
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Trip_Length
//---------------------------------------------------------

void TripSum::Write_Trip_Length (void)
{
	double distance;
	String text;
	Int_Map_Itr map_itr;

	fstream &file = trip_len_file.File ();

	Show_Message ("Writing Time Length File");

	file << "Distance\tTrips\n";

	for (map_itr = len_inc_map.begin (); map_itr != len_inc_map.end (); map_itr++) {
		distance = (double) (map_itr->first * UnRound (len_increment));
		distance = External_Units (distance, distance_units);

		if (distance_units == MILES || distance_units == KILOMETERS) {
			text ("%.2lf") % distance;

			file << text << "\t" << map_itr->second << "\n";
		} else {
			file << (int) distance << "\t" << map_itr->second << "\n";
		}
	}
	trip_time_file.Close ();
	Show_Message (1);
}
