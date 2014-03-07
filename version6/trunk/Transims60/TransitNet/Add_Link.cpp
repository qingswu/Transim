//*********************************************************
//	Add_Link.cpp - add a link to the network
//*********************************************************

#include "TransitNet.hpp"

#include <math.h>

//---------------------------------------------------------
//	Add_Link
//---------------------------------------------------------

void TransitNet::Add_Link (int anode, int bnode)
{
	int dir, *list, a, b;
	double dx, dy;

	Link_Data link_data;
	Dir_Data dir_data;
	Node_Data *node_ptr;
	Int_Map_Itr map_itr;

	//---- initialize the stop list ----

	Int_Map list_rec;
	dir_stop_array.push_back (list_rec);
	dir_stop_array.push_back (list_rec);

	//---- add to the link list ----

	dir_list.push_back (-1);
	dir_list.push_back (-1);

	local_access.push_back (0);
	local_access.push_back (0);

	fare_zone.push_back (0);
	fare_zone.push_back (0);

	if (max_link == MAX_INTEGER) max_link = 0;

	link_data.Link (++max_link);
	link_data.Anode (anode);
	link_data.Bnode (bnode);

	node_ptr = &node_array [anode];
	dx = node_ptr->X ();
	dy = node_ptr->Y ();
	a = node_ptr->Node ();

	node_ptr = &node_array [bnode];
	dx = dx - node_ptr->X ();
	dy = dy - node_ptr->Y ();
	b = node_ptr->Node ();
	
	link_data.Length ((int) sqrt (dx * dx + dy * dy));
	link_data.Type (MINOR);
	link_data.Use (Use_Code ("ANY"));
	link_data.Area_Type (3);
	link_data.Name ("Transit Link");

	dir = (int) dir_array.size ();
	link_data.AB_Dir (dir);
	ab_map.insert (Int2_Map_Data (Int2_Key (a, b), dir));

	list = &node_list [anode];

	dir_list [dir] = *list;
	*list = dir;
	
	local_access [dir] = 0;

	dir_data.Link ((int) link_array.size ());
	dir_data.Dir (0);
	dir_data.Lanes (1);
	dir_data.Speed (Round (25));
	dir_data.Capacity (1000);
	dir_data.Time0 ((int) (link_data.Length () / 25.0 + 0.99));

	dir_map.insert (Int_Map_Data (dir_data.Link_Dir (), dir));

	dir_array.push_back (dir_data);

	dir = (int) dir_array.size ();
	link_data.BA_Dir (dir);
	ab_map.insert (Int2_Map_Data (Int2_Key (b, a), dir));

	list = &node_list [bnode];

	dir_list [dir] = *list;
	*list = dir;
	
	local_access [dir] = 0;

	dir_data.Dir (1);

	dir_map.insert (Int_Map_Data (dir_data.Link_Dir (), dir));

	dir_array.push_back (dir_data);

	link_map.insert (Int_Map_Data (link_data.Link (), (int) link_array.size ()));

	link_array.push_back (link_data);
}
