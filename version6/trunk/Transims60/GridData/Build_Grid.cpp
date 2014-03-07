//*********************************************************
//	Build_Grid.cpp - create grids
//*********************************************************

#include "GridData.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Build_Grid
//---------------------------------------------------------

void GridData::Build_Grid ()
{
	int i, j, id, zone, id_fld, num_zone, x_fld, y_fld, num_x, num_y;
	double x0, y0, x1, y1, x2, y2, xmin, ymin, xmax, ymax;
	String name;

	XYZ_Point p;
	Points pts;
	Points_Map_Itr boundary_itr;
	Zone_Itr zone_itr;

	id_fld = out_file.Field_Number ("ID");
	x_fld = out_file.Field_Number ("X_COORD");
	y_fld = out_file.Field_Number ("Y_COORD");

	boundary_file.Coordinate_Range (xmin, ymin, xmax, ymax);

	num_x = (int) ((xmax - xmin) / grid_size + 0.9);
	num_y = (int) ((ymax - ymin) / grid_size + 0.9);

	id = 0;

	Show_Message (String ("Building %s -- Record") % out_file.File_Type ());
	Set_Progress ();

	for (i=0, x1=xmin; i < num_x; i++, x1 = x2) {
		x2 = x1 + grid_size;

		for (j=0, y1=ymin; j < num_y; j++, y1 = y2) {
			y2 = y1 + grid_size;
			Show_Progress ();

			//---- scan for zone centroids in the grid ----

			zone = -1;
			num_zone = 0;
			out_file.Reset_Record ();

			for (zone_itr = zone_array.begin (); zone_itr != zone_array.end (); zone_itr++) {
				x0 = UnRound (zone_itr->X ());
				y0 = UnRound (zone_itr->Y ());

				if (x1 < x0 && x0 <= x2 && y1 < y0 && y0 <= y2) {
					if (num_zone == num_zone_fields) {
						Warning ("Insufficient Zone Fields for Centroid Allocation");
						break;
					}
					zone = zone_itr->Zone ();
					out_file.Put_Field (out_zone_fld [num_zone++], zone);						
				}
			}

			//---- grid centroid ----

			x0 = (x1 + x2) / 2;
			y0 = (y1 + y2) / 2;

			if (zone < 0) {
				for (boundary_itr = boundary.begin (); boundary_itr != boundary.end (); boundary_itr++) {
					if (In_Polygon (boundary_itr->second, x0, y0)) {
						zone = boundary_itr->first;
						break;
					}
				}
			}
			if (zone < 0) {

				//---- extent check ----

				pts.clear ();

				p.x = x1;
				p.y = y1;

				pts.push_back (p);

				p.x = x2;

				pts.push_back (p);

				p.y = y2;

				pts.push_back (p);

				p.x = x1;

				pts.push_back (p);

				p.y = y1;

				pts.push_back (p);

				for (boundary_itr = boundary.begin (); boundary_itr != boundary.end (); boundary_itr++) {
					if (In_Extents (boundary_itr->second, pts)) {
						zone = boundary_itr->first;
						break;
					}
				}
				if (zone < 0) continue;

				if (!In_Polygon (boundary_itr->second, x1, y1) &&
					!In_Polygon (boundary_itr->second, x2, y2) &&
					!In_Polygon (boundary_itr->second, x1, y2) &&
					!In_Polygon (boundary_itr->second, x2, y1)) {
					continue;
				}
				out_file.swap (pts);
			} else {
				out_file.clear ();

				p.x = x1;
				p.y = y1;

				out_file.push_back (p);

				p.x = x2;

				out_file.push_back (p);

				p.y = y2;

				out_file.push_back (p);

				p.x = x1;

				out_file.push_back (p);

				p.y = y1;

				out_file.push_back (p);
			}
			if (num_zone == 0) {
				out_file.Put_Field (out_zone_fld [0], zone);
			}

			//---- sum the point data ----

			if (point_flag) {
				Point_Grid (x0, y0, x1, y1, x2, y2);
			}

			//---- save the grid data ----

			out_file.Put_Field (id_fld, ++id);
			out_file.Put_Field (x_fld, x0);
			out_file.Put_Field (y_fld, y0);		

			if (!out_file.Write_Record ()) {
				Error (String ("Writing %s") % out_file.File_Type ());
			}

			//---- summarize grid data ----

			if (summary_flag) {
				Sum_Grid ();
			}
		}
	}
	End_Progress ();

	out_file.Close ();
	
	Print (1);
	Write (1, "Number of Grids Created = ") << id;
}

