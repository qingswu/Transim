//*********************************************************
//	Line_Grp_Detail.cpp - Detail Transit Line Groups
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Line_Group_Detail
//---------------------------------------------------------

void PlanSum::Line_Group_Detail (void)
{
	int i, num, line;
	int nrun, nstop, run, board, alight, load;
	int sum_board, tot_board, max_board, min_board, avg_board;
	int high_load, tot_load, max_load, min_load, avg_load;
	int tot_routes, tot_runs, tot_stops;
	int tot_tot_board, max_max_board, min_min_board;
	int tot_tot_load, max_max_load, min_min_load;

	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run *run_ptr;

	Int_Set *list;
	Int_Set_Itr list_itr;
	Int_Map_Itr int_itr;

	Show_Message ("Transit Line Group Details -- Group");
	Set_Progress ();

	Header_Number (LINE_GRP_DETAIL);

	if (!Break_Check ((int) line_equiv.Num_Groups () * 7 + 8)) {
		Print (1);
		Line_Group_Detail_Header ();
	}
	num = line_equiv.Max_Group ();

	for (i=1; i <= num; i++) {
		Show_Progress ();

		list = line_equiv.Group_List (i);
		if (list == NULL) continue;

		Print (1, String ("%5d  %s") % i % line_equiv.Group_Label (i));

		tot_routes = tot_runs = tot_stops = avg_board = avg_load = 0;
		tot_tot_board = max_max_board = tot_tot_load = max_max_load = 0;
		min_min_load = min_min_board = -1;

		//---- process each line in the line group ----

		for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {
			int_itr = line_map.find (*list_itr);
			if (int_itr == line_map.end ()) continue;
			Show_Progress ();

			line = int_itr->second;
			line_ptr = &line_array [line];

			tot_board = max_board = min_board = 0;
			tot_load = max_load = min_load = 0;
		
			nstop = (int) line_ptr->size ();
			stop_itr = line_ptr->begin ();
			nrun = (int) stop_itr->size ();

			for (run=0; run < nrun; run++) {
				load = sum_board = high_load = 0;

				for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
					run_ptr = &stop_itr->at (run);

					board = run_ptr->Board ();
					alight = run_ptr->Alight ();

					load += board - alight;

					if (load == 0 && board == 0 && alight == 0) continue;

					if (load > high_load) high_load = load;
					sum_board += board;
				}
				if (high_load > max_load) max_load = high_load;
				if (high_load < min_load || min_load == 0) min_load = high_load;
				tot_load += high_load;

				if (sum_board > max_board) max_board = sum_board;
				if (sum_board < min_board || min_board == 0) min_board = sum_board;
				tot_board += sum_board;
			}

			//---- print report ----

			if (nrun > 0) {
				avg_board = (tot_board + nrun / 2) / nrun;
				avg_load = (tot_load + nrun / 2) / nrun;
			}
			Print (1, String ("    %8d %6d %5d  %7d %7d %7d %7d  %7d %7d %7d") %
				line_ptr->Route () % nrun % nstop % tot_board % max_board % min_board % 
				avg_board %	max_load % min_load % avg_load);

			if (Notes_Name_Flag ()) {
				if (!line_ptr->Name ().empty ()) {
					Print (0, String ("  %s") % line_ptr->Name ());
				}
				if (!line_ptr->Notes ().empty ()) {
					Print (0, String ("  %s") % line_ptr->Notes ());
				}
			}
			tot_routes++;
			tot_runs += nrun;
			tot_stops += nstop;
			tot_tot_board += tot_board;
			tot_tot_load += tot_load;

			if (max_load > max_max_load) max_max_load = max_load;
			if (min_load < min_min_load || min_min_load < 0) min_min_load = min_load;
			if (max_board > max_max_board) max_max_board = max_board;
			if (min_board < min_min_board || min_min_board < 0) min_min_board = min_board;
		}

		//---- system total ----

		if (tot_runs > 0) {
			avg_board = (tot_tot_board + tot_runs / 2) / tot_runs;
			avg_load = (tot_tot_load + tot_runs / 2) / tot_runs;
		}
		Print (1, String ("Total  %5d %6d %5d  %7d %7d %7d %7d  %7d %7d %7d") % 
			tot_routes % tot_runs % tot_stops % 
			tot_tot_board % max_max_board % min_min_board % avg_board %
			max_max_load % min_min_load % avg_load);
		Print (1);
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Group_Detail_Header
//---------------------------------------------------------

void PlanSum::Line_Group_Detail_Header (void)
{
	Print (1, "Transit Line Group Details");
	Print (2, "               Num.  Num.  -----------Boardings-----------  -----Max.Load Point----");
	Print (1, "Group   Line   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average");
	Print (1);
}

/*********************************************|***********************************************

	Transit Line Group Details

                   Num.  Num.  -----------Boardings-----------  -----Max.Load Point----
	Group   Line   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average

	ddddd  ssssssssssssssssssssssssss
	    dddddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd  sssssssssssssssssssssss
	    dddddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd  sssssssssssssssssssssss

    Total  ddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd

**********************************************|***********************************************/ 
