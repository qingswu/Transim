//*********************************************************
//	Get_Connect_Data.cpp - read the connection file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Connect_Data
//---------------------------------------------------------

bool Relocate::Get_Connect_Data (Connect_File &file, Connect_Data &connect_rec)
{
	if (!target_flag) {
		if (!Data_Service::Get_Connect_Data (file, connect_rec)) return (false);
		return (true);
	}
	int link, dir, node, dir_index, to_index, lanes, low, high;
	bool new_flag;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;

	//---- check/convert the link number and direction ----
	
	link = file.Link ();
	dir = file.Dir ();
	new_flag = false;

	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("Connection %d Link %d was Not Found") % Progress_Count () % link);
		return (false);
	}
	
	link_ptr = &link_array [map_itr->second];

	if (link_ptr->Divided () == 3) {
		map_itr = target_link_map.find (link);
		if (map_itr != target_link_map.end ()) {
			link_ptr = &link_array [map_itr->second];
			new_flag = true;
		}
	} else if (link_ptr->Divided () == 2) {
		new_flag = true;
	}
	if (dir) {
		dir_index = link_ptr->BA_Dir ();
		node = link_ptr->Anode ();
	} else {
		dir_index = link_ptr->AB_Dir ();
		node = link_ptr->Bnode ();
	}
	if (dir_index < 0) {
		Warning (String ("Connection %d Link %d Direction %s was Not Found") % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}
	connect_rec.Dir_Index (dir_index);
	connect_rec.Type (file.Type ());

	//---- convert the to-link number ----

	link = file.To_Link ();

	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("Connection %d Link %d was Not Found") % Progress_Count () % link);
		return (false);
	}
	link = map_itr->second;

	link_ptr = &link_array [link];

	if (link_ptr->Divided () == 3) {
		map_itr = target_link_map.find (link);
		if (map_itr != target_link_map.end ()) {
			link_ptr = &link_array [map_itr->second];
			new_flag = true;
		}
	} else if (link_ptr->Divided () == 2) {
		new_flag = true;
	}
	if (!new_flag) return (false);

	if (link_ptr->Anode () == node) {
		to_index = link_ptr->AB_Dir ();
	} else if (link_ptr->Bnode () == node) {
		to_index = link_ptr->BA_Dir ();
	} else {
		Node_Data *ptr = &node_array [node];
		Warning (String ("Connection %d Node %d is Not on Link %d") % Progress_Count () % 
			ptr->Node () % link_ptr->Link ());
		return (false);
	}
	connect_rec.To_Index (to_index);
	connect_rec.Node (node);

	//---- lane numbers ----

	lanes = file.Lanes ();

	Convert_Lane_Range (dir_index, lanes, low, high);
	
	connect_rec.Low_Lane (low);
	connect_rec.High_Lane (high);

	lanes = file.To_Lanes ();

	Convert_Lane_Range (to_index, lanes, low, high);

	connect_rec.To_Low_Lane (low);
	connect_rec.To_High_Lane (high);

	//---- optional fields ----
	
	connect_rec.Penalty (file.Penalty ());
	connect_rec.Speed (file.Speed ());
	connect_rec.Capacity (file.Capacity ());

	return (true);
}
