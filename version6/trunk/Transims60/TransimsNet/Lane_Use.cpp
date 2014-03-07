//*********************************************************
//	Lane_Use.cpp - process lane use records
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Lane_Use
//---------------------------------------------------------

void TransimsNet::Lane_Use (void)
{
	int dir_index, num, index;

	Link_Dir_Data link_dir;
	Link_Use_Map_Itr link_use_itr;
	Link_Use_Array *link_use_ptr;
	Link_Use_Itr use_itr;
	Dir_Data *dir_ptr;
	Lane_Use_Data lane_use_rec;

	for (link_use_itr = link_use_map.begin (); link_use_itr != link_use_map.end (); link_use_itr++) {
		dir_index = link_use_itr->first;
		link_use_ptr = &link_use_itr->second;

		num = (int) link_use_ptr->size ();
		if (num == 0) continue;

		index = (int) lane_use_array.size ();
		lane_use_rec.Dir_Index (dir_index);
			
		dir_ptr = &dir_array [dir_index];
		dir_ptr->First_Lane_Use (index);

		for (use_itr = link_use_ptr->begin (); use_itr != link_use_ptr->end (); use_itr++) {
			lane_use_rec.Type (use_itr->Type ());
			lane_use_rec.Use (use_itr->Use ());

			lane_use_rec.Low_Lane (use_itr->Low_Lane ());
			lane_use_rec.High_Lane (use_itr->High_Lane ());

			lane_use_rec.Start (use_itr->Start ());
			lane_use_rec.End (use_itr->End ());

			lane_use_rec.Toll (Round (use_itr->Toll ()));
			lane_use_rec.Speed (use_itr->Speed ());

			if (--num > 0) {
				lane_use_rec.Next_Index (++index);
			} else {
				lane_use_rec.Next_Index (-1);
			}
			lane_use_array.push_back (lane_use_rec);
		}
	}
} 

