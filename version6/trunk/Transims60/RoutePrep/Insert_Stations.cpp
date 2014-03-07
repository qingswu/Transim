//*********************************************************
//	Insert_Stations.cpp - insert station links and nodes
//*********************************************************

#include "RoutePrep.hpp"

#include "Compass.hpp"

//---------------------------------------------------------
//	Insert_Stations
//---------------------------------------------------------

void RoutePrep::Insert_Stations (void)
{
	int node, link1, link2, bearing1, bearing2;

	Int_Itr int_itr, int2_itr;
	Integers node_list;
	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Station_Map_Itr station_itr;
	Station_Data *station_ptr;

	for (station_itr = station_map.begin (); station_itr != station_map.end (); station_itr++) {

		node = station_itr->first;
		station_ptr = &station_itr->second;
		node_ptr = &node_array [node];

		//---- check for station links ----

		if (station_ptr->links.size () == 0) {
			Warning ("No Links are attached to Station Node ") << node_ptr->Node ();
		}

		//---- set the station name ----

		if (!station_ptr->name.empty ()) {
			node_ptr->Notes (station_ptr->name);
		}
		station_nodes.clear ();

		//---- find link combinations ----

		for (int_itr = station_ptr->links.begin (); int_itr != station_ptr->links.end (); int_itr++) {
			link1 = *int_itr;

			bearing1 = 0;

			//---- link1 entry bearing ----

			link_ptr = &link_array [link1];

			if (link_ptr->Bnode () == node) {
				if (link_ptr->AB_Dir () >= 0) {
					dir_ptr = &dir_array [link_ptr->AB_Dir ()];
					bearing1 = dir_ptr->Out_Bearing ();
				} else if (link_ptr->BA_Dir () >= 0) {
					dir_ptr = &dir_array [link_ptr->BA_Dir ()];
					bearing1 = compass.Flip (dir_ptr->In_Bearing ());
				}
			} else if (link_ptr->Anode () == node) {
				if (link_ptr->BA_Dir () >= 0) {
					dir_ptr = &dir_array [link_ptr->BA_Dir ()];
					bearing1 = dir_ptr->Out_Bearing ();
				} else if (link_ptr->AB_Dir () >= 0) {
					dir_ptr = &dir_array [link_ptr->AB_Dir ()];
					bearing1 = compass.Flip (dir_ptr->In_Bearing ());
				}
			} else {
				continue;
			}
			station_nodes.push_back (node_list);

			//---- check the other links ----

			for (int2_itr = int_itr + 1; int2_itr != station_ptr->links.end (); int2_itr++) {
				link2 = *int2_itr;

				//---- link2 exit bearing ----

				link_ptr = &link_array [link2];
				bearing2 = 0;

				if (link_ptr->Bnode () == node) {
					if (link_ptr->AB_Dir () >= 0) {
						dir_ptr = &dir_array [link_ptr->AB_Dir ()];
						bearing2 = dir_ptr->Out_Bearing ();
					} else if (link_ptr->BA_Dir () >= 0) {
						dir_ptr = &dir_array [link_ptr->BA_Dir ()];
						bearing2 = compass.Flip (dir_ptr->In_Bearing ());
					}
				} else if (link_ptr->Anode () == node) {
					if (link_ptr->BA_Dir () >= 0) {
						dir_ptr = &dir_array [link_ptr->BA_Dir ()];
						bearing2 = dir_ptr->Out_Bearing ();
					} else if (link_ptr->AB_Dir () >= 0) {
						dir_ptr = &dir_array [link_ptr->AB_Dir ()];
						bearing2 = compass.Flip (dir_ptr->In_Bearing ());
					}
				} else {
					continue;
				}
				bearing2 = compass.Flip (bearing2);

				//---- check bearing match -----

				if (compass.Difference (bearing1, bearing2) <= compass.Num_Points () / 5) {
					Inline_Station (node, link1, link2);
					break;
				}
			}

			if (int2_itr == station_ptr->links.end ()) {
				Endline_Station (node, link1);
			}
		}
		node_ptr = &node_array [node];
		node_ptr->Node (-node_ptr->Node ());

		Transfer_Link ();
	}
}
