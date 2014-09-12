//*********************************************************
//	Save_Route.cpp - save the route data
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Save_Route
//---------------------------------------------------------

void NetPrep::Save_Route (Route_Data &data)
{
	int i, num, node, type, a, b, count, error;
	String text;
	double freq1, freq2, avg_time;

	Route_Node route_node, *node_ptr;
	Route_Period route_period, *period_ptr;
	Route_Header route_rec, *route_ptr;
	Route_Nodes_Itr route_itr;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Int_Itr itr;
	Int2_Map_Itr ab_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	if (data.flip) {
		data.notes += " Flip";
	}

	if (oneway_link_flag) {
		a = 0;
		count = error = 0;
		for (itr = data.nodes.begin (); itr != data.nodes.end (); itr++, a=b) {
			b = node_array [abs (*itr)].Node ();

			if (a > 0) {
				ab_itr = ab_map.find (Int2_Key (a, b));

				if (ab_itr != ab_map.end ()) {
					dir_ptr = &dir_array [ab_itr->second];
					link_ptr = &link_array [dir_ptr->Link ()];

					if (link_ptr->AB_Dir () < 0 || link_ptr->BA_Dir () < 0) {
						count++;
					}
				} else {
					ab_itr = ab_map.find (Int2_Key (b, a));

					if (ab_itr != ab_map.end ()) {
						error++;
					}
				}
			}
		}
		if (count > 0 || error > 0) {
			Print (1, "Route=") << data.name << " includes " << count << " oneway links" << " and " << error << " direction errors";
		}
	}

	//---- match to existing lines ----

	for (route_itr = route_nodes_array.begin (); route_itr != route_nodes_array.end (); route_itr++) {

		if (!collapse_routes) continue;
		if (route_itr->Mode () != data.mode) continue;
		if (route_itr->nodes.size () != data.nodes.size ()) continue;

		num = (int) data.nodes.size () - 1;

		for (i=0; i <= num; i++) {
			node_ptr = &route_itr->nodes [i];
			if (data.flip) {
				node = data.nodes [num-i];
			} else {
				node = data.nodes [i];
			}
			if (node < 0) {
				node = -node;
				type = NO_STOP;
			} else {
				type = STOP;
			}
			if (node_ptr->Node () != node || node_ptr->Type () != type) break;
		}
		if (i > num) break;
	}

	//---- update existing line ----

	if (route_itr != route_nodes_array.end ()) {

		text = route_itr->Name ();

		if (text.find (data.name) == string::npos) {
			text += "|" + data.name;
			route_itr->Name (text);
		}
		text = route_itr->Notes ();
		text += "|" + data.notes;
		route_itr->Notes (text);

		for (i=0; i < num_periods; i++) {
			if (data.headway [i] > 0) {
				period_ptr = &route_itr->periods [i];
				if (period_ptr->Headway () > 0) {
					freq1 = 60.0 / period_ptr->Headway ();
					freq2 = 60.0 / data.headway [i];

					avg_time = data.ttime * freq2 + period_ptr->TTime () * freq1;
					freq2 += freq1;

					period_ptr->Headway ((int) (60.0 / freq2 + 0.5));
					period_ptr->TTime ((int) (avg_time / freq2 + 0.5));
				} else {
					period_ptr->Headway (data.headway [i]);
					period_ptr->TTime (data.ttime);
				}
				period_ptr->Offset (data.offset);
			}
		}
		num_match++;

	} else {

		//---- create a new line ----

		route_rec.Route (new_route++);
		num = (int) data.nodes.size ();

		route_rec.nodes.assign (num, route_node);
		route_rec.periods.assign (num_periods, route_period);
	
		map_stat = route_map.insert (Int_Map_Data (route_rec.Route (), (int) route_nodes_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Route Number = ") << route_rec.Route ();
			return;
		}
		route_nodes_array.push_back (route_rec);

		route_ptr = &route_nodes_array.back ();

		route_ptr->Name (data.name);
		route_ptr->Mode (data.mode);
		if (data.veh_type == 0) {
			route_ptr->Veh_Type (mode_type_map [data.mode]); 
		} else {
			route_ptr->Veh_Type (data.veh_type);
		}
		route_ptr->Notes (data.notes);

		for (i=0; i < num_periods; i++) {
			if (data.headway [i] > 0) {
				period_ptr = &route_ptr->periods [i];
				period_ptr->Headway (data.headway [i]);
				period_ptr->Offset (data.offset);
				period_ptr->TTime (data.ttime);
			}
		}
		num--;

		for (i=0; i <= num; i++) {
			node_ptr = &route_ptr->nodes [i];
			if (data.flip) {
				node = data.nodes [num-i];
			} else {
				node = data.nodes [i];
			}
			if (node < 0) {
				node = -node;
				type = NO_STOP;
			} else {
				type = STOP;
			}
			node_ptr->Node (node);
			node_ptr->Type (type);
		}
	}

	//---- split twoway lines ----

	if (!data.oneway) {

		data.notes += " Return";

		//---- match to existing lines ----

		for (route_itr = route_nodes_array.begin (); route_itr != route_nodes_array.end (); route_itr++) {

			if (route_itr->Mode () != data.mode) continue;

			num = (int) route_itr->nodes.size ();
			if (num != (int) data.nodes.size ()) continue;
			num--;

			for (i=0; i <= num; i++) {
				node_ptr = &route_itr->nodes [i];

				if (data.flip) {
					node = data.nodes [i];
				} else {
					node = data.nodes [num-i];
				}
				if (node < 0) {
					node = -node;
					type = NO_STOP;
				} else {
					type = STOP;
				}
				if (node_ptr->Node () != node || node_ptr->Type () != type) break;
			}
			if (i > num) break;
		}

		//---- update existing line ----

		if (route_itr != route_nodes_array.end ()) {
			text = route_itr->Name ();

			if (text.find (data.name) == string::npos) {
				text += "|" + data.name;
				route_itr->Name (text);
			}
			text = route_itr->Notes ();
			text += "|" + data.notes;
			route_itr->Notes (text);

			for (i=0; i < num_periods; i++) {
				if (data.headway [i] > 0) {
					period_ptr = &route_itr->periods [i];
					if (period_ptr->Headway () > 0) {
						freq1 = 60.0 / period_ptr->Headway ();
						freq2 = 60.0 / data.headway [i];

						avg_time = data.ttime * freq2 + period_ptr->TTime () * freq1;
						freq2 += freq1;

						period_ptr->Headway ((int) (60.0 / freq2 + 0.5));
						period_ptr->TTime ((int) (avg_time / freq2 + 0.5));
					} else {
						period_ptr->Headway (data.headway [i]);
						period_ptr->TTime (data.ttime);
					}
					period_ptr->Offset (data.offset);
				}
			}
			num_match++;

		} else {

			//---- create a new line ----

			route_rec.Route (new_route++);
			num = (int) data.nodes.size ();

			route_rec.nodes.assign (num, route_node);
			route_rec.periods.assign (num_periods, route_period);
	
			map_stat = route_map.insert (Int_Map_Data (route_rec.Route (), (int) route_nodes_array.size ()));

			if (!map_stat.second) {
				Warning ("Duplicate Route Number = ") << route_rec.Route ();
				return;
			}
			route_nodes_array.push_back (route_rec);

			route_ptr = &route_nodes_array.back ();

			route_ptr->Name (data.name);
			route_ptr->Mode (data.mode);
			if (data.veh_type == 0) {
				route_ptr->Veh_Type (mode_type_map [data.mode]);
			} else {
				route_ptr->Veh_Type (data.veh_type);
			}
			route_ptr->Notes (data.notes);

			for (i=0; i < num_periods; i++) {
				if (data.headway [i] > 0) {
					period_ptr = &route_ptr->periods [i];
					period_ptr->Headway (data.headway [i]);
					period_ptr->Offset (data.offset);
					period_ptr->TTime (data.ttime);
				}
			}
			num--;

			for (i=0; i <= num; i++) {
				node_ptr = &route_ptr->nodes [i];

				if (data.flip) {
					node = data.nodes [i];
				} else {
					node = data.nodes [num-i];
				}
				if (node < 0) {
					node = -node;
					type = NO_STOP;
				} else {
					type = STOP;
				}
				node_ptr->Node (node);
				node_ptr->Type (type);
			}
		}
	}
}
