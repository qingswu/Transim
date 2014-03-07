//*********************************************************
//	Link_Extents.cpp - build the link coordinate extents
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Link_Extents
//---------------------------------------------------------

void CountSum::Link_Extents (void)
{
	int index, buffer;
	bool shape_flag;

	Link_Itr link_itr;
	Extents extent_rec, *extent_ptr;
	Node_Data *node_ptr;
	Shape_Data *shape_ptr;
	XYZ_Itr xyz_itr;

	Show_Message ("Building Link Extents -- Record");
	Set_Progress ();
	
	shape_flag = System_File_Flag (SHAPE);
	buffer = Round (max_dist);

	extent_rec.low_x = extent_rec.low_y = MAX_INTEGER;
	extent_rec.high_x = extent_rec.high_y = -MAX_INTEGER;

	extents_array.assign (link_array.size (), extent_rec);
	
	for (index=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, index++) {
		Show_Progress ();

		extent_ptr = &extents_array [index];

		node_ptr = &node_array [link_itr->Anode ()];

		if (node_ptr->X () < extent_ptr->low_x) extent_ptr->low_x = node_ptr->X ();
		if (node_ptr->Y () < extent_ptr->low_y) extent_ptr->low_y = node_ptr->Y ();
		if (node_ptr->X () > extent_ptr->high_x) extent_ptr->high_x = node_ptr->X ();
		if (node_ptr->Y () > extent_ptr->high_y) extent_ptr->high_y = node_ptr->Y ();

		node_ptr = &node_array [link_itr->Bnode ()];
		
		if (node_ptr->X () < extent_ptr->low_x) extent_ptr->low_x = node_ptr->X ();
		if (node_ptr->Y () < extent_ptr->low_y) extent_ptr->low_y = node_ptr->Y ();
		if (node_ptr->X () > extent_ptr->high_x) extent_ptr->high_x = node_ptr->X ();
		if (node_ptr->Y () > extent_ptr->high_y) extent_ptr->high_y = node_ptr->Y ();

		if (shape_flag && link_itr->Shape () >= 0) {
			shape_ptr = &shape_array [link_itr->Shape ()];

			for (xyz_itr = shape_ptr->begin (); xyz_itr != shape_ptr->end (); xyz_itr++) {
				if (xyz_itr->x < extent_ptr->low_x) extent_ptr->low_x = xyz_itr->x;
				if (xyz_itr->y < extent_ptr->low_y) extent_ptr->low_y = xyz_itr->y;
				if (xyz_itr->x > extent_ptr->high_x) extent_ptr->high_x = xyz_itr->x;
				if (xyz_itr->y > extent_ptr->high_y) extent_ptr->high_y = xyz_itr->y;
			}
		}

		//---- add a search buffer ----

		extent_ptr->low_x -= buffer;
		extent_ptr->low_y -= buffer;
		extent_ptr->high_x += buffer;
		extent_ptr->high_y += buffer;
	}
	End_Progress ();
}
