//*********************************************************
//	Stop_Grp_Detail.cpp - Detail Transit Stop Groups
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Stop_Group_Detail
//---------------------------------------------------------

void PlanSum::Stop_Group_Detail (void)
{
	int i, stop, total_board, total_alight, tot_board, tot_alight;
	int walk_board, bus_board, rail_board, drive_board;
	int walk_alight, bus_alight, rail_alight, drive_alight;
	int tot_walk_on, tot_bus_on, tot_rail_on, tot_drive_on;
	int tot_walk_off, tot_bus_off, tot_rail_off, tot_drive_off;
    String label;

	Transfer_Data *transfer_ptr;
	Stop_Data *stop_ptr;
	Int_Set *list;
	Int_Set_Itr list_itr;
	Int_Map_Itr int_itr;

	Show_Message ("Transit Stop Group Details -- Group");
	Set_Progress ();

	Header_Number (STOP_GRP_DETAIL);

	if (!Break_Check ((int) stop_equiv.Num_Groups () * 7 + 8)) {
		Print (1);
		Stop_Group_Detail_Header ();
	}

	tot_walk_on = tot_bus_on = tot_rail_on = tot_drive_on = 0;
	tot_walk_off = tot_bus_off = tot_rail_off = tot_drive_off = 0;

	for (i = stop_equiv.First_Group (); i > 0; i = stop_equiv.Next_Group ()) {
		Show_Progress ();

		list = stop_equiv.Group_List (i);
		if (list == NULL) continue;

		walk_board = bus_board = rail_board = drive_board = 0;
		walk_alight = bus_alight = rail_alight = drive_alight = 0;

		label ("%d %s") % i % stop_equiv.Group_Label (i);
		Print (1, label);

		//---- process each stop in the stop group ----

		for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {
			int_itr = stop_map.find (*list_itr);
			if (int_itr == stop_map.end ()) continue;

			stop = int_itr->second;
			transfer_ptr = &transfer_array [stop];

			tot_board = transfer_ptr->Walk_Board () + transfer_ptr->Bus_Board () + 
						transfer_ptr->Rail_Board () + transfer_ptr->Drive_Board ();

			tot_alight = transfer_ptr->Walk_Alight () + transfer_ptr->Bus_Alight () + 
						transfer_ptr->Rail_Alight () + transfer_ptr->Drive_Alight ();

			walk_board += transfer_ptr->Walk_Board ();
			bus_board += transfer_ptr->Bus_Board ();
			rail_board += transfer_ptr->Rail_Board ();
			drive_board += transfer_ptr->Drive_Board ();

			walk_alight += transfer_ptr->Walk_Alight ();
			bus_alight += transfer_ptr->Bus_Alight ();
			rail_alight += transfer_ptr->Rail_Alight ();
			drive_alight += transfer_ptr->Drive_Alight ();

			Print (1, String ("%10d%7d%7d%7d%7d %7d%7d%7d%7d%7d %7d") % 
				*list_itr % transfer_ptr->Walk_Board () % transfer_ptr->Bus_Board () % 
				transfer_ptr->Rail_Board () % transfer_ptr->Drive_Board () % tot_board %
				transfer_ptr->Walk_Alight () % transfer_ptr->Bus_Alight () % 
				transfer_ptr->Rail_Alight () % transfer_ptr->Drive_Alight () % tot_alight);

			if (Notes_Name_Flag ()) {
				stop_ptr = &stop_array [stop];
				Print (0, "  ") << stop_ptr->Name ();
			}
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

		Print (1, String ("%10.10s%7d%7d%7d%7d %7d%7d%7d%7d%7d %7d") % 
			"Total" % walk_board % bus_board % rail_board % drive_board % total_board %
			walk_alight % bus_alight % rail_alight % drive_alight % total_alight);
		Print (1);
	}
	End_Progress ();

	total_board = tot_walk_on + tot_bus_on + tot_rail_on + tot_drive_on;
	total_alight = tot_walk_off + tot_bus_off + tot_rail_off + tot_drive_off;

	Print (1, String ("%-10.10s%7d%7d%7d%7d %7d%7d%7d%7d%7d %7d") %
		"Total" % tot_walk_on % tot_bus_on % tot_rail_on % tot_drive_on % total_board %
		tot_walk_off % tot_bus_off % tot_rail_off % tot_drive_off % total_alight);

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Group_Detail_Header
//---------------------------------------------------------

void PlanSum::Stop_Group_Detail_Header (void)
{
	Print (1, "Transit Stop Group Details");
	Print (2, String ("%11c--------- Boardings From ---------- -------- Alightings To ------------") % BLANK);
	Print (1, "Group Stop   Walk    Bus   Rail  Drive   Total   Walk    Bus   Rail  Drive   Total");
	Print (1);
}

/*********************************************|***********************************************

	Transit Stop Group Details

	           --------- Boardings From ---------- -------- Alightings To ------------
	Group Stop   Walk    Bus   Rail  Drive   Total   Walk    Bus   Rail  Drive   Total 
	
	ssssssssssssssssssssssssssssssssssssss 
	dddddddddd dddddd dddddd dddddd dddddd ddddddd dddddd dddddd dddddd dddddd ddddddd sssssssssssssssssssss
	     Total dddddd dddddd dddddd dddddd ddddddd dddddd dddddd dddddd dddddd ddddddd

	Total      dddddd dddddd dddddd dddddd ddddddd dddddd dddddd dddddd dddddd ddddddd

**********************************************|***********************************************/ 
