//*********************************************************
//	Write_Stop_Group - stop group boardings and alightings
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Write_Stop_Group
//---------------------------------------------------------

void RiderSum::Write_Stop_Group (void)
{
	int n, i, num, stop, index, on, off, board, alight;
	String label;

	Integers boards, alights, total_on, total_off;
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;
	Stop_Data *stop_ptr;

	fstream &file = stop_group_file.File ();
	
	num = sum_periods.Num_Periods ();
	if (num == 0) num = 1;

	Show_Message (String ("Writing % -- Record") % stop_group_file.File_Type ());
	Set_Progress ();

	file << "\t";

	if (num > 1) {
		for (i=0; i < num; i++) {
			label = sum_periods.Range_Format (i);
			file << "\t" << label << "\t";
		}
	}
	file << "\tTotal\n";

	n = num;
	if (num > 1) n++;
	file << "Group\tName";

	for (i=0; i < n; i++) {
		file << "\tBoard\tAlight";
	}
	file << "\n";

	total_on.assign (num, 0);
	total_off.assign (num, 0);

	for (n = stop_equiv.First_Group (); n > 0; n = stop_equiv.Next_Group ()) {

		group = stop_equiv.Group_List (n);
		if (group == 0) continue;

		label = stop_equiv.Group_Label (n);

		boards.assign (num, 0);
		alights.assign (num, 0);

		//---- process each stop in the stop group ----

		for (itr = group->begin (); itr != group->end (); itr++) {

			map_itr = stop_map.find (*itr);
			if (map_itr == stop_map.end ()) continue;

			Show_Progress ();

			stop = map_itr->first;
			index = map_itr->second;
			stop_ptr = &stop_array [index];

			for (i=0; i < num; i++) {
				boards [i] += board_data [i] [index];
				alights [i] += alight_data [i] [index];
			}
		}
		file << n << "\t" << label;

		board = alight = 0;

		for (i=0; i < num; i++) {
			on = boards [i];
			off = alights [i];

			file << "\t" << on << "\t" << off;
			board += on;
			alight += off;

			total_on [i] += on;
			total_off [i] += off;
		}
		if (num > 1) {
			file << "\t" << board << "\t" << alight;
		}
		file << "\n";
	}
	file << "Total" << "\t";

	board = alight = 0;

	for (i=0; i < num; i++) {
		on = total_on [i];
		off = total_off [i];

		file << "\t" << on << "\t" << off;
		board += on;
		alight += off;
	}
	if (num > 1) {
		file << "\t" << board << "\t" << alight;
	}
	file << "\n";
	End_Progress ();
}
