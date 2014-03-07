//*********************************************************
//	Data_Shapes.cpp - system data shape services
//*********************************************************

#include "Data_Service.hpp"
#include "Shape_Tools.hpp"

#include <math.h>

//---------------------------------------------------------
//	Link_Shape
//---------------------------------------------------------

bool Data_Service::Link_Shape (Link_Data *link_ptr, int dir, Points &points, double offset, double length, double side)
{
	int i, j, num_pts, node;
	double xa, ya, za, xb, yb, zb, factor, link_len, x1, y1, z1, x2, y2, z2;
	double dx, dy, dz, dx1, dy1, dz1, dx2, dy2, dz2, end_offset, min_len;
	bool point_flag, flip_flag, flag;

	XYZ *pt_ptr;
	XYZ_Point point;
	Points_Itr point_itr, output_itr;
	Node_Data *node_ptr;
	Shape_Data *shape_ptr;

	point_flag = (length == 0.0);
	flip_flag = (dir == 1);
	x1 = y1 = z1 = x2 = y2 = z2 = dx2 = dy2 = dz2 = 0.0;
	min_len = Internal_Units (2.0, FEET);

	//---- get the link length and node coordinates ----

	link_len = UnRound (link_ptr->Length ());
	factor = 1.0;

	//---- check the offsets ----

	if (offset < 0.0) offset = 0.0;
	if (length < 0.0) length = link_len;

	end_offset = offset + length;

	if (offset < 0.0) {
		offset = 0.0;
	} else if (offset > link_len) {
		offset = link_len;
	}
	if (end_offset < 0.0) {
		end_offset = 0.0;
	} else if (end_offset > link_len) {
		end_offset = link_len;
	}

	//---- get the end nodes ----

	node = (flip_flag) ? link_ptr->Bnode () : link_ptr->Anode ();

	node_ptr = &node_array [node];

	xa = UnRound (node_ptr->X ());
	ya = UnRound (node_ptr->Y ());
	za = UnRound (node_ptr->Z ());

	node = (flip_flag) ? link_ptr->Anode () : link_ptr->Bnode ();

	node_ptr = &node_array [node];

	xb = UnRound (node_ptr->X ());
	yb = UnRound (node_ptr->Y ());
	zb = UnRound (node_ptr->Z ());

	dx = xb - xa;
	dy = yb - ya;
	dz = zb - za;

	length = sqrt (dx * dx + dy * dy + dz * dz);
	if (length == 0.0) length = 0.01;

	dx /= length;
	dy /= length;
	dz /= length;

	//---- get the shape record ----

	if (System_File_Flag (SHAPE) && link_ptr->Shape () >= 0) {
		shape_ptr = &shape_array [link_ptr->Shape ()];
	} else {
		shape_ptr = 0;
	}
	points.clear ();

	//---- process offsets for a simple link ----

	if (shape_ptr == 0) {
		i = (point_flag) ? 1 : 2;
		points.reserve (i);

		if (link_len > 0.0) {
			factor = length / link_len;
		}
		offset *= factor;

		point.x = xa + offset * dx + side * dy;
		point.y = ya + offset * dy - side * dx;
		point.z = za + offset * dz;

		points.push_back (point);

		if (point_flag) return (true);

		end_offset *= factor;

		point.x = xa + end_offset * dx + side * dy;
		point.y = ya + end_offset * dy - side * dx;
		point.z = za + end_offset * dz;

		points.push_back (point);

		return (true);
	}

	//---- create the link vector ----

	num_pts = (int) shape_ptr->size () + 2;

	points.reserve (num_pts);

	j = (flip_flag) ? (int) shape_ptr->size () : 1;

	if (side == 0.0) {

		for (i=1; i <= num_pts; i++) {
			if (i == 1) {
				point.x = xa;
				point.y = ya;
				point.z = za;
			} else if (i == num_pts) {
				point.x = xb;
				point.y = yb;
				point.z = zb;
			} else {
				pt_ptr = &(shape_ptr->at (j-1));

				point.x = UnRound (pt_ptr->x);
				point.y = UnRound (pt_ptr->y);
				point.z = UnRound (pt_ptr->z);

				j += (flip_flag) ? -1 : 1;
			}
			points.push_back (point);
		}

	} else {

		dx1 = dy1 = dz1 = 0.0;
		flag = false;

		for (i=1; i <= num_pts; i++) {
			if (i == 1) {
				x2 = xa;
				y2 = ya;
				z2 = za;
				dx2 = dy2 = dz2 = 0.0;
			} else {
				if (i == num_pts) {
					x2 = xb;
					y2 = yb;
					z2 = zb;
				} else {
					pt_ptr = &(shape_ptr->at (j-1));

					x2 = UnRound (pt_ptr->x);
					y2 = UnRound (pt_ptr->y);
					z2 = UnRound (pt_ptr->z);

					j += (flip_flag) ? -1 : 1;
				}
				dx2 = x2 - x1;
				dy2 = y2 - y1;
				dz2 = z2 - z1;

				length = sqrt (dx2 * dx2 + dy2 * dy2 + dz2 * dz2);

				if (length < min_len) {
					if (i == 2) {
						dx1 = dx;
						dy1 = dy;
						dz1 = dz;
						flag = true;
					} else if (i == num_pts) {
						point.x = x2 + side * dy1;
						point.y = y2 - side * dx1;
						point.z = z2;

						points.push_back (point);
					}
					continue;
				} else {
					dx2 /= length;
					dy2 /= length;
					dz2 /= length;
				}
				if (i == 2) {
					dx1 = dx2;
					dy1 = dy2;
					dz1 = dz2;
				} else if (flag) {
					dx1 = dx2;
					dy1 = dy2;
					dz1 = dz2;
				} else {
					dx1 = (dx1 + dx2) / 2.0;
					dy1 = (dy1 + dy2) / 2.0;
					dz1 = (dz1 + dz2) / 2.0;
				}
				point.x = x1 + side * dy1;
				point.y = y1 - side * dx1;
				point.z = z1;

				points.push_back (point);

				if (i == num_pts) {
					point.x = x2 + side * dy2;
					point.y = y2 - side * dx2;
					point.z = z2;

					points.push_back (point);
				}
			}
			x1 = x2;
			y1 = y2;
			z1 = z2;
			dx1 = dx2;
			dy1 = dy2;
			dz1 = dz2;
		}
	}

	//---- extract a subset of points ----

	if (offset != 0.0 || end_offset != link_len) {
		return (Sub_Shape (points, offset, end_offset - offset, link_len));
	} else {
		return (true);
	}
}

