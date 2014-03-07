//*********************************************************
//	Get_Route_Nodes.cpp - read the route nodes file
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Route_Nodes_Data
//---------------------------------------------------------

bool ArcNet::Get_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data_rec)
{
	static int last_node, node_count;

	if (arcview_route_nodes.Is_Open ()) {

		//---- process a header line ----

		if (!file.Nested ()) {
			arcview_route_nodes.Copy_Fields (file);

			last_node = node_count = 0;
			arcview_route_nodes.clear ();

			data_rec.Route (file.Route ());
			if (data_rec.Route () == 0) return (false);
			return (true);
		}

		//---- process a nested record ----

		int node;
		Int_Map_Itr itr;
		Node_Data *node_ptr;
		XYZ_Point point;

		node = abs (file.Node ());
	
		itr = node_map.find (node);
		if (itr == node_map.end ()) {
			Warning (String ("Route %d Node %d was Not Found") % file.Route () % node);
			return (false);
		}
		node_ptr = &node_array [itr->second];

		//---- check for link shapes ----

		if (last_node > 0) {
			Int2_Map_Itr map_itr = ab_map.find (Int2_Key (last_node, node));
			if (map_itr != ab_map.end ()) {
				Dir_Data *dir_ptr = &dir_array [map_itr->second];
				Link_Data *link_ptr = &link_array [dir_ptr->Link ()];

				if (link_ptr->Shape () >= 0) {
					Shape_Data *shape_ptr = &shape_array [link_ptr->Shape ()];

					if (dir_ptr->Dir () == 0) {
						XYZ_Itr xyz_itr;

						for (xyz_itr = shape_ptr->begin (); xyz_itr != shape_ptr->end (); xyz_itr++) {
							point.x = UnRound (xyz_itr->x);
							point.y = UnRound (xyz_itr->y);
							point.z = UnRound (xyz_itr->z);

							arcview_route_nodes.push_back (point);
						}
					} else {
						XYZ_RItr xyz_ritr;

						for (xyz_ritr = shape_ptr->rbegin (); xyz_ritr != shape_ptr->rend (); xyz_ritr++) {
							point.x = UnRound (xyz_ritr->x);
							point.y = UnRound (xyz_ritr->y);
							point.z = UnRound (xyz_ritr->z);

							arcview_route_nodes.push_back (point);
						}
					}
				}
			}
		}
		last_node = node;
		node_count++;

		point.x = UnRound (node_ptr->X ());
		point.y = UnRound (node_ptr->Y ());
		point.z = UnRound (node_ptr->Z ());
		
		arcview_route_nodes.push_back (point);

		if (node_count == file.Num_Nest ()) {
			if (!arcview_route_nodes.Write_Record ()) {
				Error (String ("Writing %s") % arcview_route_nodes.File_Type ());
			}
		}
	}
	return (true);
}
