//*********************************************************
//	Write_Boardings.cpp - write stop boarding file
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Boardings
//---------------------------------------------------------

void PlanSum::Write_Boardings (void)
{
	int i, p, num_periods, stop, total;

	Integers trips;
	Ints_Itr ints_itr;
	Int_Itr int_itr;
	Int_Map_Itr map_itr;
	Int_Set *list;
	Int_Set_Itr list_itr;

	Show_Message ("Writing Stop Boardings -- Record");
	Set_Progress ();

	num_periods = sum_periods.Num_Periods ();
	if (num_periods < 1) num_periods = 1;

	fstream &file = boarding_file.File ();

	//---- print the header ----

	file << "Stop";
	if (Notes_Name_Flag ()) {
		file << "\tName";
	}
	for (p=0; p < num_periods; p++) {
		file << "\t" + sum_periods.Range_Label (p);
	}
	if (num_periods > 1) {
		file << "\tTotal";
	}
	file << endl;

	for (i = stop_equiv.First_Group (); i > 0; i = stop_equiv.Next_Group ()) {
		Show_Progress ();

		list = stop_equiv.Group_List (i);
		if (list == NULL) continue;

		trips.assign (num_periods, 0);

		//---- process each stop in the stop group ----

		for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {
			map_itr = stop_map.find (*list_itr);
			if (map_itr == stop_map.end ()) continue;

			stop = map_itr->second;

			for (p=0, ints_itr = boardings.begin (); ints_itr != boardings.end (); ints_itr++, p++) {
				trips [p] += ints_itr->at (stop);
			}
		}

		//---- write the stop ----

		file << i;

		if (Notes_Name_Flag ()) {
			file << "\t" << stop_equiv.Group_Label (i);
		}
		total = 0;

		for (int_itr = trips.begin (); int_itr != trips.end (); int_itr++) {
			file << "\t" << *int_itr;
			total += *int_itr;
		}
		if (num_periods > 1) {
			file << "\t" << total;
		}
		file << endl;
	}
	boarding_file.Close ();
	End_Progress ();
}
