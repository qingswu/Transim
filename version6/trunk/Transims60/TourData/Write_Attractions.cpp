//*********************************************************
//	Write_Attractions.cpp - Write the New Attractions File
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Write_Attractions
//---------------------------------------------------------

void TourData::Write_Attractions ()
{
	int i, zone, attr;
	Int_Map_Itr map_itr;
	Table_Itr table_itr;

	Show_Message (String ("Writing %s -- Record") % attr_file.File_Type ());
	Set_Progress ();

	for (map_itr = zone_map.begin (); map_itr != zone_map.end (); map_itr++) {
		zone = map_itr->first;
		Show_Progress ();

		attr_file.Put_Field (0, zone);

		for (i=1, table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
			attr = table_itr->zone_wt [zone];
			attr_file.Put_Field (i++, attr);

			attr = table_itr->attr [zone];
			attr_file.Put_Field (i++, attr);
		}
		if (!attr_file.Write ()) {
			Error (String ("Writing %s Zone %d") % attr_file.File_Type () % zone);
		}
	}
	End_Progress ();

	attr_file.Close ();
}

