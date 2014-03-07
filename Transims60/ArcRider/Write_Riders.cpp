//*********************************************************
//	Write_Riders - draw the transit ridership polygons
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Riders
//---------------------------------------------------------

void ArcRider::Write_Riders (void)
{
	int i, j, link, dir, link_field, dir_field, rider_field, seg_field, on_field, off_field, notes_field;
	int stop_num [20], nstop, stop, max_load, riders, seg, board, alight, *load, *enter, *cap_enter, capacity;
	int start, end, cap_start, cap_end, runs, run_field, factor_field, cap_field, capfac_field, node;
	double width, offset, end_offset, len, length, stop_off [20], factor, capfac;
	String from_text, to_text, notes;

	XYZ_Point point;
	Points_Itr pt_itr;
	Dir_Itr dir_itr;
	Link_Data *link_ptr;
	Stop_Data *stop_ptr;
	Node_Data *node_ptr;
	
	nstop = stop = start = cap_start = 0;
	link_field = arcview_rider.Field_Number ("LINK");
	dir_field = arcview_rider.Field_Number ("DIR");
	seg_field = arcview_rider.Field_Number ("SEGMENT");
	on_field = arcview_rider.Field_Number ("BOARD");
	rider_field = arcview_rider.Field_Number ("RIDERS");
	off_field = arcview_rider.Field_Number ("ALIGHT");
	run_field = arcview_rider.Field_Number ("RUNS");
	factor_field = arcview_rider.Field_Number ("LOADFAC");
	cap_field = arcview_rider.Field_Number ("CAPACITY");
	capfac_field = arcview_rider.Field_Number ("CAPFAC");
	notes_field = arcview_rider.Field_Number ("NOTES");

	Show_Message (String ("Writing %s -- Record") % arcview_rider.File_Type ());
	Set_Progress ();
	
	//---- process each link direction ----

	load = &load_data.at (0);
	enter = &enter_runs.at (0);
	cap_enter = &enter_cap.at (0);

	for (dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, load++, enter++, cap_enter++) {
		Show_Progress ();

		link = dir_itr->Link ();
		dir = dir_itr->Dir ();

		if (dir_itr->First_Pocket () == -1) {
			if (*load < min_value) continue;
			nstop = 0;

		} else {

			//---- gather the stop data ----

			for (i=0; i < 20; i++) {
				if (i == 0) {
					stop = dir_itr->First_Pocket ();
				} else {
					stop = stop_list [stop];
				}
				if (stop < 0) {
					nstop = i;
					break;
				}
				stop_num [i] = stop;
				
				stop_ptr = &stop_array [stop];
				stop_off [i] = UnRound (stop_ptr->Offset ());
			}

			//---- sort the stop offsets ----

			for (i=0; i < nstop-1; i++) {
				for (j=i+1; j < nstop; j++) {
					if (stop_off [i] > stop_off [j]) {
						offset = stop_off [i];
						stop_off [i] = stop_off [j];
						stop_off [j] = offset;

						stop = stop_num [i];
						stop_num [i] = stop_num [j];
						stop_num [j] = stop;
					}
				}
			}

			//---- calculate the max load ----

			max_load = riders = *load;
			runs = *enter;
			capacity = *cap_enter;

			for (i=0; i < nstop; i++) {
				stop = stop_num [i];
				riders += board_data [stop] - alight_data [stop];
				if (riders > max_load) max_load = riders;
			}
			if (max_load < min_value) continue;
		}

		//---- initialize the record ----

		link_ptr = &link_array [link];

		arcview_rider.Put_Field (link_field, link_ptr->Link ());
		arcview_rider.Put_Field (dir_field, dir);

		length = UnRound (link_ptr->Length ());

		offset = 0.0;
		riders = *load;
		runs = *enter;
		capacity = *cap_enter;
		seg = 1;
		board = 0;

		if (notes_field >= 0) {
			if (dir_itr->Dir () == 0) {
				node = link_ptr->Anode ();
			} else {
				node = link_ptr->Bnode ();
			}
			node_ptr = &node_array [node];

			if (!node_ptr->Notes ().empty ()) {
				from_text = node_ptr->Notes ();
			} else {
				from_text ("%d") % node_ptr->Node ();
			}
		}

		//---- process each link segment ----

		for (i=0; i <= nstop; i++) {
			notes.clear ();	

			if (i == nstop) {
				end_offset = length;
				alight = 0;
				end = 0;
				cap_end = 0;

				if (notes_field >= 0) {
					if (dir_itr->Dir () == 0) {
						node = link_ptr->Bnode ();
					} else {
						node = link_ptr->Anode ();
					}
					node_ptr = &node_array [node];

					if (!node_ptr->Notes ().empty ()) {
						to_text = node_ptr->Notes ();
					} else {
						to_text ("%d") % node_ptr->Node ();
					}
					notes = from_text + " to " + to_text;
					arcview_rider.Put_Field (notes_field, notes);
				}
			} else {
				stop = stop_num [i];
				end_offset = stop_off [i];
				alight = alight_data [stop];
				end = end_runs [stop];
				cap_end = end_cap [stop];

				if (notes_field >= 0) {
					stop_ptr = &stop_array [stop];

					if (!stop_ptr->Name ().empty ()) {
						to_text = stop_ptr->Name ();
					} else {
						to_text ("Stop %d") % stop_ptr->Stop ();
					}
					if (!stop_ptr->Notes ().empty ()) {
						to_text += " ";
						to_text += stop_ptr->Notes ();
					}
					notes = from_text + " to " + to_text;
					arcview_rider.Put_Field (notes_field, notes);
					from_text.swap (to_text);
				}
			}

			//---- check the ridership ----

			if (riders >= min_value && runs > 0) {

				//---- write the data fields ----
	
				arcview_rider.parts.clear ();
				arcview_rider.clear ();
				
				arcview_rider.parts.push_back ((int) arcview_rider.size ());

				if (capacity == 0) capacity = runs;

				factor = (double) riders / runs;
				capfac = (double) riders / capacity;

				arcview_rider.Put_Field (seg_field, seg);
				arcview_rider.Put_Field (on_field, board);
				arcview_rider.Put_Field (rider_field, riders);
				arcview_rider.Put_Field (off_field, alight);
				arcview_rider.Put_Field (run_field, runs);
				arcview_rider.Put_Field (factor_field, factor);
				arcview_rider.Put_Field (cap_field, capacity);
				arcview_rider.Put_Field (capfac_field, capfac);

				len = end_offset - offset;

				if (len > 0.0) {

					//---- get the line width ----

					if (bandwidth_flag) {
						width = arcview_rider.Get_Double (width_field);
						if (width < 0) width = -width;

						width = width / width_factor;

						if (width < min_width) {
							width = min_width;
						} else if (width > max_width) {
							width = max_width;
						}
						width = -width;

						//---- get the centerline points ----

						Link_Shape (link_ptr, dir, points, offset, len, 0.0);

						point = points [0];
				
						arcview_rider.assign (points.begin (), points.end ());

						//---- get the outside points of the band ----

						if (length >= end_offset) {

							Link_Shape (link_ptr, 1 - dir, points, (length - end_offset), len, width);

							if (max_angle > 0 && min_length > 0) {
								Smooth_Shape (points, max_angle, min_length);
							}
							arcview_rider.insert (arcview_rider.end (), points.begin (), points.end ());
						}

						//---- close the polygon ----

						arcview_rider.push_back (point);

					} else {

						width = route_offset;

						Link_Shape (link_ptr, dir, arcview_rider, offset, len, width);
					}

					//---- write the shape record ----

					if (arcview_rider.size () > 0) {
						if (!arcview_rider.Write_Record ()) {
							Error ("Writing ArcView Ridership File");
						}
						num_rider++;
					}
				}
			}

			//---- initialize the next segment ----

			if (i < nstop) {
				board = board_data [stop];
				start = start_runs [stop];
				cap_start = start_cap [stop];
			}
			runs += start - end;
			capacity += cap_start - cap_end;
			riders += board - alight;
			offset = end_offset;
			seg++;
		}
	}
	End_Progress ();

	arcview_rider.Close ();
}
