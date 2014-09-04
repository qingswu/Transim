//*********************************************************
//	Add_Link_Data.cpp - add new link records
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Add_Link_Data
//---------------------------------------------------------

void TransimsNet::Add_Link_Data (void)
{
	int link, count, index;
	Db_Sort_Itr db_itr;

	Int_Map_Stat map_stat;
	Int2_Map_Stat ab_stat;
	Link_Data link_rec;
	Dir_Data ab_rec, ba_rec;

	//---- read the data file ----

	Show_Message (String ("Adding %s -- Record") % link_data_file.File_Type ());
	Set_Progress ();

	count = 0;

	for (db_itr = link_data_array.begin (); db_itr != link_data_array.end (); db_itr++) {
		Show_Progress ();

		link = db_itr->first;
		if (link_map.find (link) != link_map.end ()) continue;
		
		update_link_range.Add_Range (link, link);

		link_data_array.Read_Record (link);

		count++;

		link_rec.Clear ();
		ab_rec.Clear ();
		ba_rec.Clear ();

		link_data_file.Copy_Fields (link_data_array);

		if (Data_Service::Get_Link_Data (link_data_file, link_rec, ab_rec, ba_rec)) {
			map_stat = link_map.insert (Int_Map_Data (link_rec.Link (), (int) link_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Link Number = ") << link_rec.Link ();
				continue;
			} else {
				link_array.push_back (link_rec);
			}

			//---- insert the AB direction ----

			if (link_rec.AB_Dir () >= 0) {
				index = (int) dir_array.size ();

				map_stat = dir_map.insert (Int_Map_Data (ab_rec.Link_Dir (), index));

				if (!map_stat.second) {
					Warning ("Duplicate Link Direction Number = ") << ab_rec.Link_Dir ();
					continue;
				} else {
					dir_array.push_back (ab_rec);
				}
				if (index != link_rec.AB_Dir ()) continue;

			}
			if (link_rec.BA_Dir () >= 0) {
				index = (int) dir_array.size ();

				map_stat = dir_map.insert (Int_Map_Data (ba_rec.Link_Dir (), index));

				if (!map_stat.second) {
					Warning ("Duplicate Link Direction Number = ") << ba_rec.Link_Dir ();
					continue;
				} else {
					dir_array.push_back (ba_rec);
				}
				if (index != link_rec.BA_Dir ()) continue;
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records Added = %d") % link_data_file.File_Type () % count);
}
