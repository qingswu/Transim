//*********************************************************
//	Stop_Sum_Report - write transit boardings and alightings
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Stop_Sum_Report
//---------------------------------------------------------

void RiderSum::Stop_Sum_Report (void)
{
	int i, num, stop, index, on, off, board, alight;

	Integers boards, alights;

	Int_Map_Itr map_itr;
	Stop_Data *stop_ptr;

	Show_Message ("Stop Summary Report -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (STOP_SUM);

	stop = (int) stop_map.size ();
	if (select_stops) stop = stop_range.Range_Count ();

	if (!Break_Check (stop + 5)) {
		Print (1);
		Stop_Sum_Header ();
	}
	num = sum_periods.Num_Periods ();

	boards.assign (num, 0);
	alights.assign (num, 0);

	//---- process each stop ----

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		Show_Progress ();

		stop = map_itr->first;

		if (select_stops && !stop_range.In_Range (stop)) continue;

		index = map_itr->second;
		stop_ptr = &stop_array [index];

		Print (1, String ("%8d") % stop);

		board = alight = 0;

		for (i=0; i < num; i++) {
			on = board_data [i] [index];
			off = alight_data [i] [index];

			Print (0, String ("  %7d %7d") % on % off);
			board += on;
			alight += off;

			boards [i] += on;
			alights [i] += off;
		}
		if (num > 1) {
			Print (0, String ("  %7d %7d") % board % alight);
		}
		if (Notes_Name_Flag ()) {
			if (!stop_ptr->Name ().empty ()) {
				Print (0, String ("  %s") % stop_ptr->Name ());
			}
			if (!stop_ptr->Notes ().empty ()) {
				Print (0, " -- ") << stop_ptr->Notes ();
			}
		}
	}
	Print (2, "   Total");

	board = alight = 0;

	for (i=0; i < num; i++) {
		on = boards [i];
		off = alights [i];

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
//	Stop_Sum_Header
//---------------------------------------------------------

void RiderSum::Stop_Sum_Header (void)
{
	int i, num;
	String label;

	num = sum_periods.Num_Periods ();

	Print (1, "Stop Summary Report");
	Print (2, "        ");

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
	Print (1, "    Stop");

	for (i=0; i < num; i++) {
		Print (0, "    Board  Alight");
	}
	Print (1);
}

/*********************************************|***********************************************

	Stop Summary Report

	          --dd:dd..dd:dd-  --dd:dd..dd:dd-  ---- Total ----
	    Stop    Board  Alight    Board  Alight    Board  Alight

	dddddddd  ddddddd ddddddd  ddddddd ddddddd  ddddddd ddddddd  ssssssssssssssssssssssss

	   Total  ddddddd ddddddd  ddddddd ddddddd 

**********************************************|***********************************************/ 
