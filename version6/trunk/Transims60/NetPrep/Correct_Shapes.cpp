//*********************************************************
//	Correct_Shapes.cpp - correct link shape files
//*********************************************************

#include "NetPrep.hpp"

#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Correct_Shapes
//---------------------------------------------------------

void NetPrep::Correct_Shapes (void)
{
	double dx, dy, first_a, first_b, last_a, last_b;

	Link_Itr link_itr;
	Node_Data *node_ptr;
	Shape_Data *shape_ptr;

	XYZ first, last;
	XYZ_RItr xyz_ritr;
	XYZ_Array xyz_array;

	Show_Message ("Correcting Link Shapes -- Record");
	Set_Progress ();

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		//---- check for a shape record ----

		if (link_itr->Shape () < 0) continue;

		//---- get the shape record ----

		shape_ptr = &shape_array [link_itr->Shape ()];

		first = shape_ptr->front ();
		last = shape_ptr->back ();

		//---- get distance to A node ----

		node_ptr = &node_array [link_itr->Anode ()];

		dx = first.x - node_ptr->X ();
		dy = first.y - node_ptr->Y ();

		first_a = sqrt (dx * dx + dy * dy);

		dx = last.x - node_ptr->X ();
		dy = last.y - node_ptr->Y ();

		last_a = sqrt (dx * dx + dy * dy);

		//---- get distance to B node ----

		node_ptr = &node_array [link_itr->Bnode ()];

		dx = first.x - node_ptr->X ();
		dy = first.y - node_ptr->Y ();

		first_b = sqrt (dx * dx + dy * dy);

		dx = last.x - node_ptr->X ();
		dy = last.y - node_ptr->Y ();

		last_b = sqrt (dx * dx + dy * dy);

		if ((last_a < first_a && first_b < last_b) ||
			(last_a <= first_a && first_b < last_b) ||
			(last_a < first_a && first_b <= last_b)) {

			xyz_array.clear ();

			for (xyz_ritr = shape_ptr->rbegin (); xyz_ritr != shape_ptr->rend (); xyz_ritr++) {
				xyz_array.push_back (*xyz_ritr);
			}
			shape_ptr->swap (xyz_array);
		}
	}
}
