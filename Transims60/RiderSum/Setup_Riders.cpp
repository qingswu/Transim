//*********************************************************
//	Setup_Riders.cpp - setup the transit ridership data
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Setup_Riders
//---------------------------------------------------------

void RiderSum::Setup_Riders (void)
{
	int i, ndir, nstop, nperiod, *list;

	Dir_Itr dir_itr;
	Dir_Data *dir_ptr;
	Stop_Itr stop_itr;
	Int_Map_Itr map_itr;
	Integers int_data;
	
	nperiod = sum_periods.Num_Periods ();
	if (nperiod < 1) nperiod = 1;

	ndir = (int) dir_array.size ();
	nstop = (int) stop_array.size ();

	//---- allocate ridership memory ----

	if (rider_flag) {
		load_data.assign (ndir, 0);
		enter_runs.assign (ndir, 0);
		enter_cap.assign (ndir, 0);
		stop_list.assign (nstop, 0);

		//---- initialize the link direction data ----

		for (dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++) {
			dir_itr->First_Pocket (-1);
		}

		//---- process each stop ----

		for (i=0, stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++, i++) {
			list = &stop_list [i];
			
			map_itr = dir_map.find (stop_itr->Link_Dir ());

			if (map_itr != dir_map.end ()) {
				dir_ptr = &dir_array [map_itr->second];

				*list = dir_ptr->First_Pocket ();
				dir_ptr->First_Pocket (i);
			}
		}
	}

	//---- allocate boarding and alighting memory ----

	if (rider_flag || on_off_flag) {
		int_data.assign (stop_array.size (), 0);

		int_data.assign (nstop, 0);
		board_data.assign (nperiod, int_data);
		alight_data.assign (nperiod, int_data);

		start_runs.assign (nstop, 0);
		end_runs.assign (nstop, 0);
		start_cap.assign (nstop, 0);
		end_cap.assign (nstop, 0);
	}
}
