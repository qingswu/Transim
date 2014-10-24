//*********************************************************
//	Execute.cpp - path building service
//*********************************************************

#include "Router_Service.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Router_Service::Execute (void)
{
	int anode, bnode, link, index, *link_ptr, *next_ptr;
	Int2_Map_Stat ab_stat;
	Int_Map_Itr map_itr;

	//---- compile the type script ----

	if (script_flag) {
		if (Report_Flag (SCRIPT_REPORT)) {
			Header_Number (SCRIPT_REPORT);

			if (!Break_Check (10)) {
				Print (1);
				Page_Header ();
			}
		}
		Db_Base_Array files;

		files.push_back (System_File_Base (HOUSEHOLD));

		type_script.Initialize (files, random.Seed () + 1);

		if (!type_script.Compile (script_file, Report_Flag (SCRIPT_REPORT))) {
			Error ("Compiling Household Type Script");
		}
		if (Report_Flag (STACK_REPORT)) {
			Header_Number (STACK_REPORT);

			type_script.Print_Commands (false);
		}
		Header_Number (0);
	}

	//---- read the data files ----

	Data_Service::Execute ();

	if (!Control_Flag ()) {
		Error ("Router_Service::Program_Control has Not Been Called");
	}

	//---- initialize the data structures ----

	Show_Message ("Initializing Data Structures -- Record");
	Set_Progress ();

	if (walk_path_flag) {
		int *first_ptr;
		Link_Itr link_itr;
		List_Data *list_ptr;

		walk_list.resize (node_array.size (), -1);
		walk_link.resize (link_array.size ());

		for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
			Show_Progress ();

			if (!Use_Permission (link_itr->Use (), WALK)) continue;
			list_ptr = &walk_link [link];

			first_ptr = &walk_list [link_itr->Anode ()];
			list_ptr->From_List (*first_ptr);
			*first_ptr = link;

			first_ptr = &walk_list [link_itr->Bnode ()];
			list_ptr->To_List (*first_ptr);
			*first_ptr = link;

			if (!bike_path_flag) {
				if (link_itr->Anode () < link_itr->Bnode ()) {
					anode = link_itr->Anode ();
					bnode = link_itr->Bnode ();
				} else {
					anode = link_itr->Bnode ();
					bnode = link_itr->Anode ();
				}
				ab_link.insert (Int2_Map_Data (Int2_Key (anode, bnode), link));
			}
		}
	}
	if (bike_path_flag) {
		int *first_ptr;
		Link_Itr link_itr;
		List_Data *list_ptr;

		bike_list.resize (node_array.size (), -1);
		bike_link.resize (link_array.size ());

		for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
			Show_Progress ();

			if (!Use_Permission (link_itr->Use (), BIKE)) continue;
			list_ptr = &bike_link [link];

			first_ptr = &bike_list [link_itr->Anode ()];
			list_ptr->From_List (*first_ptr);
			*first_ptr = link;

			first_ptr = &bike_list [link_itr->Bnode ()];
			list_ptr->To_List (*first_ptr);
			*first_ptr = link;

			if (!walk_path_flag) {
				if (link_itr->Anode () < link_itr->Bnode ()) {
					anode = link_itr->Anode ();
					bnode = link_itr->Bnode ();
				} else {
					anode = link_itr->Bnode ();
					bnode = link_itr->Anode ();
				}
				ab_link.insert (Int2_Map_Data (Int2_Key (anode, bnode), link));
			}
		}
	}
	if (walk_path_flag || bike_path_flag) {
		Link_Itr link_itr;

		for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
			Show_Progress ();

			if (!Use_Permission (link_itr->Use (), WALK) && 
				!Use_Permission (link_itr->Use (), BIKE)) continue;

			if (link_itr->Anode () < link_itr->Bnode ()) {
				anode = link_itr->Anode ();
				bnode = link_itr->Bnode ();
			} else {
				anode = link_itr->Bnode ();
				bnode = link_itr->Anode ();
			}
			ab_link.insert (Int2_Map_Data (Int2_Key (anode, bnode), link));
		}
	}

	//---- process access links ----

	access_link_flag = System_Data_Flag (ACCESS_LINK);

	if (access_link_flag) {
		Access_Itr access_itr;
		List_Data *from_ptr, *to_ptr, *first_ptr;

		loc_access.resize (location_array.size ());
		if (drive_path_flag) park_access.resize (parking_array.size ());
		if (transit_path_flag) stop_access.resize (stop_array.size ());
		if (transit_path_flag) node_access.resize (node_array.size ());

		from_access.resize (access_array.size ());
		to_access.resize (access_array.size ());

		for (index=0, access_itr = access_array.begin (); access_itr != access_array.end (); access_itr++, index++) {
			Show_Progress ();

			from_ptr = &from_access [index];
			to_ptr = &to_access [index];

			//---- from list ----

			if (access_itr->From_Type () == LOCATION_ID) {
				first_ptr = &loc_access [access_itr->From_ID ()];

				from_ptr->From_List (first_ptr->From_List ());
				first_ptr->From_List (index);

				if (access_itr->Dir () != 1) {
					to_ptr->From_List (first_ptr->To_List ());
					first_ptr->To_List (index);
				}
			} else if (access_itr->From_Type () == PARKING_ID) {
				if (drive_path_flag) {
					first_ptr = &park_access [access_itr->From_ID ()];

					from_ptr->From_List (first_ptr->From_List ());
					first_ptr->From_List (index);

					if (access_itr->Dir () != 1) {
						to_ptr->From_List (first_ptr->To_List ());
						first_ptr->To_List (index);
					}
				}
			} else if (access_itr->From_Type () == STOP_ID) {
				if (transit_path_flag) {
					first_ptr = &stop_access [access_itr->From_ID ()];

					from_ptr->From_List (first_ptr->From_List ());
					first_ptr->From_List (index);

					if (access_itr->Dir () != 1) {
						to_ptr->From_List (first_ptr->To_List ());
						first_ptr->To_List (index);
					}
				}
			} else if (access_itr->From_Type () == NODE_ID) {
				if (transit_path_flag) {
					first_ptr = &node_access [access_itr->From_ID ()];

					from_ptr->From_List (first_ptr->From_List ());
					first_ptr->From_List (index);

					if (access_itr->Dir () != 1) {
						to_ptr->From_List (first_ptr->To_List ());
						first_ptr->To_List (index);
					}
				}
			}

			//---- to list ----

			if (access_itr->To_Type () == LOCATION_ID) {
				first_ptr = &loc_access [access_itr->To_ID ()];

				to_ptr->To_List (first_ptr->To_List ());
				first_ptr->To_List (index);

				if (access_itr->Dir () != 1) {
					from_ptr->To_List (first_ptr->From_List ());
					first_ptr->From_List (index);
				}
			} else if (access_itr->To_Type () == PARKING_ID) {
				if (drive_path_flag) {
					first_ptr = &park_access [access_itr->To_ID ()];

					to_ptr->To_List (first_ptr->To_List ());
					first_ptr->To_List (index);

					if (access_itr->Dir () != 1) {
						from_ptr->To_List (first_ptr->From_List ());
						first_ptr->From_List (index);
					}
				}
			} else if (access_itr->To_Type () == STOP_ID) {
				if (transit_path_flag) {
					first_ptr = &stop_access [access_itr->To_ID ()];

					to_ptr->To_List (first_ptr->To_List ());
					first_ptr->To_List (index);

					if (access_itr->Dir () != 1) {
						from_ptr->To_List (first_ptr->From_List ());
						first_ptr->From_List (index);
					}
				}
			} else if (access_itr->To_Type () == NODE_ID) {
				if (transit_path_flag) {
					first_ptr = &node_access [access_itr->To_ID ()];

					to_ptr->To_List (first_ptr->To_List ());
					first_ptr->To_List (index);

					if (access_itr->Dir () != 1) {
						from_ptr->To_List (first_ptr->From_List ());
						first_ptr->From_List (index);
					}
				}
			}
		}
	}

	//---- link to location map ----

	link_location.resize (link_array.size (), -1);
	next_location.resize (location_array.size (), -1);

	Location_Itr loc_itr;

	for (index=0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, index++) {
		Show_Progress ();

		next_ptr = &next_location [index];
		link_ptr = &link_location [loc_itr->Link ()];

		*next_ptr = *link_ptr;
		*link_ptr = index;
	}

	//---- link to parking map ----

	if (drive_path_flag) {
		Link_Itr link_itr;

		link_parking.resize (link_array.size (), -1);
		next_parking.resize (parking_array.size (), -1);
		
		Parking_Itr park_itr;

		for (index=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, index++) {
			Show_Progress ();

			next_ptr = &next_parking [index];
			link_ptr = &link_parking [park_itr->Link ()];

			*next_ptr = *link_ptr;
			*link_ptr = index;
		}
		for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
			Show_Progress ();

			if (link_itr->Grade () != 0) {
				path_param.grade_flag = true;
				break;
			}
		}
	}

	//---- link to stop map ----

	if (transit_path_flag) {
		int route, stop;
		Line_Itr line_itr;
		Line_Stop_Itr line_stop_itr;
		Stop_Itr stop_itr;

		//---- build the list of routes serving a stop ----
		
		Route_Stop_Data route_stop, *rstop_ptr;

		stop_list.resize (stop_array.size (), -1);
		route_stop_array.resize (line_array.Route_Stops (), route_stop);

		for (index=route=0, line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++, route++) {
			Show_Progress ();

			for (stop=0, line_stop_itr = line_itr->begin (); line_stop_itr != line_itr->end (); line_stop_itr++, stop++, index++) {
				next_ptr = &stop_list [line_stop_itr->Stop ()];
				rstop_ptr = &route_stop_array [index];

				rstop_ptr->Route (route);
				rstop_ptr->Stop (stop);

				rstop_ptr->Next (*next_ptr);
				*next_ptr = index;
			}
		}

		//---- build the list of stops on a link ----

		link_stop.resize (link_array.size (), -1);
		next_stop.resize (stop_array.size (), -1);
		
		for (index=0, stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++, index++) {
			Show_Progress ();

			next_ptr = &next_stop [index];
			link_ptr = &link_stop [stop_itr->Link ()];

			*next_ptr = *link_ptr;
			*link_ptr = index;
		}

		//---- park ride lot locations ----

		if (parkride_path_flag) {
			Parking_Itr park_itr;
			Link_Data *link_ptr;
			Node_Data *node_ptr;
			Lot_XY_Data lot_rec;

			int i, ax, ay, bx, by;
			double factor;

			for (i=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, i++) {
				Show_Progress ();

				if (park_itr->Type () != PARKRIDE) continue;

				link_ptr = &link_array [park_itr->Link ()];

				node_ptr = &node_array [link_ptr->Anode ()];

				ax = node_ptr->X ();
				ay = node_ptr->Y ();

				node_ptr = &node_array [link_ptr->Bnode ()];

				bx = node_ptr->X ();
				by = node_ptr->Y ();

				factor = (double) park_itr->Offset () / (double) link_ptr->Length ();

				lot_rec.Lot (i);
				lot_rec.X (ax + (int) ((bx - ax) * factor + 0.5));
				lot_rec.Y (ay + (int) ((by - ay) * factor + 0.5));

				park_ride.push_back (lot_rec);
			}
			Print (2, "Number of Park-&-Ride Lots = ") << park_ride.size ();
		}

		//----- get the kiss-&-ride lots ----

		if (kissride_path_flag) {
			Parking_Itr park_itr;
			Link_Data *link_ptr;
			Node_Data *node_ptr;
			Stop_Itr stop_itr;
			Lot_XY_Data kiss_rec;

			int i, ax, ay, bx, by, distance;
			double factor;
			bool flag;

			for (i=0, park_itr = parking_array.begin (); park_itr != parking_array.end (); park_itr++, i++) {
				Show_Progress ();

				link_ptr = &link_array [park_itr->Link ()];

				node_ptr = &node_array [link_ptr->Anode ()];

				ax = node_ptr->X ();
				ay = node_ptr->Y ();

				node_ptr = &node_array [link_ptr->Bnode ()];

				bx = node_ptr->X ();
				by = node_ptr->Y ();

				factor = (double) park_itr->Offset () / (double) link_ptr->Length ();

				kiss_rec.Lot (i);
				kiss_rec.X (ax + (int) ((bx - ax) * factor + 0.5));
				kiss_rec.Y (ay + (int) ((by - ay) * factor + 0.5));
				flag = false;

				for (stop_itr = stop_array.begin (); stop_itr != stop_array.end (); stop_itr++) {
					if (!path_param.kissride_type [stop_itr->Type ()]) continue;

					link_ptr = &link_array [stop_itr->Link ()];

					node_ptr = &node_array [link_ptr->Anode ()];

					ax = node_ptr->X ();
					ay = node_ptr->Y ();

					node_ptr = &node_array [link_ptr->Bnode ()];

					bx = node_ptr->X ();
					by = node_ptr->Y ();

					factor = (double) stop_itr->Offset () / (double) link_ptr->Length ();

					ax += (int) ((bx - ax) * factor + 0.5) - kiss_rec.X ();
					ay += (int) ((by - ay) * factor + 0.5) - kiss_rec.Y ();

					distance = abs (ax) + abs (ay);

					if (Resolve (distance) < path_param.kissride_walk) {
						flag = true;
						break;
					}
				}
				if (flag) kiss_ride.push_back (kiss_rec);
			}
			Print (2, "Number of Kiss-&-Ride Lots = ") << kiss_ride.size ();
		}

		//---- stop penalty file ----

		if (path_param.stop_pen_flag) {
			Line_Data *line_ptr;
			Line_Run_Itr run_itr;

			int penalty, run, stop_field, pen_field, route_field, run_field, num_penalty, num_stops;
			
			stop_field = stop_pen_file.Required_Field ("STOP", "STOP_ID", "ID");
			pen_field = stop_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");
			route_field = stop_pen_file.Optional_Field ("ROUTE", "LINE", "ROUTE_ID", "LINE_ID");
			run_field = stop_pen_file.Optional_Field ("RUN", "TRIP");

			//---- process each stop penalty file record ----

			num_penalty = num_stops = 0;

			while (stop_pen_file.Read ()) {
				Show_Progress ();

				penalty = stop_pen_file.Get_Integer (pen_field);
				if (penalty <= 0) continue;
				penalty = Round (penalty);

				route = stop_pen_file.Get_Integer (route_field);
				run = stop_pen_file.Get_Integer (run_field);
				stop = stop_pen_file.Get_Integer (stop_field);

				if (stop > 0) {
					map_itr = stop_map.find (stop);

					if (map_itr == stop_map.end ()) {
						Warning (String ("Transit Penalty Stop %d was Not Found") % stop);
						continue;
					}
					stop = map_itr->second;
				} else {
					stop = -1;
				}
				if (route > 0) {
					map_itr = line_map.find (route);

					if (map_itr == line_map.end ()) {
						Warning (String ("Transit Penalty Route %d was Not Found") % route);
						continue;
					}
					line_ptr = &line_array [route];

					for (line_stop_itr = line_ptr->begin (); line_stop_itr != line_ptr->end (); line_stop_itr++) {
						if (stop < 0 || line_stop_itr->Stop () == stop) {
							if (run > 0) {
								if (run <= (int) line_stop_itr->size ()) {
									line_stop_itr->at (run - 1).Penalty (penalty);
									num_stops++;
								}
							} else {
								for (run_itr = line_stop_itr->begin (); run_itr != line_stop_itr->end (); run_itr++) {
									run_itr->Penalty (penalty);
									num_stops++;
								}
							}
						}
					}

				} else {
					for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
						for (line_stop_itr = line_itr->begin (); line_stop_itr != line_itr->end (); line_stop_itr++) {
							if (stop < 0 || line_stop_itr->Stop () == stop) {
								if (run > 0) {
									if (run <= (int) line_stop_itr->size ()) {
										line_stop_itr->at (run - 1).Penalty (penalty);
										num_stops++;
									}
								} else {
									for (run_itr = line_stop_itr->begin (); run_itr != line_stop_itr->end (); run_itr++) {
										run_itr->Penalty (penalty);
										num_stops++;
									}
								}
							}
						}
					}
				}
				num_penalty++;
			}
			Print (2, "Number of Transit Penalty Records = ") << num_penalty;
			Print (1, "Number of Route-Run-Stop Penalties = ") << num_stops;
		}
	}

	//---- parking penalty file ----

	if (path_param.park_pen_flag) {
		int parking, penalty, park_field, pen_field, num_penalty;

		park_field = park_pen_file.Required_Field ("PARKING", "LOT", "ID", "NUMBER", "LOT_ID");
		pen_field = park_pen_file.Required_Field ("PENALTY", "IMP", "IMPEDANCE", "IMPED", "PEN");

		//---- initialize the penalty field ----

		park_penalty.assign (parking_array.size (), 0);

		//---- process each parking penalty file record ----

		num_penalty = 0;

		while (park_pen_file.Read ()) {
			Show_Progress ();

			parking = park_pen_file.Get_Integer (park_field);
			if (parking <= 0) continue;

			map_itr = parking_map.find (parking);

			if (map_itr == parking_map.end ()) {
				Warning (String ("Parking Penalty Lot %d was Not Found") % parking);
				continue;
			}
			penalty = park_pen_file.Get_Integer (pen_field);
			if (penalty <= 0) continue;

			park_penalty [map_itr->second] = Round (penalty);
			num_penalty++;
		}
		Print (2, "Number of Parking Penalty Records = ") << num_penalty;
		park_pen_file.Close ();
	}
	End_Progress ();
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Router_Service::Page_Header (void)
{
	switch (Header_Number ()) {
		case SCRIPT_REPORT:		//---- Type Script ----
			Print (1, "Household Type Script");
			Print (1);
			break;
		case STACK_REPORT:		//---- Type Stack ----
			Print (1, "Household Type Stack");
			Print (1);
			break;
		default:
			break;
	}
}
