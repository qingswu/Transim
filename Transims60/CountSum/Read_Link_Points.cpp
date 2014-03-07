//*********************************************************
//	Read_link_Points.cpp - read link points
//*********************************************************

#include "CountSum.hpp"
#include "Compass.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Link_Points
//---------------------------------------------------------

void CountSum::Read_Link_Points (void)
{
	int id, x, y, x1, y1, x2, y2, x0, y0, index, best_index, dir_index, buffer, num, bearing;
	bool select_flag, flag, shape_flag;
	double best_dist, max_dist2, dist, slope1, slope2, dx, dy;
	char dir_char;
	String direction, type;
	XYZ_Point *pts;
	Str_Itr str_itr;
	Strs_Itr strs_itr;
	Ints_Itr ints_itr;
	Int_Map_Itr map_itr;
	Extents_Itr extent_itr;
	Extents extent_rec;
	Shape_Data *shape_ptr;

	XYZ pt;
	XYZ_Array pt_array;
	XYZ_Itr pt_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Dir_Data *dir_ptr;
	Int2_Key dir_offset;

	Compass_Points compass (4);

	//---- read link points----

	Show_Message (String ("Reading %s -- Record") % arc_stations.File_Type ());
	Set_Progress ();

	select_flag = (station_types.size () > 0);
	max_dist2 = Scale (max_dist) * Scale (max_dist);
	buffer = Round (max_dist);
	shape_flag = System_File_Flag (SHAPE);

	while (arc_stations.Read_Record ()) {
		Show_Progress ();

		//---- get the link number ----

		id = arc_stations.Get_Integer (station_id_field);
		if (id == 0) continue;

		if (id_offset_map.find (id) != id_offset_map.end ()) continue;

		direction = arc_stations.Get_String (station_dir_field).Trim ();
		bearing = -1;

		if (!direction.empty ()) {
			direction.To_Upper ();
			dir_char = direction [0];

			if (dir_char == 'N') {
				bearing = 0;
			} else if (dir_char == 'E') {
				bearing = 1;
			} else if (dir_char == 'S') {
				bearing = 2;
			} else if (dir_char == 'W') {
				bearing = 3;
			}
		}
		type = arc_stations.Get_String (station_type_field).Trim ();

		num = 0;

		if (select_flag) {
			flag = false;
			for (num=0, strs_itr = station_types.begin (); strs_itr != station_types.end (); strs_itr++, num++) {
				for (str_itr = strs_itr->begin (); str_itr != strs_itr->end (); str_itr++) {
					if (str_itr->Equals (type)) {
						flag = true;
						goto exit;
					}
				}
			}
exit:
			if (!flag) continue;
		}

		//---- find the closest node ----

		pts = arc_stations.Get_Points ();

		x = Round (pts->x);
		y = Round (pts->y);

		best_index = dir_index = -1;	
		best_dist = max_dist2;
		buffer = Round (max_dist);
		dir_offset.second = DTOI (arc_stations.Get_Double (station_offset_field) * 100.0);

		for (index=0, extent_itr = extents_array.begin (); extent_itr != extents_array.end (); extent_itr++, index++) {

			if (x < extent_itr->low_x || x > extent_itr->high_x ||
				y < extent_itr->low_y || y > extent_itr->high_y) continue;

			link_ptr = &link_array [index];

			if (facility_types.size () > 0) {
				if (num > 0) {
					if (facility_types [num].size () > 0) {
						if (facility_types [num] [link_ptr->Type ()] == 0) continue;
					}
				} else {
					flag = true;
					for (ints_itr = facility_types.begin (); ints_itr != facility_types.end (); ints_itr++) {
						if (ints_itr->size () > 0) {
							if (ints_itr->at (link_ptr->Type ()) == 0) {
								flag = false;
								break;
							}
						}
					}
					if (!flag) continue;
				}
			}

			node_ptr = &node_array [link_ptr->Anode ()];

			pt_array.clear ();

			x1 = node_ptr->X ();
			y1 = node_ptr->Y ();

			if (shape_flag && link_ptr->Shape () >= 0) {
				shape_ptr = &shape_array [link_ptr->Shape ()];

				for (pt_itr = shape_ptr->begin (); pt_itr != shape_ptr->end (); pt_itr++) {
					pt_array.push_back (*pt_itr);
				}
			}
			node_ptr = &node_array [link_ptr->Bnode ()];

			pt.x = node_ptr->X ();
			pt.y = node_ptr->Y ();

			pt_array.push_back (pt);

			for (pt_itr = pt_array.begin (); pt_itr != pt_array.end (); pt_itr++, x1 = x2, y1 = y2) {
				x2 = pt_itr->x;
				y2 = pt_itr->y;
				
				if (x1 == x2 && y1 == y2) continue;

				extent_rec.low_x = extent_rec.high_x = x1;
				extent_rec.low_y = extent_rec.high_y = y1;

				if (x2 < extent_rec.low_x) extent_rec.low_x = x2;
				if (y2 < extent_rec.low_y) extent_rec.low_y = y2;
				if (x2 > extent_rec.high_x) extent_rec.high_x = x2;
				if (y2 > extent_rec.high_y) extent_rec.high_y = y2;

				extent_rec.low_x -= buffer;
				extent_rec.low_y -= buffer;
				extent_rec.high_x += buffer;
				extent_rec.high_y += buffer;

				if (x < extent_rec.low_x || x > extent_rec.high_x) continue;
				if (y < extent_rec.low_y || y > extent_rec.high_y) continue;

				dx = (x2 - x1);
				dy = (y2 - y1);

				if (dx != 0 && dy != 0) {
					slope1 = (double) dy / dx;
					slope2 = (double) dx / -dy;

					if (slope1 != slope2) {
						x0 = DTOI ((y - y1 + x1 * slope1 - x * slope2) / (slope1 - slope2));
						y0 = DTOI (y1 + (x0 - x1) * slope1);
					} else {
						x0 = y0 = 0;
					}
				} else if (dx == 0) {
					x0 = x1;
					y0 = y;
				} else {
					x0 = x;
					y0 = y1;
				}
				dx = (x - x0);
				dy = (y - y0);

				dist = dx * dx + dy * dy;

				if (dist < best_dist) {
					if (bearing >= 0) {
						dir_index = -1;

						if (link_ptr->AB_Dir () >= 0) {
							dir_ptr = &dir_array [link_ptr->AB_Dir ()];
							if (compass.Bearing_Point (dir_ptr->In_Bearing ()) == bearing ||
								compass.Bearing_Point (dir_ptr->Out_Bearing ()) == bearing) {
									dir_index = link_ptr->AB_Dir ();
							}
						}
						if (dir_index < 0 && link_ptr->BA_Dir () >= 0) {
							dir_ptr = &dir_array [link_ptr->BA_Dir ()];

							if (compass.Bearing_Point (dir_ptr->In_Bearing ()) == bearing ||
								compass.Bearing_Point (dir_ptr->Out_Bearing ()) == bearing) {
									dir_index = link_ptr->BA_Dir ();
							}
						}
						if (dir_index < 0) {
							if (bearing == 0) {
								if (y2 >= y1) {
									dir_index = link_ptr->AB_Dir ();
								} else {
									dir_index = link_ptr->BA_Dir ();
								}
							} else if (bearing == 1) {
								if (x2 >= x1) {
									dir_index = link_ptr->AB_Dir ();
								} else {
									dir_index = link_ptr->BA_Dir ();
								}
							} else if (bearing == 2) {
								if (y1 >= y2) {
									dir_index = link_ptr->AB_Dir ();
								} else {
									dir_index = link_ptr->BA_Dir ();
								}
							} else if (bearing == 3) {
								if (x1 >= x2) {
									dir_index = link_ptr->AB_Dir ();
								} else {
									dir_index = link_ptr->BA_Dir ();
								}
							}
						}
						if (dir_index < 0) continue;
					} else {
						dir_index = link_ptr->AB_Dir ();
						if (dir_index < 0) dir_index = link_ptr->BA_Dir ();
					}
					best_dist = dist;
					best_index = index;

					if (dist == 0) break;
					buffer = DTOI (sqrt (best_dist));
				}
			}
		}
		if (best_index >= 0 && dir_index >= 0) {
			dir_offset.first = dir_index;
			id_offset_map.insert (ID_Offset_Data (id, dir_offset));

			offset_index_map.insert (Offset_Index_Data (dir_offset, (int) offset_index_map.size ()));
		} else {
			Warning (String ("ID %d was Not Matched to a Link (x=%.0lf, y=%.0lf)") % id % pts->x % pts->y);
		}
	}
	End_Progress ();

	arc_stations.Close ();
	Print (2, "Number of ") << arc_stations.File_Type () << " Records = " << Progress_Count ();

	if (Progress_Count () != (int) id_offset_map.size ()) {
		Write (1, "Number of Located Station Records = ") << id_offset_map.size ();
		Show_Message (1);
	}
}
