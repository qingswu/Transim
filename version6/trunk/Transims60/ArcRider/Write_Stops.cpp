//*********************************************************
//	Write_Stops - draw transit boardings and alightings
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Stops
//---------------------------------------------------------

void ArcRider::Write_Stops (void)
{
	int stop, index, board, alight, stop_field, board_field, alight_field, total_field, notes_field;
	String notes;

	Int_Map_Itr map_itr;
	Point_Map_Itr pt_itr;
	XYZ_Point point;
	Stop_Data *stop_ptr;

	stop_field = arcview_stop.Field_Number ("STOP");
	board_field = arcview_stop.Field_Number ("BOARD");
	alight_field = arcview_stop.Field_Number ("ALIGHT");
	total_field = arcview_stop.Field_Number ("TOTAL");
	notes_field = arcview_stop.Field_Number ("NOTES");

	arcview_stop.Num_Points (1);

	Show_Message (String ("Writing %s -- Record") % arcview_stop.File_Type ());
	Set_Progress ();

	//---- process each stop ----

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		Show_Progress ();

		stop = map_itr->first;

		if (select_stops && !stop_range.In_Range (stop)) continue;

		index = map_itr->second;

		board = board_data [index];
		alight = alight_data [index];

		if (board <= 0 && alight <= 0) continue;
		
		pt_itr = stop_pt.find (stop);
		if (pt_itr == stop_pt.end ()) continue;
		
		//---- write the stop record ----

		point.x = pt_itr->second.x;
		point.y = pt_itr->second.y;
		point.z = pt_itr->second.y;

		arcview_stop.Put_Field (stop_field, stop);
		arcview_stop.Put_Field (board_field, board);
		arcview_stop.Put_Field (alight_field, alight);
		arcview_stop.Put_Field (total_field, (board + alight));

		if (notes_field >= 0) {
			stop_ptr = &stop_array [index];

			notes.clear ();

			if (!stop_ptr->Name ().empty ()) {
				notes = stop_ptr->Name ();
			}
			if (!stop_ptr->Notes ().empty ()) {
				if (!notes.empty ()) {
					notes += " ";
				}
				notes += stop_ptr->Notes ();
			}
			arcview_stop.Put_Field (notes_field, notes);
		}
		arcview_stop.clear ();
		arcview_stop.push_back (point);

		if (!arcview_stop.Write_Record ()) {
			Error ("Writing ArcView Stop Demand File");
		}
		num_stop++;
	}
	End_Progress ();

	arcview_stop.Close ();
}
