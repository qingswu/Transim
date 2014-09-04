//*********************************************************
//	Add_Zone_Data.cpp - add new zone records
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Add_Zone_Data
//---------------------------------------------------------

void TransimsNet::Add_Zone_Data (void)
{
	int zone, count;
	Db_Sort_Itr db_itr;
	Zone_Data zone_rec;
	Int_Map_Stat map_stat;

	//---- read the data file ----

	Show_Message (String ("Adding %s -- Record") % zone_data_file.File_Type ());
	Set_Progress ();

	count = 0;

	for (db_itr = zone_data_array.begin (); db_itr != zone_data_array.end (); db_itr++) {
		Show_Progress ();

		zone = db_itr->first;
		if (zone_map.find (zone) != zone_map.end ()) continue;

		update_zone_range.Add_Range (zone, zone);

		zone_data_array.Read_Record (zone);

		count++;
		zone_data_file.Copy_Fields (zone_data_array);

		zone_rec.Clear ();
		zone_rec.Zone (zone_data_file.Zone ());
		zone_rec.X (zone_data_file.X ());
		zone_rec.Y (zone_data_file.Y ());
		zone_rec.Z (zone_data_file.Z ());
		zone_rec.Area_Type (zone_data_file.Area_Type ());
		zone_rec.Notes (zone_data_file.Notes ());

		map_stat = zone_map.insert (Int_Map_Data (zone_rec.Zone (), (int) zone_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Zone Number = ") << zone_rec.Zone ();
			continue;
		} else {
			zone_array.push_back (zone_rec);
			if (zone_rec.Zone () > Max_Zone_Number ()) {
				Max_Zone_Number (zone_rec.Zone ());
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records Added = %d") % zone_data_file.File_Type () % count);
}
