//*********************************************************
//	Get_Parking_Data.cpp - read the parking file
//*********************************************************

#include "Relocate.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Get_Parking_Data
//---------------------------------------------------------

bool Relocate::Get_Parking_Data (Parking_File &file, Parking_Data &parking_rec)
{
	int index;
	Link_Data *link_ptr;

	if (!Data_Service::Get_Parking_Data (file, parking_rec)) return (false);

	if (!file.Nested ()) {
		link_ptr = &link_array [parking_rec.Link ()];

		Link_Shape (link_ptr, parking_rec.Dir (), points, UnRound (parking_rec.Offset ()), 0.0, 0.0);

		index = (int) parking_array.size ();

		if (target_flag) {
			parking_rec.Type (2);

			Int_Map_Itr map_itr = parking_map.find (parking_rec.Parking ());

			if (map_itr != parking_map.end ()) {
				Parking_Data *parking_ptr = &parking_array [map_itr->second];

				if (parking_ptr->Link () != parking_rec.Link ()) {
					target_park_map.insert (Int_Map_Data (parking_rec.Parking (), index));
					parking_array.push_back (parking_rec);
					parking_pt.insert (Point_Map_Data (index, points [0]));
				} else {
					parking_ptr->Type (1);
				}
				return (false);
			}
		} else {
			parking_rec.Type (0);
		}
		parking_pt.insert (Point_Map_Data (index, points [0]));
	}
	return (true);
}
