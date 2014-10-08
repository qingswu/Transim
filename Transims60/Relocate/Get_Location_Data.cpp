//*********************************************************
//	Get_Location_Data.cpp - read the location file
//*********************************************************

#include "Relocate.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool Relocate::Get_Location_Data (Location_File &file, Location_Data &location_rec)
{
	if (!Data_Service::Get_Location_Data (file, location_rec)) return (false);

	if (target_flag) {
		location_rec.Zone (2);

		Int_Map_Itr map_itr = location_map.find (location_rec.Location ());

		if (map_itr != location_map.end ()) {
			Location_Data *location_ptr = &location_array [map_itr->second];

			if (location_ptr->Link () != location_rec.Link ()) {
				target_loc_map.insert (Int_Map_Data (location_rec.Location (), (int) location_array.size ()));
				location_array.push_back (location_rec);
			} else {
				location_ptr->Zone (1);
			}
			return (false);
		}
	} else {
		location_rec.Zone (0);
	}
	return (true);
}

