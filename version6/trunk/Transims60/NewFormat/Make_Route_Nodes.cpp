//*********************************************************
//	Make_Route_Nodes.cpp - make transit route nodes
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Make_Route_Nodes
//---------------------------------------------------------

void NewFormat::Make_Route_Nodes (void)
{
	int period, num_periods, count, anode, bnode;
	Dtime start, end, time, offset;
	bool first, found;

	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Route_Header route_rec;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Route_Period period_rec;
	Route_Node node_rec;
	Driver_Itr driver_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Stop_Data *stop_ptr;

	//---- process the route header file ----

	Show_Message ("Making Route Nodes -- Record");
	Set_Progress ();

	num_periods = sum_periods.Num_Periods ();
	if (num_periods < 1) num_periods = 1;

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		line_ptr = &line_array [map_itr->second];

		route_rec.Clear ();
		route_rec.Route (line_ptr->Route ());
		route_rec.Mode (line_ptr->Mode ());
		route_rec.Name (line_ptr->Name ());
		route_rec.Notes (line_ptr->Notes ());
		route_rec.Veh_Type (line_ptr->Type ());

		stop_itr = line_ptr->begin ();

		for (period = 0; period < num_periods; period++) {
			period_rec.Clear ();

			sum_periods.Period_Range (period, start, end);
			offset = end;
			count = 0;

			for (run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++) {
				time = run_itr->Schedule ();

				if (time >= end) break;

				if (time >= start) {
					if (time < offset) offset = time;
					count++;
				}
			}
			if (count == 0) {
				period_rec.Headway (0);
				period_rec.Offset (0);
			} else {
				time = end - start;

				period_rec.Headway (time / count);
				period_rec.Offset (offset - start);
			}
			route_rec.periods.push_back (period_rec);
		}

		//---- process each link ----

		first = true;

		for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {
			dir_ptr = &dir_array [*driver_itr];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (dir_ptr->Dir () == 1) {
				anode = link_ptr->Bnode ();
				bnode = link_ptr->Anode ();
			} else {
				anode = link_ptr->Anode ();
				bnode = link_ptr->Bnode ();
			}
			found = false;

			for (stop_itr = line_ptr->begin(); stop_itr != line_ptr->end (); stop_itr++) {
				stop_ptr = &stop_array [stop_itr->Stop ()];

				if (stop_ptr->Link_Dir () == dir_ptr->Link_Dir ()) {
					found = true;
					break;
				}
			}
			if (first) {
				if (!found) continue;
				first = false;
				node_rec.Node (anode);
				node_rec.Type (STOP);
				route_rec.nodes.push_back (node_rec);
			}
			node_rec.Node (bnode);
			if (found) {
				node_rec.Type (STOP);
			} else {
				node_rec.Type (NO_STOP);
			}
			route_rec.nodes.push_back (node_rec);
		}

		//---- save the route record ----

		map_stat = route_map.insert (Int_Map_Data (route_rec.Route (), (int) route_nodes_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Route Number = ") << route_rec.Route ();
		} else {
			route_nodes_array.push_back (route_rec);
		}
	}
	End_Progress ();
}
