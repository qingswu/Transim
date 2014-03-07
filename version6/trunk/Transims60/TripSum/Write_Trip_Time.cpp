//*********************************************************
//	Write_Trip_Time.cpp - Write the Trip Time file
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Write_Trip_Time
//---------------------------------------------------------

void TripSum::Write_Trip_Time (void)
{
	Dtime time;
	Int_Map_Itr map_itr;

	fstream &file = trip_time_file.File ();

	Show_Message ("Writing Time Time File");

	file << "Time\tTrips\n";

	for (map_itr = time_inc_map.begin (); map_itr != time_inc_map.end (); map_itr++) {
		time = map_itr->first * time_increment;

		file << time.Time_String () << "\t" << map_itr->second << "\n";
	}
	trip_time_file.Close ();
	Show_Message (1);
}
