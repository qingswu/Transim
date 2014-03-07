//*********************************************************
//	Write_Line_On_Off.cpp - write line on off file
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Write_Line_On_Off
//---------------------------------------------------------

void PlanSum::Write_Line_On_Off (void)
{
	int line, period, board, alight, first, last, trips;
	String name;

	Line_On_Off_Itr line_on_off_itr;
	On_Off_Map_Itr on_off_itr;
	First_Last_Map_Itr first_last_itr;
	Int_Map_Itr map_itr;
	Stop_Data *stop_ptr;
	Line_Data *line_ptr;

	Show_Message ("Writing Line On Off File -- Record");
	Set_Progress ();

	fstream &file = on_off_file.File ();

	//---- print the header ----

	file << "Line\tName\tPeriod\tBoard\tAlight\tFirst\tLast\tTrips\tNotes" << endl;

	for (line_on_off_itr = line_on_off.begin (); line_on_off_itr != line_on_off.end (); line_on_off_itr++) {

		line = line_on_off_itr->first.first;
		if (line == 0) continue;

		if (Line_Equiv_Flag ()) {
			name = line_equiv.Group_Label (line);
		} else {
			map_itr = line_map.find (line);
			if (map_itr != line_map.end ()) {
				line_ptr = &line_array [map_itr->second];
				name = line_ptr->Name ();
			} else {
				name = "";
			}
		}
		period = line_on_off_itr->first.second;
		if (period < 0) continue;

		for (on_off_itr = line_on_off_itr->second.begin (); on_off_itr != line_on_off_itr->second.end (); on_off_itr++) {

			board = on_off_itr->first.first;
			alight = on_off_itr->first.second;

			if (board == 0 || alight == 0) continue;

			for (first_last_itr = on_off_itr->second.begin (); first_last_itr != on_off_itr->second.end (); first_last_itr++) {
				Show_Progress ();

				first = first_last_itr->first.first;
				last = first_last_itr->first.second;
				trips = first_last_itr->second;

				file << line << "\t" << name << "\t" << sum_periods.Range_Label (period) << "\t" <<
					board << "\t" << alight << "\t" << first << "\t" << last << "\t" << trips;

				if (Notes_Name_Flag ()) {
					if (Stop_Equiv_Flag ()) {
						file << "\t" << stop_equiv.Group_Label (board);
						file << " to " << stop_equiv.Group_Label (alight);
						file << " from " << stop_equiv.Group_Label (first);
						file << " to " << stop_equiv.Group_Label (last);
					} else {
						file << "\t";
						map_itr = stop_map.find (board);
						if (map_itr != stop_map.end ()) {
							stop_ptr = &stop_array [map_itr->second];
							file << stop_ptr->Name ();
						} else {
							file << board;
						}
						file << " to ";
						map_itr = stop_map.find (alight);
						if (map_itr != stop_map.end ()) {
							stop_ptr = &stop_array [map_itr->second];
							file << stop_ptr->Name ();
						} else {
							file << alight;
						}
						file << " from ";
						map_itr = stop_map.find (first);
						if (map_itr != stop_map.end ()) {
							stop_ptr = &stop_array [map_itr->second];
							file << stop_ptr->Name ();
						} else {
							file << first;
						}
						file << " to ";
						map_itr = stop_map.find (last);
						if (map_itr != stop_map.end ()) {
							stop_ptr = &stop_array [map_itr->second];
							file << stop_ptr->Name ();
						} else {
							file << last;
						}
					}
				}
				file << endl;
			}
		}
	}
	End_Progress ();
	on_off_file.Close ();
}
