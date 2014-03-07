//*********************************************************
//	Platform_Link.cpp - insert platform links
//*********************************************************

#include "RoutePrep.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Platform_Link
//---------------------------------------------------------

void RoutePrep::Platform_Link (int link)
{
	int i, node_index, link_index, dir_index;
	String name;

	Node_Data node_rec;
	Link_Data *link_ptr, link_rec;
	Dir_Data dir_rec;
	Points points;
	Points_Itr a_itr, b_itr;

	if (link < 0) return;

	link_ptr = &link_array [link];
	link_rec = *link_ptr;

	link_rec.Use (Use_Code ("WALK"));
	link_rec.Type (WALKWAY);
	link_rec.Divided (1);

	if (!link_ptr->Name ().empty ()) {
		name (link_ptr->Name ());
	}
	if (name.empty ()) {
		name = "Platform";
	} else {
		name += " Platform";
	}
	link_rec.Name (name);
	link_rec.Notes ("Station Platform");
	node_rec.Notes (name);

	dir_rec.Lanes (1);
	dir_rec.Speed (Internal_Units (3.0, MPH));
	dir_rec.Time0 (station_length / UnRound (dir_rec.Speed ()));
	dir_rec.Capacity (1000);

	for (i=0; i < 2; i++) {
		link_ptr = &link_array [link];

		if (!Link_Shape (link_ptr, i, points, -1.0, -1.0, platform_offset)) {
			Warning ("Problem with Platform Link ") << link_ptr->Link ();
		}

		//---- update link data and create station link ----

		node_index = (int) node_array.size ();
		link_index = (int) link_array.size ();
		dir_index = (int) dir_array.size ();
	
		a_itr = points.begin ();
		b_itr = a_itr + 1;
		link_rec.Anode (node_index);
		link_rec.Bnode (node_index + 1);

		link_rec.AB_Dir (dir_index);

		dir_rec.Link (link_index);
		dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir_index));
		dir_array.push_back (dir_rec);

		link_rec.BA_Dir (++dir_index);

		dir_rec.Dir (1);

		dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir_index));
		dir_array.push_back (dir_rec);

		link_rec.Notes ("Station Platform");

		link_rec.Link (new_link++);
		link_map.insert (Int_Map_Data (link_rec.Link (), link_index));
		link_array.push_back (link_rec);

		//---- insert new nodes ----

		node_rec.Node (new_node++);
		node_rec.X (a_itr->x);
		node_rec.Y (a_itr->y);
		node_rec.Z (0);

		node_map.insert (Int_Map_Data (node_rec.Node (), node_index));
		node_array.push_back (node_rec);

		node_index++;

		node_rec.Node (new_node++);
		node_rec.X (b_itr->x);
		node_rec.Y (b_itr->y);

		node_map.insert (Int_Map_Data (node_rec.Node (), node_index));
		node_array.push_back (node_rec);

		station_nodes.back ().push_back (link_rec.Anode ());
		station_nodes.back ().push_back (link_rec.Bnode ());
	}
}
