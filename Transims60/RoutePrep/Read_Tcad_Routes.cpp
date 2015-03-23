//*********************************************************
//	Read_Tcad_Routes.cpp - convert TransCAD route system
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Tcad_Routes
//---------------------------------------------------------

void RoutePrep::Read_Tcad_Routes (void)
{
	int route, last_route, mode, lvalue, dir, stop, node;
	int route_fld, name_fld, mode_fld, head_pk_fld, time_pk_fld, head_op_fld, time_op_fld, link_fld, dir_fld, stop_fld, node_fld;
	bool first;
	String name;

	Route_Node route_node;
	Route_Period route_period;
	Route_Header route_header, *route_ptr;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;

	Route_Nodes_Itr route_itr;
	Route_Node_Itr node_itr;

	Show_Message (String ("Reading %s -- Record") % tcad_route_header.File_Type ());
	Set_Progress ();

	route_fld = tcad_route_header.Field_Number ("ROUTE_ID");
	name_fld = tcad_route_header.Field_Number ("ROUTE_NAME");
	mode_fld = tcad_route_header.Field_Number ("MODE");
	head_pk_fld = tcad_route_header.Field_Number ("HEADWAY_PK");
	time_pk_fld = tcad_route_header.Field_Number ("RUNTIME_PK");
	head_op_fld = tcad_route_header.Field_Number ("HEADWAY_OP");
	time_op_fld = tcad_route_header.Field_Number ("RUNTIME_OP");

	last_route = 0;

	while (tcad_route_header.Read ()) {
		Show_Progress ();

		route = tcad_route_header.Get_Integer (route_fld);

		if (route != last_route) {
			if (last_route > 0) {
				map_stat = route_map.insert (Int_Map_Data (last_route, (int) route_nodes_array.size ()));

				if (!map_stat.second) {
					Warning ("Duplicate Route Number = ") << last_route;
				} else {
					route_nodes_array.push_back (route_header);
				}
			}
			last_route = route;
			route_header.Clear ();
			route_header.Route (route);
			
			name = tcad_route_header.Get_String (name_fld);

			route_header.Name (name);

			mode = tcad_route_header.Get_Integer (mode_fld);

			map_itr = route_mode_map.find (mode);

			if (map_itr == route_mode_map.end ()) {
				Warning (String ("Mode %d was Not Converted") % mode);
				route_mode_map.insert (Int_Map_Data (mode, NO_TRANSIT));
				mode = 0;
			} else {
				mode = map_itr->second;
			}
			route_header.Mode (mode);

			route_header.Veh_Type (mode_type_map [mode]); 

			route_header.Oneway (true);

			route_period.Clear ();

			lvalue = tcad_route_header.Get_Integer (head_pk_fld);
			if (lvalue > 0) {
				route_period.Headway (Dtime (lvalue, MINUTES));

				lvalue = tcad_route_header.Get_Integer (time_pk_fld);

				if (lvalue > 0) {
					route_period.TTime (Dtime (lvalue, MINUTES));
				}
			}
			route_header.periods.push_back (route_period);

			route_period.Clear ();

			lvalue = tcad_route_header.Get_Integer (head_op_fld);
			if (lvalue > 0) {
				route_period.Headway (Dtime (lvalue, MINUTES));

				lvalue = tcad_route_header.Get_Integer (time_op_fld);

				if (lvalue > 0) {
					route_period.TTime (Dtime (lvalue, MINUTES));
				}
			}
			route_header.periods.push_back (route_period);
		}
	}
	if (last_route > 0) {
		map_stat = route_map.insert (Int_Map_Data (last_route, (int) route_nodes_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Route Number = ") << last_route;
		} else {
			route_nodes_array.push_back (route_header);
		}
	}

	//---- route links ----

	Show_Message (String ("Reading %s -- Record") % tcad_route_link.File_Type ());
	Set_Progress ();

	route_fld = tcad_route_link.Field_Number ("ROUTE_ID");
	link_fld = tcad_route_link.Field_Number ("LINK_ID");
	dir_fld = tcad_route_link.Field_Number ("DIRECTION");

	last_route = 0;
	first = true;
	route_ptr = 0;

	while (tcad_route_link.Read ()) {
		Show_Progress ();

		route = tcad_route_link.Get_Integer (route_fld);

		if (route != last_route) {
			map_itr = route_map.find (route);

			if (map_itr != route_map.end ()) {
				last_route = route;
				route_ptr = &route_nodes_array [map_itr->second];
				first = true;
			} else {
				continue;
			}
		}
		lvalue = tcad_route_link.Get_Integer (link_fld);
		name = tcad_route_link.Get_String (dir_fld);

		dir = name.Starts_With ("-") ? 1 : 0;

		map_itr = link_map.find (lvalue);

		if (map_itr == link_map.end ()) {
			Warning (String ("Route %d Link %d was Not Found") % route % lvalue);
			continue;
		}
		link_ptr = &link_array [map_itr->second];

		if (first) {
			first = false;
			if (dir == 1) {
				route_node.Node (link_ptr->Bnode ());
			} else {
				route_node.Node (link_ptr->Anode ());
			}
			route_ptr->nodes.push_back (route_node);
		}
		if (dir == 0) {
			route_node.Node (link_ptr->Bnode ());
		} else {
			route_node.Node (link_ptr->Anode ());
		}
		route_ptr->nodes.push_back (route_node);
	}
	End_Progress ();

	//---- stop node ----

	Show_Message (String ("Reading %s -- Record") % tcad_stop_node.File_Type ());
	Set_Progress ();

	stop_fld = tcad_stop_node.Field_Number ("STOP_ID");
	node_fld = tcad_stop_node.Field_Number ("NODEID");

	while (tcad_stop_node.Read ()) {
		Show_Progress ();

		stop = tcad_stop_node.Get_Integer (stop_fld);
		node = tcad_stop_node.Get_Integer (node_fld);

		map_itr = node_map.find (node);

		if (map_itr == node_map.end ()) {
			if (node > 0) {
				Warning (String ("Stop %d Node %d was Not Found") % stop % node);
			}
			continue;
		}
		node = map_itr->second;

		for (route_itr = route_nodes_array.begin (); route_itr != route_nodes_array.end (); route_itr++) {
			for (node_itr = route_itr->nodes.begin (); node_itr != route_itr->nodes.end (); node_itr++) {
				if (node_itr->Node () == node) {
					node_itr->Type (STOP);
					break;
				}
			}
		}
	}
	End_Progress ();
}
