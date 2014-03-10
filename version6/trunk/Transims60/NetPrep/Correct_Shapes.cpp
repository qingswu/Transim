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
	int i, num;
	double dx, dy, dz, first_a, first_b, last_a, last_b;
	double length, x1, y1, x2, y2, z1, z2, ratio;

	Link_Itr link_itr;
	Node_Data *node_ptr;
	Shape_Data *shape_ptr;

	Points points;
	Points_Itr pts_itr;
	XYZ_Point xyz;
	XYZ_Itr xyz_itr;
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

		//---- link/shape checks ----

		if (length_flag || max_angle > 0 || min_length > 0) {

			points.clear ();
			xyz.x = UnRound (first.x);
			xyz.y = UnRound (first.y);
			xyz.z = UnRound (first.z);

			points.push_back (xyz);

			for (xyz_itr = shape_ptr->begin (); xyz_itr != shape_ptr->end (); xyz_itr++) {
				xyz.x = UnRound (xyz_itr->x);
				xyz.y = UnRound (xyz_itr->y);
				xyz.z = UnRound (xyz_itr->z);

				points.push_back (xyz);
			}
			xyz.x = UnRound (last.x);
			xyz.y = UnRound (last.y);
			xyz.z = UnRound (last.z);

			points.push_back (xyz);

			//---- smooth the shape points ----

			if (max_angle > 0 || min_length > 0) {
				if (Smooth_Shape (points, max_angle, min_length)) {
					shape_ptr->clear ();
					num = (int) points.size () - 1;

					for (i=0, pts_itr = points.begin (); pts_itr != points.end (); pts_itr++, i++) {
						if (i == 0 || i == num) continue;

						first.x = Round (pts_itr->x);
						first.y = Round (pts_itr->y);
						first.z = Round (pts_itr->z);

						shape_ptr->push_back (first);
					}
				}
			}

			//---- process the point data ----

			if (length_flag) {
				length = 0;
				x1 = y1 = z1 = 0.0;

				for (i=0, pts_itr = points.begin (); pts_itr != points.end (); pts_itr++, i++) {

					x2 = pts_itr->x;
					y2 = pts_itr->y;
					z2 = pts_itr->z;

					if (i > 1) {
						dx = x2 - x1;
						dy = y2 - y1;
						dz = z2 - z1;

						length += sqrt (dx * dx + dy * dy + dz * dz);
					}
					x1 = x2; 
					y1 = y2;
					z1 = z2;
				}
				if (link_itr->Length () > 0) {
					ratio = length / UnRound (link_itr->Length ());
					if (ratio < 1.0 || ratio > length_ratio) {
						link_itr->Length (Round (length));
						num_ratio++;
					}
				} else {
					link_itr->Length (Round (length));
				}
			}
		}
	}
}
