//*********************************************************
//	Get_Shape_Data.cpp - read the shape point file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Shape_Data
//---------------------------------------------------------

bool Relocate::Get_Shape_Data (Shape_File &file, Shape_Data &shape_rec)
{
	static int num_points;

	if (!Data_Service::Get_Shape_Data (file, shape_rec)) return (false);
	if (!target_flag) return (true);

	if (!file.Nested ()) {
		num_points = file.Points ();
	} else if ((int) shape_rec.size () == num_points) {
		Int_Map_Itr map_itr = shape_map.find (shape_rec.Link ());

		if (map_itr != shape_map.end ()) {
			Shape_Data *shape_ptr = &shape_array [map_itr->second];
			if (num_points == (int) shape_ptr->size ()) {
				int i;

				for (i=0; i < num_points; i++) {
					XYZ target = shape_rec [i];
					XYZ base = shape_ptr->at (i);

					if (abs (target.x - base.x) > max_xy_diff ||
						abs (target.y - base.y) > max_xy_diff) {
						break;
					}
				}
				if (i == num_points) return (false);
			}
			target_shape_map.insert (Int_Map_Data (shape_rec.Link (), (int) shape_array.size ()));
		} else {
			shape_map.insert (Int_Map_Data (shape_rec.Link (), (int) shape_array.size ()));
		}
		shape_array.push_back (shape_rec);
	}
	return (false);
}
