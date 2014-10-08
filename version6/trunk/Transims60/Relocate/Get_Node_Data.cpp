//*********************************************************
//	Get_Node_Data.cpp - read the node file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool Relocate::Get_Node_Data (Node_File &file, Node_Data &node_rec)
{
	if (!Data_Service::Get_Node_Data (file, node_rec)) return (false);

	if (target_flag) {
		node_rec.Subarea (2);

		Int_Map_Itr map_itr = node_map.find (node_rec.Node ());
		if (map_itr != node_map.end ()) {
			Node_Data *node_ptr = &node_array [map_itr->second];

			if (abs (node_ptr->X () - node_rec.X ()) > max_xy_diff ||
				abs (node_ptr->Y () - node_rec.Y ()) > max_xy_diff) {

				target_node_map.insert (Int_Map_Data (node_rec.Node (), (int) node_array.size ()));
				node_array.push_back (node_rec);
			} else {
				node_ptr->Subarea (1);
			}
			return (false);
		}
	} else {
		node_rec.Subarea (0);
	}
	return (true);
}
