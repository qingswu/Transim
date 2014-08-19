//*********************************************************
//	Walk_Access.cpp - walk access processing
//*********************************************************

#include "LocationData.hpp"

#include <math.h>

//---------------------------------------------------------
//	Walk_Access
//---------------------------------------------------------

void LocationData::Walk_Access (void)
{
	int distance, runs, stop, loc;
	Dtime time;

	Int_Itr int_itr;
	Access_Itr access_itr;
	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Loc_Walk_Data loc_walk_rec, *loc_walk_ptr;
	Loc_Walk_Itr loc_walk_itr;
	Stop_Data *stop_ptr;
	Location_Itr loc_itr;
	Link_Data *link_ptr;

	//---- initialize the arrays ----

	if (stop_runs.size () == 0) {
		stop_runs.assign (stop_array.size (), 0);
	}
	memset (&loc_walk_rec, '\0', sizeof (loc_walk_rec));
	loc_walk_array.assign (location_array.size (), loc_walk_rec);

	//---- calculate the number of runs serving each stop ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
			if (walk_time_flag) {
				for (runs=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {
					time = run_itr->Schedule ();
					if (walk_time.Period (time) >= 0) runs++;
				}
			} else {
				runs = (int) stop_itr->size ();
			}
			if (runs > 0) {
				stop_runs [stop_itr->Stop ()] += runs;
			}
		}
	}

	//---- find activity locations with access to transit ----

	for (access_itr = access_array.begin (); access_itr != access_array.end (); access_itr++) {
		if (access_itr->From_Type () == LOCATION_ID && access_itr->To_Type () == STOP_ID) {
			loc_walk_ptr = &loc_walk_array [access_itr->From_ID ()];
			runs = stop_runs [access_itr->To_ID ()];

			if (runs > 0) {
				distance = walk_distance - Resolve (access_itr->Time ());

				if (distance > 0) {
					loc_walk_ptr->count++;
					loc_walk_ptr->distance += distance;
					loc_walk_ptr->weight += distance * runs;
				}
			}
		}
	}

	//---- match stops and locations on the same link ----

	for (loc=0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, loc++) {
		loc_walk_ptr = &loc_walk_array [loc];
		link_ptr = &link_array [loc_itr->Link ()];

		//---- check for walk permissions ----

		if (Use_Permission (link_ptr->Use (), WALK)) {
			for (stop=0, int_itr = stop_runs.begin (); int_itr != stop_runs.end (); int_itr++, stop++) {
				runs = *int_itr;
				if (runs == 0) continue;

				stop_ptr = &stop_array [stop];
				if (stop_ptr->Link () == loc_itr->Link ()) {
					if (stop_ptr->Dir () == loc_itr->Dir ()) {
						distance = loc_itr->Offset ();
					} else {
						distance = (link_ptr->Length () - loc_itr->Offset ());
					}
					distance = abs (distance - stop_ptr->Offset ());
					distance = walk_distance - Resolve (distance);

					if (distance > 0) {
						loc_walk_ptr->count++;
						loc_walk_ptr->distance += distance;
						loc_walk_ptr->weight += distance * runs;
					}
				}
			}
		}
	}

	//---- normalize the location weights ----

	for (loc_walk_itr = loc_walk_array.begin (); loc_walk_itr != loc_walk_array.end (); loc_walk_itr++) {
		if (loc_walk_itr->count > 0) {
			distance = (loc_walk_itr->distance + loc_walk_itr->count / 2) / loc_walk_itr->count;

			loc_walk_itr->distance = walk_distance - distance;

			if (distance > 0) {
				loc_walk_itr->weight = (loc_walk_itr->weight + distance / 2) / distance;
			}
		}
	}
}
