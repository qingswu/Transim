//*********************************************************
//	Zone_Groups.cpp - allocate zone and group data
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Zone_Groups
//---------------------------------------------------------

void TourData::Zone_Groups (void)
{
	//---- get the number of zones and groups ----

	while (zone_file->Read ()) {
		int zone = zone_file->Zone ();
		int group = (group_flag) ? zone_file->Get_Integer (group_field) : zone;

		if (zone > max_zone) max_zone = zone;
		if (group > max_group) max_group = group;

		group_map.insert (Int_Map_Data (group, (int) group_map.size ()));
	}
	zone_file->Rewind ();

	//---- initialize arrays ----

	num_group = (int) group_map.size ();

	zone_group.assign (max_zone+1, 0);

	Doubles dbls;
	dbls.assign (max_zone+1, 0);
	od_distance.assign (max_zone+1, dbls);

	Integers ints;
	group_zones.assign (max_group+1, ints);

	Table_Itr table_itr;

	dbls.assign (num_group, 0);

	for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
		table_itr->zone_wt.assign (max_zone+1, 0);
		table_itr->zone_pts.assign (max_zone+1, ints);
		table_itr->tours.assign (num_group, dbls);

		if (attr_file_flag) {
			table_itr->attr.assign (max_zone+1, 0);
		}
	}
	
	Write (1, "Highest Zone Number = ") << max_zone;
	Write (1, "Highest Group Number = ") << max_group;
	Write (1, "Number of Groups = ") << num_group;
	Write (1);
}
