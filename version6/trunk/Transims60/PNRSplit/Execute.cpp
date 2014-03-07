//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PNRSplit.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PNRSplit::Execute (void)
{
	int i, z, node, zone, size;
	String name, record;

	Int_Map_Stat map_stat;

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
	}
	num_pnr = (int) parking_zone_map.size ();
	Write (2, "Number of Park-n-Ride Nodes = ") << num_pnr;
	Show_Message (1);

	parking_zone_file.Close ();

	//---- allocate matrix memory ----

	size = (int) (num_zones * sizeof (float));
	
	walk_data = new float * [num_zones];

	for (z=0; z < num_zones; z++) {
		walk_data [z] = new float [num_zones];
		memset (walk_data [z], '\0', size);
	}

	drive_data = new float * [num_zones];

	for (z=0; z < num_zones; z++) {
		drive_data [z] = new float [num_zones];
		memset (drive_data [z], '\0', size);
	}
	pnr_data = new float [num_zones];
	node_data = new long [num_zones];
	row_id = new long [num_zones];
	col_id = new long [num_zones];

	Process ();

	Exit_Stat (DONE);
}
