//*********************************************************
//	Get_Pocket_Data.cpp - read the pocket lane file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Pocket_Data
//---------------------------------------------------------

bool TransimsNet::Get_Pocket_Data (Pocket_File &file, Pocket_Data &pocket_rec)
{
	if ((delete_link_flag && delete_link_range.In_Range (file.Link ())) ||
		(update_link_flag && update_link_range.In_Range (file.Link ()))) {
		xpocket++;
		return (false);
	}
	if (update_node_flag && !repair_flag) {
		Int_Map_Itr itr = link_map.find (file.Link ());
		if (itr == link_map.end ()) {
			xpocket++;
			return (false);
		}
		Link_Data *link_ptr = &link_array [itr->second];
		int node, dir;
		if (file.Dir () == 0) {
			node = link_ptr->Bnode ();
			dir = link_ptr->AB_Dir ();
		} else {
			node = link_ptr->Anode ();
			dir = link_ptr->BA_Dir ();
		}
		Node_Data *node_ptr = &node_array [node];
		if (update_node_range.In_Range (node_ptr->Node ())) {
			if (!update_dir_range.In_Range (dir)) {
				update_dir_range.Add_Range (dir, dir);
				update_dir_flag = true;
			}
			xpocket++;
			return (false);
		}
	}
	return (Data_Service::Get_Pocket_Data (file, pocket_rec));
}
