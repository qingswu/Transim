//*********************************************************
//	Coordinate.cpp - Coordinate Schedules
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Coordinate_Schedules
//---------------------------------------------------------

void TransitNet::Coordinate_Schedules (void)
{
	int r, r0, n0, route, max_n, index;
	double factor, factor1;
	bool flag;

	Route_Nodes_Itr route_itr, rt_itr; 
	Route_Node_Itr node_itr, nod_itr, nod0_itr;
	Route_Period_Itr period_itr;
	Route_Period *period_ptr;
	Int_Map_Itr map_itr;
	Line_Itr line_itr;
	Line_Data *line_ptr, *line2_ptr;
	Line_Stop_Itr stop_itr, st_itr, st0_itr;

	Dtime hour, time, time1, offset;
	Doubles num_runs;
	Dbl_Itr run_itr;
	Dtimes stop_times;

	max_n = 8 * sizeof (int);
	hour.Hours (1.0);

	//---- add schedule data ----

	Show_Message ("Coordinating Schedules -- Record");
	Set_Progress ();

	for (r0=0, route_itr = route_nodes_array.begin (); route_itr != route_nodes_array.end (); route_itr++, r0++) {
		Show_Progress ();
		route = route_itr->Route ();
		
		map_itr = line_map.find (route);
		if (map_itr == line_map.end ()) continue;
		line_ptr = &line_array [map_itr->second];

		for (n0=0, period_itr = route_itr->periods.begin (); period_itr != route_itr->periods.end (); period_itr++, n0++) {
			if (period_itr->Headway () <= 0) continue;

			if (period_itr->TTime () > 0) {
				stop_itr = --line_ptr->end ();
				factor = (double) period_itr->TTime () / stop_itr->Time ();
			} else {
				factor = 1.0;
			}
			if (period_itr->Offset () < 0) {
				period_itr->Offset (0);
			}

			//---- search for common stops on other lines ----

			for (r=r0+1, rt_itr = route_itr + 1; rt_itr != route_nodes_array.end (); rt_itr++, r++) {
				period_ptr = &rt_itr->periods [n0];

				if (period_ptr->Headway () <= 0) continue;
				if (period_ptr->Offset () > 0) continue;

				map_itr = line_map.find (rt_itr->Route ());
				if (map_itr == line_map.end ()) continue;
				line2_ptr = &line_array [map_itr->second];

				if (period_ptr->TTime () > 0) {
					st_itr = --line2_ptr->end ();
					factor1 = (double) period_ptr->TTime () / st_itr->Time ();
				} else {
					factor1 = 1.0;
				}
				flag = false;

				for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {	
					for (st_itr = line2_ptr->begin (); st_itr != line2_ptr->end (); st_itr++) {
						if (stop_itr->Stop () == st_itr->Stop ()) {
							time = stop_itr->Time () * factor;
							time1 = st_itr->Time () * factor1;
							offset = period_itr->Offset () + time - time1;

							if (period_itr->Pattern () == 0) {
								index = 0;
							} else {
								index = period_itr->Index ();
							}
							if (period_itr->Pattern () == 0 || period_ptr->Pattern () == 0) {
								offset = offset + period_itr->Headway () / 2;
							}
							while (offset < 0) {
								offset = offset + period_ptr->Headway ();
								index++;
							}
							while (offset > period_ptr->Headway ()) {
								if (index == 0) {
									index = period_ptr->Count ();
								}
								offset = offset - period_ptr->Headway ();
								index--;
							}
							period_ptr->Offset (offset);
							period_ptr->Index (index);
							flag = true;
							break;
						}
					}
					if (flag) break;
				}
			}
		}
	}
	End_Progress ();
}