//---------------------------------------------------------
//	Turn_Shape
//---------------------------------------------------------

bool Data_Service::Turn_Shape (int dir_in, int dir_out, Points &points, double setback, bool curve_flag, double side_in, double side_out, double sub_off, double sub_len)
{
	int dir;
	double length, offset, max_len, off1, off2;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Points pts;
	Points_Itr pt_itr;

	points.clear ();
	max_len = 2.0 * setback;

	//---- approach leg ----

	dir_ptr = &dir_array [dir_in];
	dir = dir_ptr->Dir ();

	link_ptr = &link_array [dir_ptr->Link ()];

	if (dir == 0) {
		off1 = UnRound (link_ptr->Boffset ());
		off2 = UnRound (link_ptr->Aoffset ());
	} else {
		off1 = UnRound (link_ptr->Aoffset ());
		off2 = UnRound (link_ptr->Boffset ());
	}
	max_len += off1;

	length = UnRound (link_ptr->Length ()) - off1;
	offset = length - setback;
	if (offset < off2) offset = off2;
	length -= offset;
	if (length < 0.0) length = 0.0;

	Link_Shape (link_ptr, dir, pts, offset, length, side_in);

	for (pt_itr = pts.begin (); pt_itr != pts.end (); pt_itr++) {
		points.push_back (*pt_itr);
	}

	//---- departure leg ----

	dir_ptr = &dir_array [dir_out];
	dir = dir_ptr->Dir ();

	link_ptr = &link_array [dir_ptr->Link ()];

	length = UnRound (link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ());
	if (length < 0.0) length = 0.0;

	if (dir == 0) {
		off1 = UnRound (link_ptr->Aoffset ());
	} else {
		off1 = UnRound (link_ptr->Boffset ());
	}
	max_len += off1;

	offset = off1;
	if (length > setback) length = setback;

	Link_Shape (link_ptr, dir, pts, offset, length, side_out);

	//---- curve connection ----

	if (curve_flag) {
		Connection_Curve (points, pts, turn_shape_setback);
	} else {
		for (pt_itr = pts.begin (); pt_itr != pts.end (); pt_itr++) {
			points.push_back (*pt_itr);
		}
	}

	//---- extract a subset of points ----

	if (sub_off < 0.0) sub_off = 0.0;
	if (sub_len < 0.0) sub_len = 0.0;

	if (sub_off != 0.0 || sub_len != 0.0) {
		return (Sub_Shape (points, sub_off, sub_len, max_len));
	} else {
		return (true);
	}
}

