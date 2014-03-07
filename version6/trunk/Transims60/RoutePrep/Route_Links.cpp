//*********************************************************
//	Route_Links.cpp - create links from route shapes
//*********************************************************

#include "RoutePrep.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Route_Links
//---------------------------------------------------------

void RoutePrep::Route_Links (Route_Link_Data *route_link_ptr, Stop_Offset_Map *stop_offset_ptr)
{
	int link, anode, bnode;
	double offset1, offset2, length;

	Points points;
	Points_Itr pt_itr;
	Stop_Offset_Map_Itr stop_offset_itr;
	Route_Stop_Map_Itr route_stop_itr;
	Int2_Map_Itr ab_itr;

	Int_Map_Itr map_itr;

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
			points.assign (route_shape_file.begin (), route_shape_file.end ());

			Sub_Shape (points, offset1, length);

			link = Insert_Link (points); 
			if (link < 0) continue;

			route_link_ptr->links.insert (Int2_Key (stop_offset_itr->first, link));
		}
		offset1 = offset2;
		anode = bnode;
	}
}
