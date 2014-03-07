//*********************************************************
//	Route_Segments.cpp - arrange segment links in order
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Route_Segemtns
//---------------------------------------------------------

void RoutePrep::Route_Segments (void)
{
	int link, dir, segment, node, count;
	bool flag;

	Link_Itr link_itr;
	Ints_Map_Itr ints_map_itr;
	Integers sort_list, link_list, next_list, *list_ptr;
	Int_Itr int_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	//---- gather segment links ----

	for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		segment = link_itr->Area_Type ();
		if (segment <= 0) continue;

		ints_map_itr = segments.find (segment);

		if (ints_map_itr == segments.end ()) {
			link_list.assign (1, link);
			segments.insert (Ints_Map_Data (segment, link_list));
		} else {
			ints_map_itr->second.push_back (link);
		}
	}

	//---- sort each segment list ----

	for (ints_map_itr = segments.begin (); ints_map_itr != segments.end (); ints_map_itr++) {
		segment = ints_map_itr->first;
		list_ptr = &ints_map_itr->second;
		if (list_ptr->size () < 2) continue;

		link = *list_ptr->begin ();
		link_ptr = &link_array [link];

		if (link_ptr->AB_Dir () >= 0) {
			sort_list.assign (1, link_ptr->AB_Dir ());
		} else {
			sort_list.assign (1, link_ptr->BA_Dir ());
		}
		link_list.assign (list_ptr->begin () + 1, list_ptr->end ());

		//---- forward pass ----

		flag = true;

		while (link_list.size () > 0 && flag) {

			dir = *(--sort_list.end ());

			dir_ptr = &dir_array [dir];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (dir_ptr->Dir () == 1) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
			flag = false;
			next_list.clear ();

			for (int_itr = link_list.begin (); int_itr != link_list.end (); int_itr++) {
				link = *int_itr;
				link_ptr = &link_array [link];

				if (link_ptr->AB_Dir () >= 0) {
					if (link_ptr->Anode () == node) {
						node = link_ptr->Bnode ();
						sort_list.push_back (link_ptr->AB_Dir ());
						flag = true;
						continue;
					}
				}
				if (link_ptr->BA_Dir () >= 0) {
					if (link_ptr->Bnode () == node) {
						node = link_ptr->Anode ();
						sort_list.push_back (link_ptr->BA_Dir ());
						flag = true;
						continue;
					}
				}
				next_list.push_back (link);
			}
			link_list.swap (next_list);
		}

		//---- backward pass ----

		flag = true;
		next_list.clear ();

		while (link_list.size () > 0 && flag) {

			dir = *sort_list.begin ();

			dir_ptr = &dir_array [dir];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (dir_ptr->Dir () == 1) {
				node = link_ptr->Bnode ();
			} else {
				node = link_ptr->Anode ();
			}
			next_list.clear ();
			flag = false;

			for (int_itr = link_list.begin (); int_itr != link_list.end (); int_itr++) {
				link = *int_itr;
				link_ptr = &link_array [link];

				if (link_ptr->AB_Dir () >= 0) {
					if (link_ptr->Bnode () == node) {
						node = link_ptr->Anode ();
						sort_list.insert (sort_list.begin (), link_ptr->AB_Dir ());
						flag = true;
						continue;
					}
				}
				if (link_ptr->BA_Dir () >= 0) {
					if (link_ptr->Anode () == node) {
						node = link_ptr->Bnode ();
						sort_list.insert (sort_list.begin (), link_ptr->BA_Dir ());
						flag = true;
						continue;
					}
				}
				next_list.push_back (link);
			}
			link_list.swap (next_list);
		}

		//---- create a node list ----

		list_ptr->clear ();

		for (int_itr = sort_list.begin (); int_itr != sort_list.end (); int_itr++) {
			dir_ptr = &dir_array [*int_itr];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (dir_ptr->Dir () == 1) {
				if (list_ptr->size () == 0) {
					list_ptr->push_back (link_ptr->Bnode ());
				}
				node = link_ptr->Anode ();
			} else {
				if (list_ptr->size () == 0) {
					list_ptr->push_back (link_ptr->Anode ());
				}
				node = link_ptr->Bnode ();
			}
			list_ptr->push_back (node);
		}

		//---- print the segment report ----

		if (segment_report) {
			count = 0;
			Print (2, "Route Segment = ") << segment;

			for (int_itr = list_ptr->begin (); int_itr != list_ptr->end (); int_itr++) {
				if ((++count % 10) == 1) {
					Print (1, "\t");
				} else {
					Print (0, ", ");
				}
				Print (0, (String ("%d") % node_array [*int_itr].Node ()));
			}
		}
		if (link_list.size () > 0) {
			Warning (String ("%d Links could not be Inserted into Segment %d") % link_list.size () % segment);

			if (segment_report) {
				count = 0;
				Print (1, "Missing Links");

				for (int_itr = link_list.begin (); int_itr != link_list.end (); int_itr++) {
					if ((++count % 5) == 1) {
						Print (1, "\t");
					} else {
						Print (0, ", ");
					}
					link_ptr = &link_array [*int_itr];

					Print (0, (String ("%d-%d") % node_array [link_ptr->Anode ()].Node () % node_array [link_ptr->Bnode ()].Node ()));
				}
			}
		}
	}
}
