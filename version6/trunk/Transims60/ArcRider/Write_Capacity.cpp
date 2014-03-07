//*********************************************************
//	Write_Capacity - draw the transit run capacity file
//*********************************************************

#include "ArcRider.hpp"

#define MAX_SEGMENT		20

//---------------------------------------------------------
//	Write_Capacity
//---------------------------------------------------------

void ArcRider::Write_Capacity (void)
{
	int i, j, num_links, dir_index, dir, index, index1, index2;
	int seg, segment, stop1 [MAX_SEGMENT], stop2 [MAX_SEGMENT];
	int ride, stop, run, runs, period, count;
	int per_runs, cap_runs, max_run, run_max, run_tot, cap_tot, max_load;
	int mode_field, route_field, time_field, period_field, avg_runs_field, avg_load_field, avg_fac_field;
	int cap_runs_field, cap_load_field,	cap_fac_field, max_run_field, max_load_field, max_fac_field, notes_field;
	double offset, length, link_len, side, avg_fac, cap_fac, max_fac, stop_off;
	bool offset_flag;
	Dtime low, high;

	Int_Itr int_itr;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Line_Data *line_ptr;
	Veh_Type_Data *veh_type_ptr;
	Driver_Itr driver_itr;
	Points_Itr pt_itr;
	Integers run_flag;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Line_Stop *line_stop_ptr;
	Line_Run *run_ptr;
	Stop_Data *stop_ptr;

	offset_flag = (route_offset != 0.0);
	side = 0.0;

	mode_field = arcview_cap.Field_Number ("MODE");		
	route_field = arcview_cap.Field_Number ("ROUTE");
	time_field = arcview_cap.Field_Number ("TIME");
	period_field = arcview_cap.Field_Number ("PERIOD");
	avg_runs_field = arcview_cap.Field_Number ("AVG_RUNS");
	avg_load_field = arcview_cap.Field_Number ("AVG_LOAD");
	avg_fac_field = arcview_cap.Field_Number ("AVG_FAC");
	cap_runs_field = arcview_cap.Field_Number ("CAP_RUNS");
	cap_load_field = arcview_cap.Field_Number ("CAP_LOAD");
	cap_fac_field = arcview_cap.Field_Number ("CAP_FAC");
	max_run_field = arcview_cap.Field_Number ("MAX_RUN");
	max_load_field = arcview_cap.Field_Number ("MAX_LOAD");
	max_fac_field = arcview_cap.Field_Number ("MAX_FAC");
	notes_field = arcview_cap.Field_Number ("NOTES");

	Show_Message (String ("Writing %s -- Record") % arcview_cap.File_Type ());
	Set_Progress ();

	//---- get the route data ----

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		if (select_routes && !route_range.In_Range (map_itr->first)) continue;

		line_ptr = &line_array [map_itr->second];

		if (select_modes && !select_mode [line_ptr->Mode ()]) continue;
		
		veh_type_ptr = &veh_type_array [line_ptr->Type ()];
		if (veh_type_ptr->Capacity () <= 0) continue;

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

		//---- process each time period ----

		runs = (int) line_ptr->begin ()->size ();

		for (period = 0; period < time_range.Num_Periods (); period++) {
			time_range.Period_Range (period, low, high);

			per_runs = cap_runs = max_run = run_tot = cap_tot = max_load = segment = 0;
			memset (stop1, '\0', sizeof (stop1));
			memset (stop2, '\0', sizeof (stop2));

			//---- process each run ----

			for (run=0; run < runs; run++) {
				if (run_flag [run] == 0) continue;

				//---- process each stop ----

				run_max = count = 0;

				for (stop=0, stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++, stop++) {
					run_ptr = &stop_itr->at (run);

					if (run_ptr->Schedule () < low || run_ptr->Schedule () >= high) continue;

					ride = run_ptr->Load ();
					if (ride > run_max) run_max = ride;
					count++;

					if (ride > veh_type_ptr->Capacity ()) {
						if (stop1 [segment] == 0) {
							stop1 [segment] = stop;
							stop2 [segment] = stop + 1;
						} else if (stop2 [segment] == stop || segment == MAX_SEGMENT - 1) {
							stop2 [segment] = stop + 1;
						} else {
							segment++;
							stop1 [segment] = stop;
							stop2 [segment] = stop + 1;
						}
					}
				}
				if (count > 0) {
					per_runs++;
					run_tot += run_max;
					if (run_max > veh_type_ptr->Capacity ()) {
						if (run_max > max_load) {
							max_load = run_max;
							max_run = run;
						}
						cap_runs++;
						cap_tot += run_max;
					}
				}
			}
			if (cap_runs == 0) continue;

			if (per_runs > 0) {
				avg_fac = (double) run_tot / per_runs;
				run_tot = (int) (avg_fac + 0.5);
				avg_fac /= veh_type_ptr->Capacity ();
			} else {
				run_tot = 0;
				avg_fac = 0;
			}
			if (cap_runs > 0) {
				cap_fac = (double) cap_tot / cap_runs;
				cap_tot = (int) (cap_fac + 0.5);
				cap_fac /= veh_type_ptr->Capacity ();
			} else {
				cap_tot = 0;
				cap_fac = 0;
			}
			if (max_run > 0) {
				max_fac = (double) max_load / veh_type_ptr->Capacity ();
			} else {
				max_fac = 0.0;
			}
			arcview_cap.Put_Field (mode_field, line_ptr->Mode ());
			arcview_cap.Put_Field (route_field, line_ptr->Route ());
			arcview_cap.Put_Field (time_field, high);
			arcview_cap.Put_Field (period_field, period + 1);
			arcview_cap.Put_Field (avg_runs_field, per_runs);
			arcview_cap.Put_Field (avg_load_field, run_tot);
			arcview_cap.Put_Field (avg_fac_field, avg_fac);
			arcview_cap.Put_Field (cap_runs_field, cap_runs);
			arcview_cap.Put_Field (cap_load_field, cap_tot);
			arcview_cap.Put_Field (cap_fac_field, cap_fac);
			arcview_cap.Put_Field (max_run_field, max_run);
			arcview_cap.Put_Field (max_load_field, max_load);
			arcview_cap.Put_Field (max_fac_field, max_fac);
			arcview_cap.Put_Field (notes_field, line_ptr->Name ());

			arcview_cap.parts.clear ();
			arcview_cap.clear ();

			num_links = (int) line_ptr->driver_array.size ();

			//---- draw each segment ----

			for (seg = 0; seg <= segment; seg++) {
				arcview_cap.parts.push_back ((int) arcview_cap.size ());

				//---- find the end index ----

				line_stop_ptr = &line_ptr->at (stop2 [seg]);

				stop = line_stop_ptr->Stop ();

				stop_ptr = &stop_array [stop];

				map_itr = dir_map.find (stop_ptr->Link_Dir ());
				if (map_itr == dir_map.end ()) continue;
				dir_index = map_itr->second;

				for (i=num_links-1; i >= 0; i--) {
					if (dir_index == line_ptr->driver_array [i]) break;
				}
				index2 = i;

				//---- find the start index ----

				line_stop_ptr = &line_ptr->at (stop1 [seg]);

				stop = line_stop_ptr->Stop ();

				stop_ptr = &stop_array [stop];

				map_itr = dir_map.find (stop_ptr->Link_Dir ());
				if (map_itr == dir_map.end ()) continue;
				dir_index = map_itr->second;

				for (i=0; i < index2; i++) {
					if (dir_index == line_ptr->driver_array [i]) break;
				}
				index1 = i;

				//---- create transit legs ----

				for (j=stop1 [seg]; i <= index2; i++) {
					index = line_ptr->driver_array [i];

					dir_ptr = &dir_array [index];

					dir = dir_ptr->Dir ();
					link_ptr = &link_array [dir_ptr->Link ()];

					link_len = UnRound (link_ptr->Length ());

					if (offset_flag) {
						if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
							side = route_offset;
						} else {
							side = 0.0;
						}
					}
					offset = route_offset;

					//---- check for stops ----

					while (stop_ptr->Link_Dir () == dir_ptr->Link_Dir ()) {

						stop_off = stop_ptr->Offset ();
						length = stop_off - offset;
						if (length < 0.1) continue;

						Link_Shape (link_ptr, dir, points, offset, length, side);

						offset = stop_off;

						if (j != stop1 [seg]) {

							//---- draw the link to the stop ----

							pt_itr = points.begin ();

							if (side == 0.0) {
								pt_itr++;
							}
							for (; pt_itr != points.end (); pt_itr++) {
								arcview_cap.push_back (*pt_itr);
							}
						}

						//---- find the next stop -----

						if (++j > stop2 [seg]) break;
				
						line_stop_ptr = &line_ptr->at (j);
						stop = line_stop_ptr->Stop ();

						stop_ptr = &stop_array [stop];
					}

					//---- complete the link ----

					if (i < index2) {
						link_len = UnRound (link_ptr->Length ());

						length = link_len - 2 * route_offset;
						if (length < 0.1) continue;

						offset = route_offset;

						if (offset_flag) {
							if (link_ptr->AB_Dir () > 0 && link_ptr->BA_Dir () > 0) {
								side = route_offset;
							} else {
								side = 0.0;
							}
						}

						Link_Shape (link_ptr, dir, points, offset, length, side);

						//---- add the link to the route ----

						pt_itr = points.begin ();

						if (side == 0.0 && i > index1) {
							pt_itr++;
						}
						for (; pt_itr != points.end (); pt_itr++) {
							arcview_cap.push_back (*pt_itr);
						}
					}
				}
			}
			if (arcview_cap.size () > 0) {
				if (!arcview_cap.Write_Record ()) {
					Error (String ("Writing %s") % arcview_cap.File_Type ());
				}
				num_cap++;
			}
		}
	}
	End_Progress ();

	arcview_cap.Close ();
}
