//*********************************************************
//	Zone_Grids.cpp - Count the grids in each zone
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Zone_Grids
//---------------------------------------------------------

void GridData::Zone_Grids ()
{
	int zone;
	String name;

	Int_Itr fld_itr;
	Int_Map zone_count;
	Int_Map_Stat map_stat;
	Int_Map_Itr count_itr;

	Show_Message (String ("Scanning %s -- Record") % in_file.File_Type ());
	Set_Progress ();

	if (in_zone_fld.size () == 0) {
		Error ("Grid Zone Field Missing");
	}

	while (in_file.Read_Record ()) {
		Show_Progress ();

		for (fld_itr = in_zone_fld.begin (); fld_itr != in_zone_fld.end (); fld_itr++) {
			zone = in_file.Get_Integer (*fld_itr);
			if (zone > 0) {
				map_stat = zone_count.insert (Int_Map_Data (zone, 1));
				if (!map_stat.second) {
					map_stat.first->second++;
				}
			}
		}
	}
	End_Progress ();

	in_file.Rewind ();

	//---- save the zone counts ----

	for (count_itr = zone_count.begin (); count_itr != zone_count.end (); count_itr++) {
		zone_grids.Put_Field (0, count_itr->second);
		zone_grids.Write_Record (count_itr->first);
	}
	Print (2, "Number of Zones with Grids = ") << zone_count.size ();
}
