//*********************************************************
//	Line_Group.cpp - Report Transit Line Groups
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Line_Group_Report
//---------------------------------------------------------

void PlanSum::Line_Group_Report (void)
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

	Show_Message ("Transit Line Group Report -- Group");
	Set_Progress ();

	Header_Number (LINE_GROUP);

	if (!Break_Check ((int) line_equiv.Num_Groups () + 8)) {
		Print (1);
		Line_Group_Header ();
	}
	num = line_equiv.Max_Group ();

	for (i=1; i <= num; i++) {
		Show_Progress ();

		list = line_equiv.Group_List (i);
		if (list == NULL) continue;

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
		Print (1, String ("%5d %6d %5d  %7d %7d %7d %7d  %7d %7d %7d  %s") % 
			i % tot_runs % tot_stops % tot_tot_board % max_max_board % min_min_board % 
			avg_board % max_max_load % min_min_load % avg_load % line_equiv.Group_Label (i));
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Group_Header
//---------------------------------------------------------

void PlanSum::Line_Group_Header (void)
{
	Print (1, "Transit Line Group Summary");
	Print (2, "        Num.  Num.  -----------Boardings-----------  -----Max.Load Point----");
	Print (1, "Group   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average");
	Print (1);
}

/*********************************************|***********************************************

	Transit Line Group Summary

            Num.  Num.  -----------Boardings-----------  -----Max.Load Point----
	Group   Runs Stops    Total Maximum Minimum Average  Maximum Minimum Average

	ddddd dddddd ddddd  ddddddd ddddddd ddddddd ddddddd  ddddddd ddddddd ddddddd  sssssssssssssssssssssss

**********************************************|***********************************************/ 
