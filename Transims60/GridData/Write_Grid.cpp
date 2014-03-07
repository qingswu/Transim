//*********************************************************
//	Write_Grid.cpp - Write the New Arcview Grid File
//*********************************************************

#include "GridData.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Write_Grid
//---------------------------------------------------------

void GridData::Write_Grid ()
{
	int i, zone, count;
	double value, x0, y0;
	bool flag;

	Data_Itr data_itr;
	Db_Base_Itr db_itr;
	Field_Ptr fld_ptr;
	XYZ_Point p1, p2;
	Int_Itr fld_itr;
	Polygon_Itr poly_itr;
	Db_Sort_Array *data;
	Points_Map *polygon;
	Points_Map_Itr boundary_itr;

	count = 0;

	Show_Message (String ("Writing %s -- Record") % out_file.File_Type ());
	Set_Progress ();

	while (in_file.Read_Record ()) {
		Show_Progress ();

		out_file.Reset_Record ();
		out_file.Copy_Fields (in_file);

		for (db_itr = data_files.begin (); db_itr != data_files.end (); db_itr++) {
			if (!(*db_itr)->Read ()) {
				Error ("Grid Data Records Missing");
			}
		}
		if (script_flag) {

			//---- set the polygon record index ----

			if (polygon_flag) {
				p1 = in_file [0];
				p2 = in_file [2];

				x0 = (p1.x + p2.x) / 2.0;
				y0 = (p1.y + p2.y) / 2.0;

				for (poly_itr = polygons.begin (); poly_itr != polygons.end (); poly_itr++) {
					data = poly_itr->data_db;
					polygon = &poly_itr->polygon;

					zone = 0;

					for (boundary_itr = polygon->begin (); boundary_itr != polygon->end (); boundary_itr++) {
						if (In_Polygon (boundary_itr->second, x0, y0)) {
							zone = boundary_itr->first;
							break;
						}
					}
					if (zone == 0 || !data->Read_Record (zone)) {
						data->Reset_Record ();
					}
				}
			}

			//---- process each zone number ----

			if (zone_flag) {
				flag = false;

				for (fld_itr = in_zone_fld.begin (); fld_itr != in_zone_fld.end (); fld_itr++) {
					zone = in_file.Get_Integer (*fld_itr);
					if (zone == 0) continue;

					zone_grids.Read_Record (zone);

					for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
						data_itr->data_db->Read_Record (zone);
					}
					if (program.Execute ()) flag = true;
				}
				if (!flag) continue;
			} else {
				if (!program.Execute ()) continue;
			}
		} else {
			for (i=1; i < in_file.Num_Fields (); i++) {
				fld_ptr = in_file.Field (i);
				if (fld_ptr->Type () != DB_INTEGER && fld_ptr->Type () != DB_DOUBLE) continue;

				value = in_file.Get_Double (i);

				for (db_itr = data_files.begin (); db_itr != data_files.end (); db_itr++) {
					value += (*db_itr)->Get_Double (fld_ptr->Name ().c_str ());
				}
				out_file.Put_Field (i, value);
			}
		}

		//---- add point data ----

		if (point_flag) {
			p1 = in_file [0];
			p2 = in_file [2];

			x0 = (p1.x + p2.x) / 2.0;
			y0 = (p1.y + p2.y) / 2.0;

			Point_Grid (x0, y0, p1.x, p1.y, p2.x, p2.y);
		}

		//---- write the shape record ----

		out_file.swap (in_file);

		if (!out_file.Write_Record ()) {
			Error (String ("Writing %s") % out_file.File_Type ());
		}
		count++;

		//---- summarize the grid data ----

		if (summary_flag) {
			Sum_Grid ();
		}
	}
	End_Progress ();

	out_file.Close ();

	for (db_itr = data_files.begin (); db_itr != data_files.end (); db_itr++) {
		(*db_itr)->Close ();
	}
	Print (1);
	Write (1, "Number of Grids Written = ") << count;
}
