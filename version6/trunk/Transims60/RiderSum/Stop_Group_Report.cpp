//*********************************************************
//	Stop_Group_Report - stop group boardings and alightings
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Stop_Group_Report
//---------------------------------------------------------

void RiderSum::Stop_Group_Report (void)
{
	int n, groups, i, num, stop, index, on, off, board, alight;
	String label;

	Integers boards, alights, total_on, total_off;
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;
	Stop_Data *stop_ptr;

	Show_Message ("Stop Group Report -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (STOP_GROUP);

	groups = stop_equiv.Num_Groups ();

	if (!Break_Check (groups + 7)) {
		Print (1);
		Stop_Group_Header ();
	}
	groups = stop_equiv.Max_Group ();
	num = sum_periods.Num_Periods ();

	total_on.assign (num, 0);
	total_off.assign (num, 0);

	for (n=1; n <= groups; n++) {

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
		Print (1, String ("%5d") % n);

		board = alight = 0;

		for (i=0; i < num; i++) {
			on = boards [i];
			off = alights [i];

			Print (0, String ("  %7d %7d") % on % off);
			board += on;
			alight += off;

			total_on [i] += on;
			total_off [i] += off;
		}
		if (num > 1) {
			Print (0, String ("  %7d %7d") % board % alight);
		}
		Print (0, String ("  %s") % label);
	}
	Print (2, "Total");

	board = alight = 0;

	for (i=0; i < num; i++) {
		on = total_on [i];
		off = total_off [i];

		Print (0, String ("  %7d %7d") % on % off);
		board += on;
		alight += off;
	}
	if (num > 1) {
		Print (0, String ("  %7d %7d") % board % alight);
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Group_Header
//---------------------------------------------------------

void RiderSum::Stop_Group_Header (void)
{
	int i, num;
	String label;

	num = sum_periods.Num_Periods ();

	Print (1, "Stop Group Report");
	Print (2, "     ");

	if (num > 1) {
		for (i=0; i < num; i++) {
			label = sum_periods.Range_Format (i);
			if (label.length () >= 12) {
				Print (0, String ("  --%12.12s-") % label);
			} else if (label.length () == 11) {
				Print (0, String ("  ---%11.11s-") % label);
			} else {
				Print (0, String ("  ---%10.10s--") % label);
			}
		}
	}
	Print (0, "  ---- Total ----");

	if (num > 1) num++;
	Print (1, "Group");

	for (i=0; i < num; i++) {
		Print (0, "    Board  Alight");
	}
	Print (1);
}

/*********************************************|***********************************************

	Stop Group Report

	       --dd:dd..dd:dd-  --dd:dd..dd:dd-  ---- Total ----
	Group    Board  Alight    Board  Alight    Board  Alight

	ddddd  ddddddd ddddddd  ddddddd ddddddd  ddddddd ddddddd  ssssssssssssssssssssssss

	Total  ddddddd ddddddd  ddddddd ddddddd  ddddddd ddddddd

**********************************************|***********************************************/ 
