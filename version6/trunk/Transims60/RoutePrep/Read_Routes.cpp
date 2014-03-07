//*********************************************************
//	Read_Routes.cpp - convert the route shapes to routes
//*********************************************************

#include "RoutePrep.hpp"

#include "Shape_Tools.hpp"
#include <math.h>

//---------------------------------------------------------
//	Read_Routes
//---------------------------------------------------------

void RoutePrep::Read_Routes (void)
{
	int i, link, order, num, anode, bnode, part, part0, part1;
	double fvalue, offset1, offset2, length, dx, dy;
	Dtime headway, head;
	String name;
	Strings parse;

	Points points, link_pts;
	Points_Itr pt_itr, pt0_itr;
	Route_Link_Data route_link, *route_link_ptr;
	Route_Link_Map_Stat route_stat;
	Int_Map_Itr map_itr;
	Int_Itr int_itr;
	Stop_Offset_Map *stop_offset_ptr = 0;
	Route_Stop_Map_Itr route_stop_itr;
	Stop_Offset_Map_Itr stop_offset_itr;
	Shape_File_Itr shape_itr;

	route_link.headways.assign (num_periods, 0);

	//---- read route link files ----

	for (shape_itr = shape_file_array.begin (); shape_itr != shape_file_array.end (); shape_itr++) {
		route_shape_file = *shape_itr;

		Show_Message (String ("Reading %s -- Record") % route_shape_file->File_Type ());
		Set_Progress ();

		order = 1;
		if (convert_flag) {
			convert.New_File_Pointer (0, route_shape_file);
		}

		while (route_shape_file->Read_Record ()) {
			Show_Progress ();

			for (part = 1, part0 = 0; part <= route_shape_file->Part_Size (); part++, part0 = part1) {
				if (part < route_shape_file->Part_Size ()) {
					part1 = route_shape_file->parts [part];
				} else {
					part1 = (int) route_shape_file->size ();
				}
				points.clear ();

				for (i=part0; i < part1; i++) {
					points.push_back (route_shape_file->at (i));
				}

				//---- copy fields and execute user program ----

				new_link_file->Reset_Record ();

				new_link_file->Copy_Fields (*route_shape_file);

				if (route_name_field < 0) {
					parse = route_shape_file->File_Parse ();
					new_route_nodes->Name (parse [1]);
				}

				if (convert_flag) {
					if (convert.Execute (-1, part) <= 0) continue;
				}
				name = route_shape_file->Get_String (route_name_field);
				name.Trim ();

				if (name.empty ()) {
					name = new_route_nodes->Name ();
				}

				//---- get the route stop data ----

				if (route_stop_flag) {
					route_stop_itr = route_stop_map.find (name);

					if (route_stop_itr == route_stop_map.end ()) continue;

					stop_offset_ptr = &route_stop_itr->second;
				}

				//---- retrieve the route link data pointer ----

				route_stat = route_link_map.insert (Route_Link_Map_Data (name, route_link));

				route_link_ptr = &route_stat.first->second;

				if (route_stop_flag) {

					//---- create links between stops ----

					offset1 = 0.0;
					anode = -1;

					for (stop_offset_itr = stop_offset_ptr->begin (); stop_offset_itr != stop_offset_ptr->end (); stop_offset_itr++) {
						bnode = stop_offset_itr->second.stop;
						offset2 = stop_offset_itr->second.offset;

						if (anode == bnode) continue;

						//---- create a node at the beginning of the link ----

						if (anode < 0 && offset2 > 300) {
							anode = new_node++;
						}

						if (anode >= 0) {

							//---- get the shape points for the link ----

							length = offset2 - offset1;
							points.assign (route_shape_file->begin (), route_shape_file->end ());

							if (!Sub_Shape (points, offset1, length)) continue;

							link = Insert_Link (points); 
							if (link < 0) continue;

							route_link_ptr->links.insert (Int2_Key (stop_offset_itr->first, link));
						}
						offset1 = offset2;
						anode = bnode;
					}

				} else if (route_link_field >= 0) {

					//---- add the link to the link sort ----

					order = route_shape_file->Get_Integer (route_order_field);

					link = route_shape_file->Get_Integer (route_link_field);

					map_itr = link_map.find (link);

					if (map_itr == link_map.end ()) {
						points.assign (route_shape_file->begin (), route_shape_file->end ());

						link = Insert_Link (points); 
						if (link < 0) continue;
					}  else {
						link = map_itr->second;
					}
					route_link_ptr->links.insert (Int2_Key (order, link));

				} else {

					//---- check the shape length ----

					length = 0;
					pt0_itr = points.begin ();

					for (pt_itr = pt0_itr; pt_itr != points.end (); pt0_itr = pt_itr++) {
						dx = pt_itr->x - pt0_itr->x;
						dy = pt_itr->y - pt0_itr->y;

						length += sqrt (dx * dx + dy * dy);
					}
					if (length > 1.5 * link_length) {
						length = 0;
						link_pts.clear ();
						pt0_itr = points.begin ();
						link_pts.push_back (*pt0_itr);

						//---- split the shape into link segments ----

						for (pt_itr = pt0_itr; pt_itr != points.end (); pt0_itr = pt_itr++) {
							dx = pt_itr->x - pt0_itr->x;
							dy = pt_itr->y - pt0_itr->y;

							length += sqrt (dx * dx + dy * dy);
							link_pts.push_back (*pt_itr);

							if (length > link_length * 0.95 || pt_itr == points.end ()-1) {
								link = Insert_Link (link_pts);

								route_link_ptr->links.insert (Int2_Key (order++, link));

								length = 0;
								link_pts.clear ();
								link_pts.push_back (*pt_itr);
							}
						}

					} else {

						//---- save the link ----

						link = Insert_Link (points);

						route_link_ptr->links.insert (Int2_Key (order++, link));
					}
				}

				//---- retrieve headway data ----

				for (num=0, int_itr = route_headway_flds.begin (); int_itr != route_headway_flds.end (); int_itr++, num++) {
					if (*int_itr >= 0) {
						fvalue = route_shape_file->Get_Double (*int_itr);
						if (fvalue > 0.0) {
							if (route_freq_flag) {
								fvalue = 60.0 / fvalue;
							}
							head = route_link_ptr->headways [num];
							headway = Dtime (fvalue, MINUTES);
							if (head == 0 || headway < head) {
								route_link_ptr->headways [num] = headway;
							}
						}
					}
				}
			}
		}
		End_Progress ();

		route_shape_file->Close ();

		Print (2, String ("Number of %s Records = %d") % route_shape_file->File_Type () % Progress_Count ());
	}
}
