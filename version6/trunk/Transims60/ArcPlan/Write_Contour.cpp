//*********************************************************
//	Write_Contour - draw the path contour
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Write_Contour
//---------------------------------------------------------

void ArcPlan::Write_Contour (Plan_Data &plan)
{
	Dtime time, time_in, time_out, time1, time2;
	int i, length, len_in, len_out, link, dir, index;
	int time_contour, time_field, distance_contour, distance_field;
	double offset, offset1, seg_len, link_len, len, side, dist1, dist2, factor;
	bool flag;

	Plan_Leg_Itr leg_itr, prev_itr;
	Parking_Data *parking_ptr;

	Link_Data *link_ptr;
	Int_Map_Itr int_itr;
	Point_Map_Itr pt_itr;
	Dtime_Itr time_itr;
	Dbl_Itr dist_itr;

	time_contour = arcview_time.Field_Number ("CONTOUR");
	time_field = arcview_time.Field_Number ("TIME");
	distance_contour = arcview_distance.Field_Number ("CONTOUR");
	distance_field = arcview_distance.Field_Number ("DISTANCE");

	len_out = 0;
	time_out = 0;
	offset = 0.0;
	i = 0;

	for (leg_itr = prev_itr = plan.begin (); leg_itr != plan.end (); prev_itr = leg_itr++) {
		time_in = time_out;
		time_out += time = leg_itr->Time ();

		len_in = len_out;
		len_out += length = leg_itr->Length ();
		offset = 0.0;

		if (leg_itr->Type () == PARKING_ID && prev_itr->Mode () == DRIVE_MODE && prev_itr->Link_Type ()) {
			int_itr = parking_map.find (leg_itr->ID ());
			if (int_itr == parking_map.end ()) continue;

			parking_ptr = &parking_array [int_itr->second];

			link = prev_itr->Link_ID ();
			dir = prev_itr->Link_Dir ();

			int_itr = link_map.find (link);
			if (int_itr == link_map.end ()) continue;

			link_ptr = &link_array [int_itr->second];

			if (parking_ptr->Dir () != dir) {
				link_len = UnRound (link_ptr->Length () - parking_ptr->Offset ());
			} else {
				link_len = UnRound (parking_ptr->Offset ());
			}
			flag = false;
		} else if (leg_itr->Mode () == DRIVE_MODE && leg_itr->Link_Type ()) {
			link = leg_itr->Link_ID ();
			dir = leg_itr->Link_Dir ();

			int_itr = link_map.find (link);
			if (int_itr == link_map.end ()) continue;

			link_ptr = &link_array [int_itr->second];
			link_len = UnRound (link_ptr->Length ());

			if (prev_itr->Type () == PARKING_ID) {
				int_itr = parking_map.find (prev_itr->ID ());
				if (int_itr != parking_map.end ()) {
					parking_ptr = &parking_array [int_itr->second];
					if (parking_ptr->Dir () != dir) {
						offset = UnRound (link_ptr->Length () - parking_ptr->Offset ());
					} else {
						offset = UnRound (parking_ptr->Offset ());
					}
				}
			}
			flag = true;
		} else {
			continue;
		}

		//---- check the link flag ----

		if (dir) {
			index = link_ptr->BA_Dir ();
		} else {
			index = link_ptr->AB_Dir ();
		}
		if (link_flag [index]) continue;

		//---- process the link ----

		link_flag [index] = flag;

		if (link_ptr->AB_Dir () >= 0 && link_ptr->BA_Dir () >= 0) {
			side = link_offset;
		} else {
			side = 0.0;
		}

		//---- time contours ----

		if (time_flag) {

			//---- find the time contour ----

			time1 = time_in;
			time2 = time_out;
			seg_len = link_len - offset;
			offset1 = offset;

			for (i=0, time_itr = time_list.begin (); time_itr != time_list.end (); time_itr++, i++) {
				if (i == 0) continue;

				if (*time_itr <= time1) continue;

				if (*time_itr > time2) {

					time1 = time2;
					len = seg_len;

				} else {

					factor = time2 - time1;
					if (factor <= 0) continue;

					factor = (double) (*time_itr - time1) / factor;
					len = seg_len * factor;
					seg_len -= len;

					time1 = *time_itr;
				}
				arcview_time.clear ();

				Link_Shape (link_ptr, dir, arcview_time, offset1, len, side);

				offset1 += len;

				//---- save the link record ----

				if (arcview_time.size () > 0) {
					arcview_time.Put_Field (time_contour, i);
					arcview_time.Put_Field (time_field, time1);

					if (!arcview_time.Write_Record ()) {
						Error (String ("Writing %s") % arcview_time.File_Type ());
					}
					num_time++;
				}
				if (time1 >= time2) break;
			}
		}

		//---- distance contours ----

		if (distance_flag) {

			//---- find the distance contour ----

			dist1 = UnRound (len_in);
			dist2 = UnRound (len_out);
			seg_len = link_len - offset;
			offset1 = offset;

			for (i=0, dist_itr = distance_list.begin (); dist_itr != distance_list.end (); dist_itr++, i++) {
				if (i == 0) continue;

				if (*dist_itr <= dist1) continue;

				if (*dist_itr > dist2) {

					dist1 = dist2;
					len = seg_len;

				} else {

					factor = dist2 - dist1;
					if (factor <= 0) continue;

					factor = (double) (*dist_itr - dist1) / factor;
					len = seg_len * factor;
					seg_len -= len;

					dist1 = *dist_itr;
				}
				arcview_distance.clear ();

				Link_Shape (link_ptr, dir, arcview_distance, offset1, len, side);

				offset1 += len;

				//---- save the link record ----

				if (arcview_distance.size () > 0) {
					arcview_distance.Put_Field (distance_contour, i);
					arcview_distance.Put_Field (distance_field, dist1);

					//---- write the shape record ----

					if (!arcview_distance.Write_Record ()) {
						Error (String ("Writing %s") % arcview_distance.File_Type ());
					}
					num_distance++;
				}
				if (dist1 >= dist2) break;
			}
		}
	}
}
