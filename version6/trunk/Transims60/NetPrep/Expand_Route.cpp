//*********************************************************
//	Expand_Route.cpp - expand the route data
//*********************************************************

#include "NetPrep.hpp"

//---------------------------------------------------------
//	Expand_Route
//---------------------------------------------------------

void NetPrep::Expand_Routes (void)
{
	int in, out, route, nroute, max_route, node, in_periods, out_periods;
	double freq1, freq2, avg_time, factor;
	bool flip_flag;

	Route_Node route_node, *node_ptr;
	Route_Period route_period, *in_ptr, *out_ptr;
	Route_Header *route_ptr, flip_rec, *header_ptr;
	Route_Period_Array in_period_array, out_period_array;
	Expand_Group_Itr group_itr;
	Int_Map_Itr map_itr;
	Int_Itr itr;

	nroute = (int) route_nodes_array.size ();
	out_periods = transit_time_periods.Num_Periods ();
	out_period_array.assign (out_periods, route_period);

	map_itr = --route_map.end ();
	max_route = (((map_itr->first + 50) / 100) + 1) * 100;

	for (route=0; route <= nroute; route++) {
		route_ptr = &route_nodes_array [route];

		in_period_array = route_ptr->periods;
		in_periods = (int) in_period_array.size ();
				
		route_ptr->periods = out_period_array;

		flip_rec.Clear ();
		flip_flag = false;

		for (group_itr = expand_groups.begin (); group_itr != expand_groups.end (); group_itr++) {
			if (group_itr->flip) {
				if (!flip_flag) {
					flip_flag = true;
					flip_rec.Route (max_route++);
					flip_rec.Mode (route_ptr->Mode ());
					flip_rec.Veh_Type (route_ptr->Veh_Type ());
					flip_rec.Oneway (route_ptr->Oneway ());
					flip_rec.Name (route_ptr->Name () + "**");
					flip_rec.Notes (route_ptr->Notes () + "Flip");

					flip_rec.periods = out_period_array;

					for (node = (int) route_ptr->nodes.size () - 1; node >= 0; node--) {
						node_ptr = &route_ptr->nodes [node];
						flip_rec.nodes.push_back (*node_ptr);
					}
				}
				header_ptr = &flip_rec;
			} else {
				header_ptr = route_ptr;
			}
			for (out=1; out <= out_periods; out++) {
				in = group_itr->period_map.Best (out);
				factor = group_itr->period_fac.Best (out);
				if (factor == 0.0) factor = 1.0;

				if (in > 0 && in <= in_periods) {
					in_ptr = &in_period_array [in-1];
					out_ptr = &header_ptr->periods [out-1];

					if (out_ptr->Headway () > 0) {
						freq1 = 60.0 / (in_ptr->Headway () * factor);
						freq2 = 60.0 / out_ptr->Headway ();

						avg_time = out_ptr->TTime () * freq2 + in_ptr->TTime () * freq1;
						freq2 += freq1;

						out_ptr->Headway ((int) (60.0 / freq2 + 0.5));
						out_ptr->TTime ((int) (avg_time / freq2 + 0.5));
					} else {
						out_ptr->Headway (in_ptr->Headway () * factor);
						out_ptr->TTime (in_ptr->TTime ());
					}
				}
			}
		}
		if (flip_flag) {
			route_map.insert (Int_Map_Data (flip_rec.Route (), (int) route_map.size ()));
			route_nodes_array.push_back (flip_rec);
		}
	}
}
