//*********************************************************
//	Get_Access_Data.cpp - read the access link file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Access_Data
//---------------------------------------------------------

bool Relocate::Get_Access_Data (Access_File &file, Access_Data &access_rec)
{
	if (!Data_Service::Get_Access_Data (file, access_rec)) return (false);

	if (target_flag) {
		access_rec.Cost (2);

		Int_Map_Itr map_itr = access_map.find (access_rec.Link ());

		if (map_itr != access_map.end ()) {
			Access_Data *access_ptr = &access_array [map_itr->second];

			if (access_ptr->From_Type () != access_rec.From_Type () || 
				access_ptr->From_ID () != access_rec.From_ID () ||
				access_ptr->To_Type () != access_rec.To_Type () ||
				access_ptr->To_ID () != access_rec.To_ID ()) {

				target_access_map.insert (Int_Map_Data (access_rec.Link (), (int) access_array.size ()));
				access_array.push_back (access_rec);
			} else {
				access_ptr->Cost (1);
			}
			return (false);
		}
	} else {
		access_rec.Cost (0);
	}
	return (true);
}
