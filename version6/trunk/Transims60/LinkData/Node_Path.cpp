//*********************************************************
//	Node_Path.cpp - build a path between two nodes
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Node_Path
//---------------------------------------------------------

int LinkData::Node_Path (int node1, int node2)
{
	int anode, bnode, best_cum, cum_a, cum_b;

	Node_Path_Data path_data, *first_ptr, *last_ptr, *node_ptr;
	Integers *links_ptr;
	Int_Itr int_itr;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	//---- build node link list ----

	if (node_links.size () == 0) {
		Integers links;
		Link_Itr link_itr;

		node_links.assign (node_array.size (), links);

		for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
			if (link_itr->AB_Dir () >= 0) {
				links_ptr = &node_links [link_itr->Anode ()];
				links_ptr->push_back (link_itr->AB_Dir ());
			}
			if (link_itr->BA_Dir () >= 0) {
				links_ptr = &node_links [link_itr->Bnode ()];
				links_ptr->push_back (link_itr->BA_Dir ());
			}
		}
	}
	path_data.next_list = path_data.dir_index = -1;
	node_path_array.assign (node_array.size (), path_data);

	first_ptr = &path_data;
	first_ptr->next_list = node1;
	last_ptr = &node_path_array [node1];

	//---- add links leaving the origin node ----

	links_ptr = &node_links [node1];

	for (int_itr = links_ptr->begin (); int_itr != links_ptr->end (); int_itr++) {
		dir_ptr = &dir_array [*int_itr];
		link_ptr = &link_array [dir_ptr->Link ()];

		if (dir_ptr->Dir () == 0) {
			bnode = link_ptr->Bnode ();
		} else {
			bnode = link_ptr->Anode ();
		}
		cum_b = link_ptr->Length ();

		node_ptr = &node_path_array [bnode];

		if (node_ptr->next_list == -1 && last_ptr != node_ptr) {
			last_ptr->next_list = bnode;
			last_ptr = node_ptr;
		}
		node_ptr->distance = cum_b;
		node_ptr->from_node = bnode;
		node_ptr->dir_index = *int_itr;
	}
	if (last_ptr == &node_path_array [node1]) return (-1);

	//---- build a path to the destination node ----

	best_cum = MAX_INTEGER;

	for (;;) {
		anode = first_ptr->next_list;
		if (anode < 0) break;

		first_ptr->next_list = -2;

		first_ptr = &node_path_array [anode];
		if (first_ptr->dir_index < 0) continue;

		//---- check the cumulative impedance ----

		cum_a = first_ptr->distance;

		if (cum_a >= best_cum) continue;

		//---- identify the approach link ----

		dir_ptr = &dir_array [first_ptr->dir_index];

		//---- process each link leaving the node ----

		links_ptr = &node_links [anode];

		for (int_itr = links_ptr->begin (); int_itr != links_ptr->end (); int_itr++) {
			dir_ptr = &dir_array [*int_itr];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (dir_ptr->Dir () == 0) {
				bnode = link_ptr->Bnode ();
			} else {
				bnode = link_ptr->Anode ();
			}
			if (bnode == node1) continue;

			//---- check the cumulative distance ----

			cum_b = cum_a + link_ptr->Length ();
			if (cum_b > best_cum) continue;

			node_ptr = &node_path_array [bnode];

			if (cum_b >= node_ptr->distance && node_ptr->distance > 0) continue;

			if (bnode == node2) {
				best_cum = cum_b;
			}

			//---- add to the tree ----

			if (node_ptr->next_list == -2) {
				node_ptr->next_list = first_ptr->next_list;
				first_ptr->next_list = bnode;
			} else if (node_ptr->next_list == -1 && last_ptr != node_ptr) {
				last_ptr->next_list = bnode;
				last_ptr = node_ptr;
			}
			node_ptr->distance = cum_b;
			node_ptr->from_node = anode;
			node_ptr->dir_index = *int_itr;
		}
	}
	if (best_cum == MAX_INTEGER) return (-1);

	node_ptr = &node_path_array [node2];

	return (node_ptr->dir_index);
}
