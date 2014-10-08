//*********************************************************
//	Stop_Group.cpp - Report Transit Stop Groups
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Stop_Group_Report
//---------------------------------------------------------

void PlanSum::Stop_Group_Report (void)
{
	int i, stop, total_board, total_alight;
	int walk_board, bus_board, rail_board, drive_board;
	int walk_alight, bus_alight, rail_alight, drive_alight;
	int tot_walk_on, tot_bus_on, tot_rail_on, tot_drive_on;
	int tot_walk_off, tot_bus_off, tot_rail_off, tot_drive_off;

	Transfer_Data *transfer_ptr;
	Int_Set *list;
	Int_Set_Itr list_itr;
	Int_Map_Itr int_itr;

	Show_Message ("Transit Stop Group Report -- Group");
	Set_Progress ();

	Header_Number (STOP_GROUP);

	if (!Break_Check ((int) stop_equiv.Num_Groups () + 8)) {
		Print (1);
		Stop_Group_Header ();
	}

	tot_walk_on = tot_bus_on = tot_rail_on = tot_drive_on = 0;
	tot_walk_off = tot_bus_off = tot_rail_off = tot_drive_off = 0;

	for (i = stop_equiv.First_Group (); i > 0; i = stop_equiv.Next_Group ()) {
		Show_Progress ();

		list = stop_equiv.Group_List (i);
		if (list == NULL) continue;

		walk_board = bus_board = rail_board = drive_board = 0;
		walk_alight = bus_alight = rail_alight = drive_alight = 0;

		//---- process each stop in the stop group ----

		for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {
			int_itr = stop_map.find (*list_itr);
			if (int_itr == stop_map.end ()) continue;

			stop = int_itr->second;
			transfer_ptr = &transfer_array [stop];

			walk_board += transfer_ptr->Walk_Board ();
			bus_board += transfer_ptr->Bus_Board ();
			rail_board += transfer_ptr->Rail_Board ();
			drive_board += transfer_ptr->Drive_Board ();

			walk_alight += transfer_ptr->Walk_Alight ();
			bus_alight += transfer_ptr->Bus_Alight ();
			rail_alight += transfer_ptr->Rail_Alight ();
			drive_alight += transfer_ptr->Drive_Alight ();
		}
		total_board = walk_board + bus_board + rail_board + drive_board;
		total_alight = walk_alight + bus_alight + rail_alight + drive_alight;

		tot_walk_on += walk_board;
		tot_bus_on += bus_board;
		tot_rail_on += rail_board;
		tot_drive_on += drive_board;

		tot_walk_off += walk_alight;
		tot_bus_off += bus_alight;
		tot_rail_off += rail_alight;
		tot_drive_off += drive_alight;

		Print (1, String ("%5d%7d%7d%7d%7d %7d%7d%7d%7d%7d %7d") % 
			i % walk_board % bus_board % rail_board % drive_board % total_board %
			walk_alight % bus_alight % rail_alight % drive_alight % total_alight);

		if (Notes_Name_Flag ()) {		
			Print (0, String ("  %s") % stop_equiv.Group_Label (i));
		}
	}
	End_Progress ();

	total_board = tot_walk_on + tot_bus_on + tot_rail_on + tot_drive_on;
	total_alight = tot_walk_off + tot_bus_off + tot_rail_off + tot_drive_off;

	Print (2, String ("Total%7d%7d%7d%7d %7d%7d%7d%7d%7d %7d") %
		tot_walk_on % tot_bus_on % tot_rail_on % tot_drive_on % total_board %
		tot_walk_off % tot_bus_off % tot_rail_off % tot_drive_off % total_alight);

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Group_Header
//---------------------------------------------------------

void PlanSum::Stop_Group_Header (void)
{
	Print (1, "Transit Stop Group Summary");
	Print (2, "      --------- Boardings From ---------- -------- Alightings To ------------");
	Print (1, "Group   Walk    Bus   Rail  Drive   Total   Walk    Bus   Rail  Drive   Total");
	Print (1);
}

/*********************************************|***********************************************

	Transit Stop Group Summary

	      --------- Boardings From ---------- -------- Alightings To ------------
	Group   Walk    Bus   Rail  Drive   Total   Walk    Bus   Rail  Drive   Total 
	
	ddddd dddddd dddddd dddddd dddddd ddddddd dddddd dddddd dddddd dddddd ddddddd  ssssssssssssssss

	Total dddddd dddddd dddddd dddddd ddddddd dddddd dddddd dddddd dddddd ddddddd

**********************************************|***********************************************/ 
