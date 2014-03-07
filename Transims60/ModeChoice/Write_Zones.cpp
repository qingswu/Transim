//*********************************************************
//	Write_Zones.cpp - write the zone mode shares
//*********************************************************

#include "ModeChoice.hpp"

//---------------------------------------------------------
//	Write_Zones
//---------------------------------------------------------

void ModeChoice::Write_Zones (void)
{
	int fld;
	double value;
	Int_Map *map;
	Int_Map_Itr itr;

	Show_Message ("Writing Zone Data");
	Set_Progress ();

	//---- process each zone ----

	if (prod_flag) {
		map = trip_file->Org_Map ();
	
		for (itr = map->begin (); itr != map->end (); itr++) {
			Show_Progress ();

			prod_file.Put_Field (0, itr->first);

			for (fld=0; fld <= num_modes; ) {
				value = prod_share [fld] [itr->second];
				prod_file.Put_Field (++fld, value);
			}
			prod_file.Write ();
		}
		prod_file.Close ();
	}
	if (attr_flag) {
		map = trip_file->Des_Map ();

		for (itr = map->begin (); itr != map->end (); itr++) {
			Show_Progress ();

			attr_file.Put_Field (0, itr->first);

			for (fld=0; fld <= num_modes; ) {
				value = attr_share [fld] [itr->second];
				attr_file.Put_Field (++fld, value);
			}
			attr_file.Write ();
		}
		attr_file.Close ();
	}
	End_Progress ();
}
