//*********************************************************
//	Get_Link_Data.cpp - read the link file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Link_Data
//---------------------------------------------------------

bool Relocate::Get_Link_Data (Link_File &file, Link_Data &link_rec, Dir_Data &ab_rec, Dir_Data &ba_rec)
{
	if (!Data_Service::Get_Link_Data (file, link_rec, ab_rec, ba_rec)) return (false);

	if (target_flag) {
		link_rec.Divided (2);
		ab_rec.Sign (2);
		ba_rec.Sign (2);

		Int_Map_Itr map_itr = link_map.find (link_rec.Link ());

		if (map_itr != link_map.end ()) {
			Link_Data *link_ptr = &link_array [map_itr->second];

			if (link_ptr->Anode () == link_rec.Anode () && link_ptr->Bnode () == link_rec.Bnode ()) {
				link_ptr->Divided (1);
				if (shape_flag) {
					Int_Map_Itr map_itr = target_shape_map.find (link_rec.Link ());
					if (map_itr != target_shape_map.end ()) {
						link_ptr->Shape (map_itr->second);
					}
				}
				return (false);
			}
			if (link_rec.AB_Dir () >= 0) {
				link_rec.AB_Dir ((int) dir_array.size ());

				target_dir_map.insert (Int_Map_Data (ab_rec.Link_Dir (), link_rec.AB_Dir ()));
				dir_array.push_back (ab_rec);
			}
			if (link_rec.BA_Dir () >= 0) {
				link_rec.BA_Dir ((int) dir_array.size ());

				target_dir_map.insert (Int_Map_Data (ba_rec.Link_Dir (), link_rec.BA_Dir ()));
				dir_array.push_back (ba_rec);
			}
			target_link_map.insert (Int_Map_Data (link_rec.Link (), (int) link_array.size ()));
			link_array.push_back (link_rec);
			return (false);
		}
	} else {
		link_rec.Divided (0);
		ab_rec.Sign (0);
		ba_rec.Sign (0);
	}
	return (true);
}

