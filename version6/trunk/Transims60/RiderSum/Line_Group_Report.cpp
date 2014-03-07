//*********************************************************
//	Line_Group_Report - write the transit line demand
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Line_Group_Report
//---------------------------------------------------------

void RiderSum::Line_Group_Report (void)
{
	int n, riders, max_load, board, alight, run, runs, groups, mode, stop, stops, ndir, nstop, index, load;
	String label;
	
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Driver_Itr driver_itr;
	Integers loads, boards, alights, nruns;
	Int_Itr int_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Stop_Data *stop_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	Show_Message ("Line Group Report -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (LINE_GROUP);
	
	groups = line_equiv.Num_Groups ();

	if (!Break_Check (groups + 7)) {
		Print (1);
		Line_Group_Header ();
	}
	groups = line_equiv.Max_Group ();
	mode = stops = 0;
	
	ndir = (int) dir_array.size ();
	nstop = (int) stop_array.size ();

	for (n=1; n <= groups; n++) {

		group = line_equiv.Group_List (n);
		if (group == 0) continue;

		label = line_equiv.Group_Label (n);

		loads.assign (ndir, 0);
		nruns.assign (nstop, 0);
		boards.assign (nstop, 0);
		alights.assign (nstop, 0);

		//---- process each line in the line group ----

		for (itr = group->begin (); itr != group->end (); itr++) {

			map_itr = line_map.find (*itr);
			if (map_itr == line_map.end ()) continue;

			Show_Progress ();

			line_ptr = &line_array [map_itr->second];
			mode = line_ptr->Mode ();
			stops = (int) line_ptr->size ();

			//---- set the run flags ----

			if (!Run_Selection (line_ptr)) continue;

			//---- save the route ridership by link ----

			stop = 1;
			stop_itr = line_ptr->begin();
			if (stop_itr == line_ptr->end ()) continue;

			index = stop_itr->Stop ();

			stop_ptr = &stop_array [index];

			load = 0;

			for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {
				dir_ptr = &dir_array [*driver_itr];

				link_ptr = &link_array [dir_ptr->Link ()];

				loads [*driver_itr] += load;

				while (stop_ptr->Link_Dir () == dir_ptr->Link_Dir ()) {

					load = 0;

					for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
						if (run_flag [run] == 0) continue;
						if (select_time_of_day && !time_range.In_Range (run_itr->Schedule ())) continue;
						if (sum_periods.Period (run_itr->Schedule ()) < 0) continue;

						load += run_itr->Load ();

						boards [index] += run_itr->Board ();
						alights [index] += run_itr->Alight ();
						nruns [index]++;
					}
					stop++;
					if (++stop_itr == line_ptr->end ()) break;

					index = stop_itr->Stop ();

					stop_ptr = &stop_array [index];
				}
			}
		}

		//---- save the route ridership data ----

		riders = board = alight = max_load = runs = stops = 0;

		for (int_itr = boards.begin (); int_itr != boards.end (); int_itr++) {
			if (*int_itr > board) board = *int_itr;
			riders += *int_itr;
		}
		for (int_itr = alights.begin (); int_itr != alights.end (); int_itr++) {
			if (*int_itr > alight) alight = *int_itr;
		}
		for (int_itr = nruns.begin (); int_itr != nruns.end (); int_itr++) {
			if (*int_itr > runs) runs = *int_itr;
		}
		for (int_itr = loads.begin (); int_itr != loads.end (); int_itr++) {
			if (*int_itr > max_load) max_load = *int_itr;
		}
		for (stop = 0; stop < nstop; stop++) {
			if (boards [stop] > 0 || alights [stop] > 0) stops++;
		}
		
		Print (1, String ("%5d  %10.10s  %5d  %5d  %8d  %8d %8d %8d  %s") % n %
			Transit_Code ((Transit_Type) mode) % stops % runs %
			riders % max_load % board % alight % label);
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Group_Header
//---------------------------------------------------------

void RiderSum::Line_Group_Header (void)
{
	Print (1, "Line Group Report");
	Print (2, "Group        Mode  Stops   Runs    Riders  Max_Load   Max_On  Max_Off");
	Print (1);
}

/*********************************************|***********************************************

	Line Summary Report

	Group        Mode  Stops   Runs    Riders  Max_Load   Max_On  Max_Off

	ddddd  ssssssssss  ddddd  ddddd  dddddddd  dddddddd dddddddd dddddddd  sssssssssssssssssssss

**********************************************|***********************************************/ 