//---------------------------------------------------------
//	Link_Bearings
//---------------------------------------------------------

void Data_Service::Link_Bearings (Link_Data &link_rec, int &bearing_in, int &bearing_out)
{
	int xa, ya, xb, yb;
	double dx, dy;

	Node_Data *node_ptr;

	node_ptr = &node_array [link_rec.Anode ()];

	xa = node_ptr->X ();
	ya = node_ptr->Y ();

	node_ptr = &node_array [link_rec.Bnode ()];

	xb = node_ptr->X ();
	yb = node_ptr->Y ();

	dx = dy = 0;

	if (link_rec.Shape () >= 0 && 
		(Bearing_Offset () + MAX (link_rec.Aoffset (), link_rec.Boffset ())) < link_rec.Length ()) {

		int i, num, offset, len1, len2, xy_len, x1, y1;

		XYZ *point;
		Shape_Data *shape_ptr;

		shape_ptr = &shape_array [link_rec.Shape ()];
		num = (int) shape_ptr->size ();

		//---- from the A node ---

		offset = Bearing_Offset () + link_rec.Aoffset ();

		len1 = xy_len = 0;
		x1 = xa;
		y1 = ya;

		for (i=0; i < num; i++) {
			point = &((*shape_ptr) [i]);

			dx = point->x - x1;
			dy = point->y - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;

			if (len2 > offset) break;

			len1 = len2;
			x1 = point->x;
			y1 = point->y;
		}
		if (i == num) {
			dx = xb - x1;
			dy = yb - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;
		}
		offset -= len1;
		if (xy_len == 0) xy_len = 1;

		dx = x1 + (dx * offset / xy_len) - xa;
		dy = y1 + (dy * offset / xy_len) - ya;

		bearing_in = compass.Direction (dx, dy);
		
		//---- from the B node ----

		offset = Bearing_Offset () + link_rec.Boffset ();

		len1 = xy_len = 0;
		x1 = xb;
		y1 = yb;

		for (i=num-1; i >= 0; i--) {
			point = &((*shape_ptr) [i]);

			dx = point->x - x1;
			dy = point->y - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;

			if (len2 > offset) break;

			len1 = len2;
			x1 = point->x;
			y1 = point->y;
		}
		if (i < 0) {
			dx = xa - x1;
			dy = ya - y1;

			xy_len = (int) (sqrt (dx * dx + dy * dy) + 0.5);
			len2 = len1 + xy_len;
		}
		offset -= len1;
		if (xy_len == 0) xy_len = 1;

		dx = x1 + (dx * offset / xy_len) - xb;
		dy = y1 + (dy * offset / xy_len) - yb;

		bearing_out = compass.Direction (-dx, -dy);

	} else {
		dx = xb - xa;
		dy = yb - ya;

		bearing_in = bearing_out = compass.Direction (dx, dy);
	}
}
