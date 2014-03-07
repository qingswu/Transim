//*********************************************************
//	Draw_Link.cpp - draw the arcview link shape record
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Draw_Link
//---------------------------------------------------------

void ArcDelay::Draw_Link (Arcview_File &file, Link_Data *link_ptr, int dir)
{
	int lane, center, num_lanes, index;
	double length, side, start, width, offset;
	bool dir_flag, two_way;

	XYZ_Point point;
	Dir_Data *dir_ptr;

	if (link_ptr->Length () == 0) return;

	file.clear ();
	file.parts.clear ();

	two_way = (dir == 2);
	if (two_way) dir = 0;

	//---- bandwidth processing ----

	if (bandwidth_flag) {
		width = file.Get_Double (width_field);
		if (width < 0) width = -width;
		if (width < min_value || width == 0) return;

		width = width / width_factor;

		if (width < min_width) {
			width = min_width;
		} else if (width > max_width) {
			width = max_width;
		}
		width = -width;
		offset = length = -1;

		//---- get the centerline points ----

		side = (two_way) ? (-width / 2.0) : 0.0;

		Link_Shape (link_ptr, dir, points, offset, length, side);

		point = points [0];
				
		file.parts.push_back ((int) file.size ());
		file.assign (points.begin (), points.end ());

		//---- get the outside points of the band ----

		side = (two_way) ? (width / 2.0) : width;

		Link_Shape (link_ptr, 1 - dir, points, offset, length, side);

		if (max_angle > 0 && min_length > 0) {
			Smooth_Shape (points, max_angle, min_length);
		}

		file.insert (file.end (), points.begin (), points.end ());

		//---- close the polygon ----

		file.push_back (point);

	} else {

		//---- draw lanes ----

		if (lanes_flag) {
repeat1:				
			length = UnRound (link_ptr->Length () - link_ptr->Aoffset () - link_ptr->Boffset ());
			width = lane_width / 2.0;

			if (dir) {
				index = link_ptr->BA_Dir ();
				start = UnRound (link_ptr->Boffset ());
				dir_flag = (link_ptr->AB_Dir () >= 0);
			} else {
				index = link_ptr->AB_Dir ();
				start = UnRound (link_ptr->Aoffset ());
				dir_flag = (link_ptr->BA_Dir () >= 0);
			}

			if (index >= 0) {
				dir_ptr = &dir_array [index];
				num_lanes = dir_ptr->Lanes () + dir_ptr->Left ();

				if (center_flag && !dir_flag) {
					center = num_lanes + dir_ptr->Right () + 1;
				} else {
					center = 1;
				}
				for (lane = dir_ptr->Left (); lane < num_lanes; lane++) {
					side = (2 + 2 * lane - center) * width;

					Link_Shape (link_ptr, dir, points, start, length, side);

					file.parts.push_back ((int) file.size ());

					if (arrow_flag) Add_Arrow (points);

					file.insert (file.end (), points.begin (), points.end ());
				}
			}
			if (two_way && dir == 0) {
				dir = 1;
				goto repeat1;
			}

		} else {

			if (link_offset != 0.0) {
repeat2:
				//---- draw each direction ----

				side = link_offset;

				if (dir) {
					index = link_ptr->BA_Dir ();
					if (center_flag && link_ptr->AB_Dir () < 0) side = 0.0;
					start = -1.0;
				} else {
					index = link_ptr->AB_Dir ();
					if (center_flag && link_ptr->BA_Dir () < 0) side = 0.0;
					start = 0.0;
				}
				if (index >= 0) {
					Link_Shape (link_ptr, dir, points, start, -1.0, side);

					file.parts.push_back ((int) file.size ());

					if (arrow_flag) Add_Arrow (points);

					file.insert (file.end (), points.begin (), points.end ());
				}
				if (two_way && dir == 0) {
					dir = 1;
					goto repeat2;
				}

			} else {
					
				//---- draw centerline -----

				file.parts.push_back ((int) file.size ());

				if (direction_flag) dir = 0;

				Link_Shape (link_ptr, dir, file);

				if (arrow_flag) Add_Arrow (file);
			}
		}
	}

	//---- write the shape record ----

	if (file.size () == 0) return;

	if (!file.Write_Record ()) {
		Error (String ("Writing %s") % file.File_Type ());
	}
}
