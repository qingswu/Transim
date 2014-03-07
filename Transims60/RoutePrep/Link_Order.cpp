//*********************************************************
//	Link_Order.cpp - arrange route links in order
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Link_Order
//---------------------------------------------------------

void RoutePrep::Link_Order (void)
{
	int order, num, best_last, best_first, node, dir;
	double dx, dy, dist, last_dist, first_dist;
	Dtime headway, head;
	String name;
	bool first, flag, warn_flag;

	Int2_Set_Itr int2_itr, from_itr;
	Int2_Set *order_link_ptr;
	Dtime_Itr head_itr;
	Route_Link_Data route_link, *route_link_ptr;
	Route_Link_Map_Stat route_stat;
	Route_Link_Map_Itr route_itr;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr, *first_ptr, *last_ptr;
	Dir_Data *dir_ptr;
	Int_Itr int_itr;
	Integers ab_links, ba_links;

	Route_Data route_rec;

	struct {
		int first;
		int last;
		Integers list;
		void Clear (void)  { first = last = -1; list.clear (); }
	} ab_list, ba_list, ab_sort, ba_sort;

	route_rec.mode = (route_stop_flag) ? LRT : LOCAL_BUS;
	route_rec.oneway = true;
	route_rec.flip = false;
	warn_flag = false;

	//---- arrange links and convert to nodes ----

	for (route_itr = route_link_map.begin (); route_itr != route_link_map.end (); route_itr++) {
		route_link_ptr = &route_itr->second;
		order_link_ptr = &route_link_ptr->links;
		num = (int) order_link_ptr->size ();

		if (route_stop_flag) {

			//---- save the route record ----

			route_rec.name = route_itr->first;
			route_rec.headway.swap (route_itr->second.headways);
			route_rec.nodes.clear ();
			node = -1;
		
			for (int2_itr = order_link_ptr->begin (); int2_itr != order_link_ptr->end (); int2_itr++) {
				dir = int2_itr->second;

				dir_ptr = &dir_array [dir];
				link_ptr = &link_array [dir_ptr->Link ()];

				if (dir_ptr->Dir () == 0) {
					if (link_ptr->Anode () != node) {
						route_rec.nodes.push_back (link_ptr->Anode ());
					}
					node = link_ptr->Bnode ();
					route_rec.nodes.push_back (node);
				} else {
					if (link_ptr->Bnode () != node) {
						route_rec.nodes.push_back (link_ptr->Bnode ());
					}
					node = link_ptr->Anode ();
					route_rec.nodes.push_back (node);
				}
			}
			Save_Route (route_rec);
			continue;
		}

		ab_list.Clear ();
		ba_list.Clear ();
		order = -1;
		first = true;

		for (int2_itr = order_link_ptr->begin (); ; int2_itr++) {
			if (route_link_field < 0) {
				if (int2_itr == order_link_ptr->end ()) break;
				ab_list.list.push_back (int2_itr->second);
				continue;
			}
			if (int2_itr == order_link_ptr->end () || order != int2_itr->first) {
				if (order >= 0) {

					//---- build the ab ordered list ----

					ab_sort.Clear ();
					flag = false;
retry_ab:
					for (int_itr = ab_links.begin (); int_itr != ab_links.end (); int_itr++) {
						if (*int_itr < 0) continue;

						link_ptr = &link_array [*int_itr];

						if (ab_sort.first < 0) {
							if (link_ptr->AB_Dir () < 0) break;
							ab_sort.first = link_ptr->Anode ();
							ab_sort.last = link_ptr->Bnode ();
							ab_sort.list.push_back (link_ptr->AB_Dir ());
						} else if (link_ptr->AB_Dir () >= 0 && link_ptr->Anode () == ab_sort.last) {
							ab_sort.last = link_ptr->Bnode ();
							ab_sort.list.push_back (link_ptr->AB_Dir ());
						} else if (link_ptr->BA_Dir () >= 0 && link_ptr->Bnode () == ab_sort.last) {
							ab_sort.last = link_ptr->Anode ();
							ab_sort.list.push_back (link_ptr->BA_Dir ());
						} else if (link_ptr->AB_Dir () >= 0 && link_ptr->Bnode () == ab_sort.first) {
							ab_sort.first = link_ptr->Anode ();
							ab_sort.list.insert (ab_sort.list.begin (), link_ptr->AB_Dir ());
						} else if (link_ptr->BA_Dir () >= 0 && link_ptr->Anode () == ab_sort.first) {
							ab_sort.first = link_ptr->Bnode ();
							ab_sort.list.insert (ab_sort.list.begin (), link_ptr->BA_Dir ());
						} else {
							continue;
						}
						*int_itr = -1;
						int_itr = ab_links.begin ();
					}

					//---- check for unassigned links ----

					best_last = best_first = -1;
					last_dist = first_dist = MAX_INTEGER;
					first_ptr = last_ptr = 0;

					for (int_itr = ab_links.begin (); int_itr != ab_links.end (); int_itr++) {
						if (*int_itr < 0) continue;

						if (first_ptr == 0) {
							first_ptr = &node_array [ab_sort.first];
						}
						if (last_ptr == 0) {
							last_ptr = &node_array [ab_sort.last];
						}
						link_ptr = &link_array [*int_itr];

						node_ptr = &node_array [link_ptr->Anode ()];
						dx = node_ptr->X () - first_ptr->X ();
						dy = node_ptr->Y () - first_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < first_dist) {
							first_dist = dist;
							best_first = link_ptr->Anode ();
						}
						dx = node_ptr->X () - last_ptr->X ();
						dy = node_ptr->Y () - last_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < last_dist) {
							last_dist = dist;
							best_last = link_ptr->Anode ();
						}

						node_ptr = &node_array [link_ptr->Bnode ()];
						dx = node_ptr->X () - first_ptr->X ();
						dy = node_ptr->Y () - first_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < first_dist) {
							first_dist = dist;
							best_first = link_ptr->Bnode ();
						}
						dx = node_ptr->X () - last_ptr->X ();
						dy = node_ptr->Y () - last_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < last_dist) {
							last_dist = dist;
							best_last = link_ptr->Bnode ();
						}
					}
					if (best_first >= 0 || best_last >= 0) {
						if (first_dist < last_dist) {
							Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % first_ptr->Node () % node_array [best_first].Node ());
							ab_sort.first = best_first;
							warn_flag = true;
						} else {
							Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % last_ptr->Node () % node_array [best_last].Node ());
							ab_sort.last = best_last;
							warn_flag = true;
						}
						flag = true;
						goto retry_ab;
					}

					//----  build the ba ordered list ----

					ba_sort.Clear ();
