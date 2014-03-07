//*********************************************************
//	Line_Sum_Report - write the transit line groups
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Line_Sum_Report
//---------------------------------------------------------

void RiderSum::Line_Sum_Report (void)
{
	int riders, max_load, board, alight, run, runs;

	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;

	Show_Message ("Line Summary Report -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (LINE_SUM);

	if (!Break_Check ((int) line_map.size () + 5)) {
		Print (1);
		Line_Sum_Header ();
	}

	//---- get the route data ----

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		if (select_routes && !route_range.In_Range (map_itr->first)) continue;

		line_ptr = &line_array [map_itr->second];

		if (select_modes && !select_mode [line_ptr->Mode ()]) continue;

		//---- check the link criteria ----

		if (!Link_Selection (line_ptr)) continue;

		//---- set the run flags ----

		if (!Run_Selection (line_ptr)) continue;

		//---- save the route ridership data ----

		riders = board = alight = max_load = runs = 0;

		for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
			for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
				if (run_flag [run] == 0) continue;
				if (run_period [run] < 0) continue;
				if (select_time_of_day && !time_range.In_Range (run_itr->Schedule ())) continue;

				if (run_itr->Board () > board) board = run_itr->Board ();
				if (run_itr->Alight () > alight) alight = run_itr->Alight ();
				if (run_itr->Load () > max_load)  max_load = run_itr->Load ();
				riders += run_itr->Board ();

				if (run_flag [run] == 1) {
					runs++;
					run_flag [run] = 2;
				}
			}
		}
		if (riders == 0) continue;
		
		Print (1, String ("%8d  %10.10s  %5d  %5d  %8d  %8d %8d %8d") % line_ptr->Route () %
			Transit_Code ((Transit_Type) line_ptr->Mode ()) % line_ptr->size () % runs %
			riders % max_load % board % alight);

		if (!line_ptr->Name ().empty ()) {
			Print (0, String ("  %s") % line_ptr->Name ());
		}
		if (Notes_Name_Flag ()) {
			if (!line_ptr->Notes ().empty ()) {
				Print (0, String ("  %s") % line_ptr->Notes ());
			}
		}
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Sum_Header
//---------------------------------------------------------

void RiderSum::Line_Sum_Header (void)
{
	Print (1, "Line Summary Report");
	Print (2, "   Route        Mode  Stops   Runs    Riders  Max_Load   Max_On  Max_Off");
	Print (1);
}

/*********************************************|***********************************************

	Line Summary Report

	   Route        Mode  Stops   Runs    Riders  Max_Load   Max_On  Max_Off
	
	dddddddd  ssssssssss  ddddd  ddddd  dddddddd  dddddddd dddddddd dddddddd  sssssssssssssssssssss

**********************************************|***********************************************/ 
