//*********************************************************
//	Write_Group - draw stop group boardings and alightings
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Group
//---------------------------------------------------------

void ArcRider::Write_Group (void)
{
	int i, num, index, board, alight;
	int group_field, board_field, alight_field, total_field, name_field;
	double x, y, z;

	Point_Map_Itr pt_itr;
	XYZ_Point point;
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;

	group_field = arcview_group.Field_Number ("GROUP");
	name_field = arcview_group.Field_Number ("NAME");
	board_field = arcview_group.Field_Number ("BOARD");
	alight_field = arcview_group.Field_Number ("ALIGHT");
	total_field = arcview_group.Field_Number ("TOTAL");

	arcview_group.clear ();
	arcview_group.push_back (point);

	Show_Message (String ("Writing %s -- Record") % arcview_group.File_Type ());
	Set_Progress ();

	//---- process each stop group ----

	for (i = stop_equiv.First_Group (); i > 0; i = stop_equiv.Next_Group ()) {

		group = stop_equiv.Group_List (i);
		if (group == NULL) continue;

		Show_Progress ();

		board = alight = num = 0;
		x = y = z = 0.0;

		//---- sum the stops in the group ----

		for (itr = group->begin (); itr != group->end (); itr++) {

			pt_itr = stop_pt.find (*itr);
			if (pt_itr == stop_pt.end ()) continue;

			x += pt_itr->second.x;
			y += pt_itr->second.y;
			z += pt_itr->second.z;

			map_itr = stop_map.find (*itr);
			if (map_itr == stop_map.end ()) continue;

			index = map_itr->second;

			board += board_data [index];
			alight += alight_data [index];
			num++;
		}
		if (board <= 0 && alight <= 0) continue;

		//---- write the stop record ----

		point.x = x / num;
		point.y = y / num;
		point.z = z / num;

		arcview_group.Put_Field (group_field, i);
		arcview_group.Put_Field (name_field, stop_equiv.Group_Label (i));
		arcview_group.Put_Field (board_field, board);
		arcview_group.Put_Field (alight_field, alight);
		arcview_group.Put_Field (total_field, (board + alight));

		arcview_group [0] = point;

		if (!arcview_group.Write_Record ()) {
			Error ("Writing ArcView Stop Group File");
		}
		num_group++;
	}
	End_Progress ();

	arcview_group.Close ();
}