retry_ba:
					for (int_itr = ba_links.begin (); int_itr != ba_links.end (); int_itr++) {
						if (*int_itr < 0) continue;

						link_ptr = &link_array [*int_itr];

						if (ba_sort.first < 0) {
							if (link_ptr->BA_Dir () < 0) break;
							ba_sort.first = link_ptr->Bnode ();
							ba_sort.last = link_ptr->Anode ();
							ba_sort.list.push_back (link_ptr->BA_Dir ());
						} else if (link_ptr->AB_Dir () >= 0 && link_ptr->Anode () == ba_sort.last) {
							ba_sort.last = link_ptr->Bnode ();
							ba_sort.list.push_back (link_ptr->AB_Dir ());
						} else if (link_ptr->BA_Dir () >= 0 && link_ptr->Bnode () == ba_sort.last) {
							ba_sort.last = link_ptr->Anode ();
							ba_sort.list.push_back (link_ptr->BA_Dir ());
						} else if (link_ptr->AB_Dir () >= 0 && link_ptr->Bnode () == ba_sort.first) {
							ba_sort.first = link_ptr->Anode ();
							ba_sort.list.insert (ba_sort.list.begin (), link_ptr->AB_Dir ());
						} else if (link_ptr->BA_Dir () >= 0 && link_ptr->Anode () == ba_sort.first) {
							ba_sort.first = link_ptr->Bnode ();
							ba_sort.list.insert (ba_sort.list.begin (), link_ptr->BA_Dir ());
						} else {
							continue;
						}
						*int_itr = -1;
						int_itr = ba_links.begin ();
					}

					//---- check for unassigned links ----

					best_last = best_first = -1;
					last_dist = first_dist = MAX_INTEGER;
					first_ptr = last_ptr = 0;

					for (int_itr = ba_links.begin (); int_itr != ba_links.end (); int_itr++) {
						if (*int_itr < 0) continue;

						if (first_ptr == 0) {
							first_ptr = &node_array [ba_sort.first];
						}
						if (last_ptr == 0) {
							last_ptr = &node_array [ba_sort.last];
						}
						link_ptr = &link_array [*int_itr];

						node_ptr = &node_array [link_ptr->Anode ()];
						dx = node_ptr->X () - first_ptr->X ();
						dy = node_ptr->Y () - first_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < first_dist) {
							first_dist = dist;
							best_first = link_ptr->Anode ();
						}
						dx = node_ptr->X () - last_ptr->X ();
						dy = node_ptr->Y () - last_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < last_dist) {
							last_dist = dist;
							best_last = link_ptr->Anode ();
						}

						node_ptr = &node_array [link_ptr->Bnode ()];
						dx = node_ptr->X () - first_ptr->X ();
						dy = node_ptr->Y () - first_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < first_dist) {
							first_dist = dist;
							best_first = link_ptr->Bnode ();
						}
						dx = node_ptr->X () - last_ptr->X ();
						dy = node_ptr->Y () - last_ptr->Y ();

						dist = sqrt (dx * dx + dy * dy);

						if (dist < last_dist) {
							last_dist = dist;
							best_last = link_ptr->Bnode ();
						}
					}
					if (best_first >= 0 || best_last >= 0) {
						if (first_dist < last_dist) {
							if (!flag) Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % first_ptr->Node () % node_array [best_first].Node ());
							ba_sort.first = best_first;
							warn_flag = true;
						} else {
							if (!flag) Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % last_ptr->Node () % node_array [best_last].Node ());
							ba_sort.last = best_last;
							warn_flag = true;
						}
						goto retry_ba;
					}

					//---- merge segments ----

					if (ab_list.first < 0 && ba_list.first < 0) {
						ab_list.first = ab_sort.first;
						ab_list.last = ab_sort.last;
						ab_list.list.swap (ab_sort.list);
						ba_list.first = ba_sort.first;
						ba_list.last = ba_sort.last;
						ba_list.list.swap (ba_sort.list);
					} else {
						if (first) {
							first = false;

							if (ba_list.last == ab_sort.first || ba_list.last == ba_sort.first) {
								ab_list.first = ba_list.first;
								ab_list.last = ba_list.last;
								ab_list.list.swap (ba_list.list);
							} else if (ab_list.last != ab_sort.first && ab_list.last != ba_sort.first) {

								//---- join to the closest end ----

								last_ptr = &node_array [ab_list.last];

								first_ptr = &node_array [ab_sort.first];
								dx = first_ptr->X () - last_ptr->X ();
								dy = first_ptr->Y () - last_ptr->Y ();

								first_dist = sqrt (dx * dx + dy * dy);

								node_ptr = &node_array [ba_sort.first];

								dx = node_ptr->X () - last_ptr->X ();
								dy = node_ptr->Y () - last_ptr->Y ();

								dist = sqrt (dx * dx + dy * dy);

								if (dist < first_dist) {
									first_dist = dist;
									best_first = ba_sort.first;
								} else {
									best_first = ab_sort.first;
								}

								first_ptr = &node_array [ab_list.first];

								last_ptr = &node_array [ab_sort.last];
								dx = first_ptr->X () - last_ptr->X ();
								dy = first_ptr->Y () - last_ptr->Y ();

								last_dist = sqrt (dx * dx + dy * dy);

								node_ptr = &node_array [ba_sort.last];

								dx = node_ptr->X () - first_ptr->X ();
								dy = node_ptr->Y () - first_ptr->Y ();

								dist = sqrt (dx * dx + dy * dy);

								if (dist < last_dist) {
									last_dist = dist;
									best_last = ba_sort.last;
								} else {
									best_last = ab_sort.last;
								}

								if (last_dist < first_dist) {
									Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % node_array [ab_list.first].Node () % node_array [best_last].Node ());
									ab_list.first = best_last;
									warn_flag = true;
								} else {
									Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % node_array [ab_list.last].Node () % node_array [best_first].Node ());
									ab_list.last = best_first;
									warn_flag = true;
								}
							}
						}
						if (ab_sort.first != ab_list.last && ba_sort.first != ab_list.last) {

							//---- join to the closest end ----

							last_ptr = &node_array [ab_list.last];

							first_ptr = &node_array [ab_sort.first];
							dx = first_ptr->X () - last_ptr->X ();
							dy = first_ptr->Y () - last_ptr->Y ();

							first_dist = sqrt (dx * dx + dy * dy);

							node_ptr = &node_array [ba_sort.first];

							dx = node_ptr->X () - last_ptr->X ();
							dy = node_ptr->Y () - last_ptr->Y ();

							dist = sqrt (dx * dx + dy * dy);

							if (dist < first_dist) {
								Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % last_ptr->Node () % node_ptr->Node ());
								ba_sort.first = ab_list.last;
								warn_flag = true;
							} else {
								Warning (String ("Route %s Gap between Nodes %d-%d Joined") % route_itr->first % last_ptr->Node () % first_ptr->Node ());
								ab_sort.first = ab_list.last;
								warn_flag = true;
							}
						}
						if (ba_sort.first == ab_list.last) {
							ab_list.list.insert (ab_list.list.end (), ba_sort.list.begin (), ba_sort.list.end ());
							ab_list.last = ba_sort.last;
						} else {
							ab_list.list.insert (ab_list.list.end (), ab_sort.list.begin (), ab_sort.list.end ());
							ab_list.last = ab_sort.last;							
						}
					}
				}
				if (int2_itr == order_link_ptr->end ()) break;

				order = int2_itr->first;
				ab_links.clear ();
				ba_links.clear ();
			}
			ab_links.push_back (int2_itr->second);
			ba_links.push_back (int2_itr->second);
		}

		//---- save the route record ----

		route_rec.name = route_itr->first;
		route_rec.headway.swap (route_itr->second.headways);
		route_rec.nodes.clear ();
		node = -1;

		for (int_itr = ab_list.list.begin (); int_itr != ab_list.list.end (); int_itr++) {
			if (*int_itr < 0) continue;

			dir_ptr = &dir_array [*int_itr];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (dir_ptr->Dir () == 0) {
				if (link_ptr->Anode () != node) {
					route_rec.nodes.push_back (link_ptr->Anode ());
				}
				node = link_ptr->Bnode ();
				route_rec.nodes.push_back (node);
			} else {
				if (link_ptr->Bnode () != node) {
					route_rec.nodes.push_back (link_ptr->Bnode ());
				}
				node = link_ptr->Anode ();
				route_rec.nodes.push_back (node);
			}
		}
		Save_Route (route_rec);
	}
	if (warn_flag) Show_Message (1);
}
