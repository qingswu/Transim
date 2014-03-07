//*********************************************************
//	Read_Link_Nodes.cpp - Read the Link Node List File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Read_Link_Nodes
//---------------------------------------------------------

void LinkData::Read_Link_Nodes (void)
{
	int i, j, num, link, node, dir_index, anode, bnode;
	bool flag, convert_flag;

	String record;
	Strings parse;
	Str_Itr str_itr;
	Int2_Key ab_key;
	Int2_Map_Stat map_stat;
	Int2_Map_Itr ab_itr;
	Int_Map_Itr itr, map_itr;
	Int_Itr int_itr;
	Link_Nodes *ptr;
	Node_Data *node_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Integers nodes;

	convert_flag = (node_map_flag || input_flag);

	//---- process each link ----
	
	Show_Message (String ("Reading %s -- Record") % link_node.File_Type ());
	Set_Progress ();

	while (link_node.Read ()) {
		Show_Progress ();

		//---- read the field ----

		record = link_node.Record_String ();

		record.Parse (parse);

		ptr = 0;
		link = node = 0;
		flag = false;
		nodes.clear ();

		for (str_itr = parse.begin (); str_itr != parse.end (); str_itr++) {
			if (link == 0) {
				link = str_itr->Integer ();

				if (!convert_flag) {
					itr = link_map.find (link);
					if (itr == link_map.end ()) break;

					ptr = &link_node_array [itr->second];
					ptr->link = link;
				}
			} else if (!str_itr->Equals ("=")) {
				node = str_itr->Integer ();

				if (convert_flag) {
					nodes.push_back (node);
				} else {
					ptr->nodes.push_back (node);
				}
			}
		}
		if (!convert_flag) continue;
		if (nodes.size () < 2) continue;

		//---- check for a link ID match ----

		itr = link_map.find (link);

		if (itr != link_map.end ()) {
			ptr = &link_node_array [itr->second];
//			if (ptr->link != 0) continue;

			//--- compare node numbers ----

			link_ptr = &link_array [itr->second];
			node_ptr = &node_array [link_ptr->Anode ()];

			if (node_ptr->Node () == nodes [0]) {
				ptr->link = link;
				ptr->nodes.swap (nodes);
				continue;
			} else {
				node_ptr = &node_array [link_ptr->Bnode ()];

				if (node_ptr->Node () == nodes.back ()) {
					ptr->link = link;
					ptr->nodes.swap (nodes);
					continue;
				}
			}

			//---- check the nodes ----

			//for (int_itr = nodes.begin (); int_itr != nodes.end (); int_itr++) {
			//	node = *int_itr;

			//	map_itr = input_map.find (node);
			//	if (map_itr != input_map.end ()) {
			//		if (map_itr->second >= 0) {
			//			node_ptr = &node_array [map_itr->second];
			//			if (node_ptr->Control () == node) {
			//				ptr->link = link;
			//				ptr->nodes.swap (nodes);
			//				break;
			//			}
			//		}
			//	}
			//}
			//if (int_itr != nodes.end ()) continue;
		}

		//---- check the anode ----

		node = nodes [0];
		ab_key.first = anode = -1;

		map_itr = input_map.find (node);
		if (map_itr != input_map.end ()) {
			if (map_itr->second >= 0) {
				node_ptr = &node_array [map_itr->second];
				if (node_ptr->Control () >= 0) {
					anode = map_itr->second;
					ab_key.first = node_ptr->Node ();
				}
			}
		}

		//---- check the bnode ----

		node = nodes.back ();
		ab_key.second = bnode = -1;

		map_itr = input_map.find (node);
		if (map_itr != input_map.end ()) {
			if (map_itr->second >= 0) {
				node_ptr = &node_array [map_itr->second];
				if (node_ptr->Control () >= 0) {
					bnode = map_itr->second;
					ab_key.second = node_ptr->Node ();
				}
			}
		}

		//---- check the ab match ----

		if (ab_key.first >= 0 && ab_key.second >= 0) {
			ab_itr = ab_map.find (ab_key);

			if (ab_itr != ab_map.end ()) {
				dir_ptr = &dir_array [ab_itr->second];

				ptr = &link_node_array [dir_ptr->Link ()];

				if (ptr->link == 0) {
					ptr->link = link_array [dir_ptr->Link ()].Link ();

					if (dir_ptr->Dir () == 0) {
						ptr->nodes.swap (nodes);
					} else {
						ptr->nodes.assign (nodes.rbegin (), nodes.rend ());
					}
				}
				continue;
			}

			//---- check the ba direction ----

			ab_itr = ab_map.find (Int2_Key (ab_key.second, ab_key.first));

			if (ab_itr != ab_map.end ()) {
				dir_ptr = &dir_array [ab_itr->second];

				ptr = &link_node_array [dir_ptr->Link ()];

				if (ptr->link == 0) {
					ptr->link = link_array [dir_ptr->Link ()].Link ();

					if (dir_ptr->Dir () == 1) {
						ptr->nodes.swap (nodes);
					} else {
						ptr->nodes.assign (nodes.rbegin (), nodes.rend ());
					}
				}
				continue;
			}

			//---- build a path ----

			dir_index = Node_Path (anode, bnode);

			if (dir_index >= 0) {
				dir_ptr = &dir_array [dir_index];

				ptr = &link_node_array [dir_ptr->Link ()];

				if (ptr->link == 0) {
					ptr->link = link_array [dir_ptr->Link ()].Link ();

					if (dir_ptr->Dir () == 0) {
						ptr->nodes.swap (nodes);
					} else {
						ptr->nodes.assign (nodes.rbegin (), nodes.rend ());
					}
				}
				continue;
			}
		}

		//---- try to find an ab match for shape nodes ----

		num = (int) nodes.size ();

		if (num > 2 && (ab_key.first >= 0 || ab_key.second >= 0)) {

			//---- pivot from anode ----

			if (ab_key.first >= 0) {
				for (i=num - 2; i > 0; i--) {
					node = nodes [i];

					map_itr = input_map.find (node);
					if (map_itr != input_map.end ()) {
						if (map_itr->second >= 0) {
							node_ptr = &node_array [map_itr->second];
							if (node_ptr->Control () >= 0) {
								ab_key.second = node_ptr->Node ();
							}
						}
					}
					if (ab_key.second < 0) continue;

					ab_itr = ab_map.find (ab_key);

					if (ab_itr != ab_map.end ()) {
						dir_ptr = &dir_array [ab_itr->second];

						ptr = &link_node_array [dir_ptr->Link ()];

						if (ptr->link == 0) {
							ptr->link = link_array [dir_ptr->Link ()].Link ();

							if (dir_ptr->Dir () == 0) {
								for (j=0; j <= i; j++) {
									ptr->nodes.push_back (nodes [j]);
								}
							} else {
								for (j=i; j >= 0; j--) {
									ptr->nodes.push_back (nodes [j]);
								}
							}
						}
						break;
					}

					//---- check the ba direction ----

					ab_itr = ab_map.find (Int2_Key (ab_key.second, ab_key.first));

					if (ab_itr != ab_map.end ()) {
						dir_ptr = &dir_array [ab_itr->second];

						ptr = &link_node_array [dir_ptr->Link ()];

						if (ptr->link == 0) {
							ptr->link = link_array [dir_ptr->Link ()].Link ();
							
							if (dir_ptr->Dir () == 1) {
								for (j=0; j <= i; j++) {
									ptr->nodes.push_back (nodes [j]);
								}
							} else {
								for (j=i; j >= 0; j--) {
									ptr->nodes.push_back (nodes [j]);
								}
							}
						}
						break;
					}
					ab_key.second = -1;
				}
				if (i > 0) continue;

			} else {	//---- pivot from bnode ----

				for (i=1; i < num - 1; i++) {
					node = nodes [i];

					map_itr = input_map.find (node);
					if (map_itr != input_map.end ()) {
						if (map_itr->second >= 0) {
							node_ptr = &node_array [map_itr->second];
							if (node_ptr->Control () >= 0) {
								ab_key.first = node_ptr->Node ();
							}
						}
					}
					if (ab_key.first < 0) continue;

					ab_itr = ab_map.find (ab_key);

					if (ab_itr != ab_map.end ()) {
						dir_ptr = &dir_array [ab_itr->second];

						ptr = &link_node_array [dir_ptr->Link ()];

						if (ptr->link == 0) {
							ptr->link = link_array [dir_ptr->Link ()].Link ();

							if (dir_ptr->Dir () == 0) {
								for (j=i; j < num; j++) {
									ptr->nodes.push_back (nodes [j]);
								}
							} else {
								for (j=num-1; j >= i; j--) {
									ptr->nodes.push_back (nodes [j]);
								}
							}
						}
						break;
					}

					//---- check the ba direction ----

					ab_itr = ab_map.find (Int2_Key (ab_key.second, ab_key.first));

					if (ab_itr != ab_map.end ()) {
						dir_ptr = &dir_array [ab_itr->second];

						ptr = &link_node_array [dir_ptr->Link ()];

						if (ptr->link == 0) {
							ptr->link = link_array [dir_ptr->Link ()].Link ();
							
							if (dir_ptr->Dir () == 1) {
								for (j=1; j < num; j++) {
									ptr->nodes.push_back (nodes [j]);
								}
							} else {
								for (j=num-1; j >= 1; j--) {
									ptr->nodes.push_back (nodes [j]);
								}
							}
						}
						break;
					}
					ab_key.first = -1;
				}
				if (i < num - 1) continue;
			}
		}
	}
	End_Progress ();
	nequiv = Progress_Count ();

	link_node.Close ();
}
