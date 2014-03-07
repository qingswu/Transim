//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TourSkim.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TourSkim::Execute (void)
{
	int i, r, t, z, p, tabs, node, zone, num_threads;

	String name, record;
	Str_Itr str_itr;

	Int_Map_Stat map_stat;
	Int_Itr int_itr;

	//---- build the park-n-ride zone map ----

	i = 0;

	while (parking_zone_file.Read ()) {
		record = parking_zone_file.Record_String ();

		record.Split (name);
		node = name.Integer ();

		record.Split (name);
		zone = name.Integer ();

		map_stat = parking_zone_map.insert (Int_Map_Data (node, zone));
		if (!map_stat.second) {
			Warning (String ("Duplicate Parking Zone Map (%d = %d vs %d)") % node % zone % map_stat.first->second);
		}
		map_stat = parking_id_map.insert (Int_Map_Data (node, i++));
	}
	num_pnr = (int) parking_zone_map.size ();
	Write (2, "Number of Park-n-Ride Nodes = ") << num_pnr;
	Show_Message (1);

	parking_zone_file.Close ();

	//---- allocate matrix memory ----
	
	walk_data = new float *** [num_return];
	num_tabs = (int) walk_table.size ();

	for (r=0; r < num_return; r++) {
		walk_data [r] = new float ** [num_tabs];

		for (t=0; t < num_tabs; t++) {
			walk_data [r] [t] = new float * [num_zones];

			for (z=0; z < num_zones; z++) {
				walk_data [r] [t] [z] = new float [num_zones];
			}
		}
	}
	drive_data = new float ** [num_return];

	for (r=0; r < num_return; r++) {
		drive_data [r] = new float * [num_pnr];

		for (p=0; p < num_pnr; p++) {
			drive_data [r] [p] = new float [num_zones];
		}
	}
	z = 0;

	//---- set the read queue ----

	tabs = (int) return_array.size ();

	for (i=0; i < tabs; i++) {
		read_queue.Put (i);
	}
	read_queue.End_of_Queue ();

	//---- read processing threads ---

	num_threads = Num_Threads ();
	Num_Threads (MIN (num_threads, tabs));

	if (Num_Threads () > 1) {
#ifdef THREADS
		Threads threads;

		for (i=0; i < Num_Threads (); i++) {
			threads.push_back (thread (Read_Process (this)));
		}
		threads.Join_All ();
#endif
	} else {
		Read_Process read_process (this);
		read_process ();
	}

	//---- set the processing queue ----

	tabs = (int) file_group.size ();

	for (i=0; i < tabs; i++) {
		group_queue.Put (i);
	}
	group_queue.End_of_Queue ();

	//---- processing threads ---
	
	Num_Threads (MIN (num_threads, tabs));

	if (Num_Threads () > 1) {
#ifdef THREADS
		Threads threads;

		for (i=0; i < Num_Threads (); i++) {
			threads.push_back (thread (Group_Process (this)));
		}
		threads.Join_All ();
#endif
	} else {
		Group_Process group_process (this);
		group_process ();
	}
	Exit_Stat (DONE);
}
