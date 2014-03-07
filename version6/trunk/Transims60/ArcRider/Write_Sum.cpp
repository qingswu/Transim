//*********************************************************
//	Write_Sum - draw the transit line groups
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Write_Sum
//---------------------------------------------------------

void ArcRider::Write_Sum (void)
{
	int j, num, dir, routes, tot_on, overlap, index;
	int riders, max_load, board, alight, stops, run, runs;
	int group_field, name_field, routes_field, stops_field, runs_field, riders_field;
	int load_field, board_field, alight_field;
	double side, side_offset, length;
	bool offset_flag, first, flag_ab, flag_ba;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Line_Data *line_ptr;
	Driver_Itr driver_itr;
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr;
	Integers run_flag;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Points_Itr pt_itr;
	Link_Itr link_itr;

	offset_flag = (route_offset != 0.0);
	side = 0.0;

	group_field = arcview_sum.Field_Number ("GROUP");
	name_field = arcview_sum.Field_Number ("NAME");
	routes_field = arcview_sum.Field_Number ("ROUTES");
	stops_field = arcview_sum.Field_Number ("STOPS");
	runs_field = arcview_sum.Field_Number ("RUNS");
	riders_field = arcview_sum.Field_Number ("RIDERS");
	load_field = arcview_sum.Field_Number ("MAX_LOAD");
	board_field = arcview_sum.Field_Number ("MAX_BOARD");
	alight_field = arcview_sum.Field_Number ("MAX_ALIGHT");

	Show_Message (String ("Writing %s -- Record") % arcview_sum.File_Type ());
	Set_Progress ();

	//---- set the overlap count ----

	if (!overlap_flag) {
		for (index=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, index++) {
			link_itr->Aoffset (0);
			link_itr->Boffset (0);
		}
	}

	//---- process each line group ----

	for (num=1; num <= line_equiv.Max_Group (); num++) {

		group = line_equiv.Group_List (num);
		if (group == 0) continue;

		Show_Progress ();

		arcview_sum.Put_Field (group_field, num);
		arcview_sum.Put_Field (name_field, line_equiv.Group_Label (num));
		arcview_sum.clear ();
		arcview_sum.parts.clear ();

		riders = board = alight = max_load = runs = stops = routes = 0;

		//---- set the overlap count ----

		if (!overlap_flag) {
			for (index=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, index++) {

				flag_ab = flag_ba = false;

				for (itr = group->begin (); itr != group->end () && !(flag_ab && flag_ba); itr++) {
					map_itr = line_map.find (*itr);
					if (map_itr == line_map.end ()) continue;

					line_ptr = &line_array [map_itr->second];

					for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {
						dir_ptr = &dir_array [*driver_itr];
						dir = dir_ptr->Dir ();

						if (dir_ptr->Link () == index) {
							if (dir == 1) {
								flag_ba = true;
							} else {
								flag_ab = true;
							}
							if (flag_ab && flag_ba) break;
						}
					}
				}
				if (flag_ab) link_itr->Aoffset (link_itr->Aoffset () + 1);
				if (flag_ba) link_itr->Boffset (link_itr->Boffset () + 1);
			}
		}

		//---- sum the lines in the group ----

		for (itr = group->begin (); itr != group->end (); itr++) {
			map_itr = line_map.find (*itr);
			if (map_itr == line_map.end ()) continue;

			line_ptr = &line_array [map_itr->second];

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

			tot_on = 0;

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
				for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
					if ((select_start_times || select_end_times) && run_flag [run] == 0) continue;
					if (select_time_of_day && !time_range.In_Range (run_itr->Schedule ())) continue;

					if (run_itr->Board () > board) board = run_itr->Board ();
					if (run_itr->Alight () > alight) alight = run_itr->Alight ();
					if (run_itr->Load () > max_load)  max_load = run_itr->Load ();
					riders += run_itr->Board ();
					tot_on += run_itr->Board ();

					if (run_flag [run] == 1) {
						runs++;
						run_flag [run] = 2;
					}
				}
			}
			if (tot_on == 0) continue;

			routes++;

			arcview_sum.parts.push_back ((int) arcview_sum.size ());
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
							overlap = link_ptr->Aoffset ();
						} else {
							overlap = link_ptr->Boffset ();
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
				Link_Shape (link_ptr, dir, points, side_offset, length, side);

				pt_itr = points.begin ();
				if (j > 1 && pt_itr != points.end ()) pt_itr++;

				for (; pt_itr != points.end (); pt_itr++) {
					arcview_sum.push_back (*pt_itr);
				}
			}
		}

		//---- save the data to the file ----

		if (routes > 0 && arcview_sum.size () > 0) {
			arcview_sum.Put_Field (routes_field, routes);
			arcview_sum.Put_Field (stops_field, stops);
			arcview_sum.Put_Field (runs_field, runs);
			arcview_sum.Put_Field (riders_field, riders);
			arcview_sum.Put_Field (load_field, max_load);
			arcview_sum.Put_Field (board_field, board);
			arcview_sum.Put_Field (alight_field, alight);

			if (!arcview_sum.Write_Record ()) {
				Error (String ("Writing %s") % arcview_sum.File_Type ());
			}
			num_sum++;
		}
	}
	End_Progress ();

	arcview_sum.Close ();
}
