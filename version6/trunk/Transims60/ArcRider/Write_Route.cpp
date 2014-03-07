//*********************************************************
//	Write_Route - draw the transit line demand
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void ArcRider::Write_Route (void)
{
	int dir, j, riders, max_load, board, alight, run, runs, overlap;
	int route_field, name_field, mode_field, stops_field, runs_field, riders_field, notes_field;
	int load_field, board_field, alight_field;
	double length, side, side_offset;
	bool offset_flag, first;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Line_Data *line_ptr;
	Driver_Itr driver_itr;
	Points_Itr pt_itr;
	Integers run_flag;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;

	side = 0;
	offset_flag = (route_offset != 0.0);

	route_field = arcview_line.Field_Number ("ROUTE");
	name_field = arcview_line.Field_Number ("NAME");
	mode_field = arcview_line.Field_Number ("MODE");
	stops_field = arcview_line.Field_Number ("STOPS");
	runs_field = arcview_line.Field_Number ("RUNS");
	riders_field = arcview_line.Field_Number ("RIDERS");
	load_field = arcview_line.Field_Number ("MAX_LOAD");
	board_field = arcview_line.Field_Number ("MAX_BOARD");
	alight_field = arcview_line.Field_Number ("MAX_ALIGHT");
	notes_field = arcview_line.Field_Number ("NOTES");

	Show_Message (String ("Writing %s -- Record") % arcview_line.File_Type ());
	Set_Progress ();

	//---- set the overlap count ----

	if (!overlap_flag) {
		for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
			link_itr->Aoffset (0);
			link_itr->Boffset (0);
		}
	}

	//---- get the route data ----

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		if (select_routes && !route_range.In_Range (map_itr->first)) continue;

		line_ptr = &line_array [map_itr->second];

		if (select_modes && !select_mode [line_ptr->Mode ()]) continue;

		//---- check the link criteria ----

		if (select_links) {
			int num, n1, n2;
			Data_Range_Itr range_itr;

			for (range_itr = link_ranges.begin (); range_itr != link_ranges.end (); range_itr++) {
				num = range_itr->Max_Count ();
				n1 = n2 = 0;

				for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {

					//---- check the link sequence ----

					n2 = range_itr->Range_Count (*driver_itr);
					if (!n2) continue;

					if (n2 != n1 + 1) {
						n2 = 0;
						break;
					}
					if (n2 == num) break;
					n1 = n2;
				}
				if (n2 != num) continue;
			}
		}

		//---- set the run flags ----

		run_flag.assign (line_array.Max_Runs (), 1);

		if (select_start_times || select_end_times) {
			if (select_start_times) {
				stop_itr = line_ptr->begin ();

				for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
					if (!start_range.In_Range (run_itr->Schedule ())) {
						run_flag [run] = 0;
					}
				}
			}
			if (select_end_times) {
				stop_itr = line_ptr->end ()--;

				for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
					if (!end_range.In_Range (run_itr->Schedule ())) {
						run_flag [run] = 0;
					}
				}
			}
		}

		//---- save the route ridership data ----

		riders = board = alight = max_load = runs = 0;

		for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
			for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
				if ((select_start_times || select_end_times) && run_flag [run] == 0) continue;
				if (select_time_of_day && !time_range.In_Range (run_itr->Schedule ())) continue;

				if (run_itr->Board () > board) board = run_itr->Board ();
				if (run_itr->Alight () > alight) alight = run_itr->Alight ();
				if (run_itr->Load () > max_load)  max_load = run_itr->Load ();
				riders += run_itr->Board ();

				if (run_flag [run] == 1) {
					runs++;
					run_flag [run] = 2;
				}
			}
		}
		if (riders == 0) continue;

		//---- save the data to the file ----

		arcview_line.Put_Field (route_field, line_ptr->Route ());
		arcview_line.Put_Field (name_field, line_ptr->Name ());
		arcview_line.Put_Field (mode_field, line_ptr->Mode ());
		arcview_line.Put_Field (stops_field, (int) line_ptr->size ());
		arcview_line.Put_Field (runs_field, runs);
		arcview_line.Put_Field (riders_field, riders);
		arcview_line.Put_Field (load_field, max_load);
		arcview_line.Put_Field (board_field, board);
		arcview_line.Put_Field (alight_field, alight);
		arcview_line.Put_Field (notes_field, line_ptr->Notes ());
				
		arcview_line.clear ();
		first = true;

		//---- create transit path ----

		for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {
			dir_ptr = &dir_array [*driver_itr];
			dir = dir_ptr->Dir ();

			link_ptr = &link_array [dir_ptr->Link ()];

			length = UnRound (link_ptr->Length ());
			side_offset = route_offset;

			if (side_offset > length / 3.0) {
				side_offset = length / 3.0;
			}
			if (offset_flag) {
				if (!overlap_flag) {
					if (dir == 0) {
						overlap = link_ptr->Aoffset () + 1;
						link_ptr->Aoffset (overlap);
					} else {
						overlap = link_ptr->Boffset () + 1;
						link_ptr->Boffset (overlap);
					}
					side = side_offset = route_offset * overlap;

					if (side_offset > length / 3.0) {
						side_offset = length / 3.0;
					}
					length -= 2.0 * side_offset;
				} else {
					length -= 2.0 * side_offset;

					if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
						side = side_offset;
					} else {
						side = 0.0;
					}
				}
				j = 1;
			} else if (first) {
				j = 1;
				first = false;
			} else {
				j = 2;
			}
			if (length > 0.0) {
				Link_Shape (link_ptr, dir, points, side_offset, length, side);

				pt_itr = points.begin ();
				if (j > 1 && pt_itr != points.end ()) pt_itr++;

				for (; pt_itr != points.end (); pt_itr++) {
					arcview_line.push_back (*pt_itr);
				}
			}
		}

		if (arcview_line.size () > 0) {
			if (!arcview_line.Write_Record ()) {
				Error (String ("Writing %s") % arcview_line.File_Type ());
			}
			num_line++;
		}
	}
	End_Progress ();

	arcview_line.Close ();
}
