//*********************************************************
//	Write_Route.cpp - write the subarea transit route file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void SubareaNet::Write_Route (void)
{
	int links, route, s, s1;
	int i, j, stops, stop, time, diff, link_dir, offset, new_dist, old_dist;
	int first_stop, first_link, first_index, first_dir;
	int last_stop, last_link, last_index, last_dir;
	double first_factor, last_factor;
	bool version3_flag, sub_flag, split_flag, save_flag;

	Line_Data *line_ptr, line_rec;
	Link_Data *link_ptr;
	Stop_Data *stop_ptr, *stop2_ptr;
	Dir_Data *dir_ptr;
	Int_Map_Itr map_itr;
	Line_Itr line_itr;
	Line_Stop *line_stop_ptr, line_stop, *line_stop1_ptr, *line_stop2_ptr;
	Line_Run line_run;
	Line_Run_Itr run1_itr, run2_itr;
	Int_Itr int_itr;
	Driver_Array *driver_ptr;

	Line_File *line_file = (Line_File *) System_File_Header (NEW_TRANSIT_ROUTE);
	Schedule_File *schedule_file = (Schedule_File *) System_File_Header (NEW_TRANSIT_SCHEDULE);
	Driver_File *driver_file = (Driver_File *) System_File_Header (NEW_TRANSIT_DRIVER);

	version3_flag = (line_file->Dbase_Format () == VERSION3);

	map_itr = --line_map.end ();

	line_ptr = &line_array [map_itr->second];
	max_route = line_ptr->Route ();
	max_route = ((max_route + 1000) / 1000) * 1000;

	//---- process each subarea route ----
	
	Show_Message ("Writing Subarea Transit Route Data -- Record");
	Set_Progress ();

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		Show_Progress ();
			
		stops = (int) line_itr->size ();
		line_stop_ptr = &line_itr->at (0);

		driver_ptr = &(line_itr->driver_array);
		links = (int) driver_ptr->size ();

		sub_flag = split_flag = save_flag = false;

		s = 0;
		stop_ptr = &stop_array [line_stop_ptr->Stop ()];

		first_index = first_link = last_link = first_dir = 0;
		last_index = stops - 1;

		for (i=0; i < links && s < stops; i++) {
			dir_ptr = &dir_array [driver_ptr->at (i)];
			link_ptr = &link_array [dir_ptr->Link ()];

			link_dir = dir_ptr->Link_Dir ();

			s1 = -1;
			while (s < stops && stop_ptr->Link_Dir () == link_dir) {
				if (s1 == -1) s1 = s;
				if (++s >= stops) break;

				line_stop_ptr = &line_itr->at (s);
				stop_ptr = &stop_array [line_stop_ptr->Stop ()];
			}

			//---- check for a subarea link ----

			if (!sub_flag && link_ptr->Type () > 0 && s > 0) {

				//---- enter the subarea ----

				sub_flag = true;
				first_link = i;
				if (s1 >= 0) {
					first_index = s1;
				} else {
					first_index = s;
				}
				if (link_ptr->Type () < 3) {
					if (dir_ptr->Dir () == 0) {
						if (link_ptr->Type () == 1) {
							sub_flag = false;
						}
					} else if (link_ptr->Type () == 2) {
						sub_flag = false;
					}

					//---- exit the subarea ----

					if (!sub_flag) {
						last_link = i;
						last_index = s - 1;
						save_flag = true;
					}
				}

			} else if (sub_flag && (link_ptr->Type () < 3 || s >= stops)) {

				//---- exit the subarea ----

				sub_flag = false;
				last_link = i;
				last_index = s - 1;
				save_flag = true;
			}
			if (!save_flag) continue;
			save_flag = false;

			//---- save the subarea route segment ----
			//---- set the route number ----

			if (split_flag) {
				route = ++max_route;
				new_route++;
			} else {
				route = line_itr->Route ();
				split_flag = true;
			}

			//---- output the route records ----

			line_rec.Clear ();

			line_rec.Route (route);
			line_rec.Mode (line_itr->Mode ());
			line_rec.Type (line_itr->Type ());
			line_rec.Name (line_itr->Name ());
			line_rec.Notes (line_itr->Notes ());

			for (j=first_link; j <= last_link; j++) {
				line_rec.driver_array.push_back (driver_ptr->at (j));
			}

			first_factor = last_factor = 1.0;
			first_stop = last_stop = 0;

			//---- add a stop to the beginning of the line ----

			if (first_index > 0) {
				first_dir = driver_ptr->at (first_link);

				dir_ptr = &dir_array [first_dir];

				first_dir = dir_ptr->Link_Dir ();

				first_stop = dir_ptr->Use_Index ();

				line_stop2_ptr = &line_itr->at (first_index);

				line_stop.Stop (first_stop);
				line_stop.Zone (line_stop2_ptr->Zone ());

				line_rec.push_back (line_stop);

				line_stop_ptr = &line_rec.back ();

				//---- find the distance to the stop ahead ----

				stop2_ptr = &stop_array [first_stop];

				new_dist = -stop2_ptr->Offset ();

				stop = line_stop2_ptr->Stop ();
				stop2_ptr = &stop_array [stop];
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == first_dir) {
					new_dist += stop2_ptr->Offset ();
				} else {
					for (j=first_link; j < links; j++) {
						dir_ptr = &dir_array [driver_ptr->at (j)];

						if (link_dir != dir_ptr->Link_Dir ()) {
							link_ptr = &link_array [dir_ptr->Link ()];
							new_dist += link_ptr->Length ();
						} else {
							new_dist += stop2_ptr->Offset ();
							break;
						}
					}
				}

				//---- find the distance to the stop behind ----

				stop2_ptr = &stop_array [first_stop];
				old_dist = stop2_ptr->Offset ();

				line_stop1_ptr = &line_itr->at (first_index - 1);
				
				stop = line_stop1_ptr->Stop ();
				stop2_ptr = &stop_array [stop];
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == first_dir) {
					old_dist -= stop2_ptr->Offset ();
				} else {
					for (j=first_link-1; j >= 0; j--) {
						dir_ptr = &dir_array [driver_ptr->at (j)];
						link_ptr = &link_array [dir_ptr->Link ()];

						old_dist += link_ptr->Length ();

						if (link_dir == dir_ptr->Link_Dir ()) {
							old_dist -= stop2_ptr->Offset ();
							break;
						}
					}
				}
				old_dist += new_dist;
				if (old_dist > 0) {
					first_factor = (double) new_dist / old_dist;
				}

				//---- add schedule data ----

				for (run1_itr = line_stop1_ptr->begin (), run2_itr = line_stop2_ptr->begin ();
					run1_itr != line_stop1_ptr->end (); run1_itr++, run2_itr++) {

					time = run2_itr->Schedule ();
					diff = time - run1_itr->Schedule ();
					diff = (int) (diff * first_factor + 0.5);

					line_run.Schedule ((time - diff));

					line_stop_ptr->push_back (line_run);
				}
			}

			//---- save the stop records ----

			for (j=first_index; j <= last_index; j++) {
				line_rec.push_back (line_itr->at (j));
			}

			//---- add a stop to the end of the line ----

			if (last_index < stops - 1) {
				last_dir = driver_ptr->at (last_link);

				dir_ptr = &dir_array [last_dir];

				last_dir = dir_ptr->Link_Dir ();

				last_stop = dir_ptr->Use_Index ();

				line_stop1_ptr = &line_itr->at (last_index);

				line_stop.Stop (last_stop);
				line_stop.Zone (line_stop1_ptr->Zone ());

				line_rec.push_back (line_stop);

				line_stop_ptr = &line_rec.back ();

				//---- find the distance to the stop ahead ----

				stop2_ptr = &stop_array [last_stop];

				offset = stop2_ptr->Offset ();
				old_dist = -offset;

				line_stop2_ptr = &line_itr->at (last_index + 1);
				
				stop = line_stop2_ptr->Stop ();
				stop2_ptr = &stop_array [stop];
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == last_dir) {
					old_dist += stop2_ptr->Offset ();
				} else {
					for (j=last_link; j < links; j++) {
						dir_ptr = &dir_array [driver_ptr->at (j)];

						if (link_dir != dir_ptr->Link_Dir ()) {
							link_ptr = &link_array [dir_ptr->Link ()];
							old_dist += link_ptr->Length ();
						} else {
							old_dist += stop2_ptr->Offset ();
							break;
						}
					}
				}

				//---- find the distance to the stop behind ----
				
				stop2_ptr = &stop_array [last_stop];
				new_dist = stop2_ptr->Offset ();
				
				stop = line_stop1_ptr->Stop ();
				stop2_ptr = &stop_array [stop];
				link_dir = stop2_ptr->Link_Dir ();

				if (link_dir == first_dir) {
					new_dist -= stop2_ptr->Offset ();
				} else {
					for (j=last_link-1; j >= 0; j--) {
						dir_ptr = &dir_array [driver_ptr->at (j)];
						link_ptr = &link_array [dir_ptr->Link ()];

						new_dist += link_ptr->Length ();

						if (link_dir == dir_ptr->Link_Dir ()) {
							new_dist -= stop2_ptr->Offset ();
							break;
						}
					}
				}
				old_dist += new_dist;
				if (old_dist > 0) {
					last_factor = (double) new_dist / old_dist;
				}

				//---- add schedule data ----

				for (run1_itr = line_stop1_ptr->begin (), run2_itr = line_stop2_ptr->begin ();
					run1_itr != line_stop1_ptr->end (); run1_itr++, run2_itr++) {

					time = run1_itr->Schedule ();
					diff = run2_itr->Schedule () - time;
					diff = (int) (diff * last_factor + 0.5);

					line_run.Schedule ((time + diff));

					line_stop_ptr->push_back (line_run);
				}
			}

			//---- write the line data ----

			nroute += Put_Line_Data (*line_file, line_rec);
			ndriver += Put_Driver_Data (*driver_file, line_rec);
			nschedule += Put_Schedule_Data (*schedule_file, line_rec);
		}
	}
	End_Progress ();
}
