//*********************************************************
//	Write_Bandwidth - draw the link bandwidth polygons
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Bandwidth
//---------------------------------------------------------

void ArcPlan::Write_Bandwidth (void)
{
	int dir_index, dir, link_field, dir_field, data_field;
	double width, offset, length;

	XYZ_Point point;
	Dbl_Itr data_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	link_field = arcview_width.Field_Number ("LINK");
	dir_field = arcview_width.Field_Number ("DIR");
	data_field = arcview_width.Field_Number (((width_flow_flag) ? "FLOW" : "VOLUME"));

	Show_Message (String ("Writing %s -- Record") % arcview_width.File_Type ());
	Set_Progress ();

	//---- process each link direction ----
	
	for (dir_index = 0, data_itr = width_data.begin (); data_itr != width_data.end (); data_itr++, dir_index++) {
		Show_Progress ();

		if (*data_itr == 0 || *data_itr < min_value) continue;

		//---- write the data fields ----

		dir_ptr = &dir_array [dir_index];
		link_ptr = &link_array [dir_ptr->Link ()];

		arcview_width.Put_Field (link_field, link_ptr->Link ());
		arcview_width.Put_Field (dir_field, dir_ptr->Dir ());
		arcview_width.Put_Field (data_field, *data_itr);

		dir = dir_ptr->Dir ();

		width = *data_itr / width_factor;

		if (width < min_width) {
			width = min_width;
		} else if (width > max_width) {
			width = max_width;
		}
		width = -width;
		offset = length = -1;

		//---- get the centerline points ----

		arcview_width.clear ();

		Link_Shape (link_ptr, dir, points, offset, length, 0.0);

		point = points [0];

		arcview_width.assign (points.begin (), points.end ());

		//---- get the outside points of the band ----

		dir = 1 - dir;

		Link_Shape (link_ptr, dir, points, offset, length, width);

		if (max_angle > 0 && min_length > 0) {
			Smooth_Shape (points, max_angle, min_length);
		}

		arcview_width.insert (arcview_width.end (), points.begin (), points.end ());

		//---- close the polygon ----

		arcview_width.push_back (point);

		//---- write the shape record ----

		if (!arcview_width.Write_Record ()) {
			Error (String ("Writing %s") % arcview_width.File_Type ());
		}
		num_width++;
	}
	End_Progress ();

	arcview_width.Close ();
}
