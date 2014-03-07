//*********************************************************
//	Write_Route.cpp - write the route data
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Write_Route
//---------------------------------------------------------

void RoutePrep::Write_Route (Db_File *file, Route_Data &data)
{
	int count, i, n, n2, segment, node, last_node;
	double speed, factor;
	bool speed_flag, n_flag, node_flag, skip;

	Int_Itr itr, itr2, int_itr, int2_itr;
	Int_RItr int_ritr, int2_ritr;
	Dtime_Itr dtime;
	Ints_Map_Itr ints_map_itr;
	Integers *list_ptr;
	Int_Dbl_Map_Itr int_dbl_map_itr;

	fstream &fh = file->File ();

	//---- update the route nodes ----

	node_flag = System_File_Flag (NODE);
	speed_flag = (data.speeds.size () > 0);

	if (System_File_Flag (LINK) && !speed_flag) {
		for (ints_map_itr = segments.begin (); ints_map_itr != segments.end (); ints_map_itr++) {
			segment = ints_map_itr->first;
			list_ptr = &ints_map_itr->second;

			int_dbl_map_itr = segment_factors.find (segment);

			if (int_dbl_map_itr == segment_factors.end ()) {
				factor = 1.0;
			} else {
				factor = int_dbl_map_itr->second;
			}
			for (n=0, itr = data.nodes.begin (); itr != data.nodes.end (); itr++, n++) {
				node = abs (*itr);
			
				for (int_itr = list_ptr->begin (); int_itr != list_ptr->end (); int_itr++) {
					if (node == *int_itr) {
						for (n2 = n, itr2 = itr + 1, int2_itr = int_itr + 1; itr2 != data.nodes.end () && int2_itr != list_ptr->end (); itr2++, int2_itr++) {
							if (abs (*itr2) != *int2_itr) break;
							n2++;
						}
						if (n == n2) break;

						if (data.speeds.size () == 0) {
							data.speeds.assign (data.nodes.size (), 0.0);
						}
						speed = (data.headway [0] > 0) ? data.peak : data.offpeak;

						data.speeds [n] = speed * factor;
						if (data.speeds [n2] == 0.0) data.speeds [n2] = speed;
						n = n2;
						itr = itr2;
						break;
					}
				}
				if (itr == data.nodes.end ()) break;

				//---- check the reverse direction ----

				for (int_ritr = list_ptr->rbegin (); int_ritr != list_ptr->rend (); int_ritr++) {
					if (node == *int_ritr) {
						for (n2 = n, itr2 = itr + 1, int2_ritr = int_ritr + 1; itr2 != data.nodes.end () && int2_ritr != list_ptr->rend (); itr2++, int2_ritr++) {
							if (abs (*itr2) != *int2_ritr) break;
							n2++;
						}
						if (n == n2) break;

						if (data.speeds.size () == 0) {
							data.speeds.assign (data.nodes.size (), 0.0);
						}
						speed = (data.headway [0] > 0) ? data.peak : data.offpeak;

						data.speeds [n] = speed * factor;
						if (data.speeds [n2] == 0.0) data.speeds [n2] = speed;
						n = n2;
						itr = itr2;
						break;
					}
				}
				if (itr == data.nodes.end ()) break;
			}
		}
		speed_flag = (data.speeds.size () > 0);
	}
	n_flag = false;

	fh << "LINE NAME=\"" << data.name << "\"," << endl;
	fh << "     OWNER=\"" << data.notes << "\"," << endl;
	fh << "     ONEWAY=" << ((data.oneway) ? "T" : "F") << ", MODE=" << data.mode;

	if (!speed_flag && data.ttime > 0) {
		fh << ", RUNTIME=" << data.ttime.Minutes ();
	}
	
	for (i=1, dtime = data.headway.begin (); dtime != data.headway.end (); dtime++) {
		if (*dtime > 0) {
			fh << ", FREQ[" << i << "]=" << dtime->Minutes ();
			i++;
		}
	}
	fh << ", " << endl;

	skip = false;
	count = last_node = 0;
	fh << "     N=";

	for (i=0, itr = data.nodes.begin (); itr != data.nodes.end (); itr++, i++) {
		if (count > 0) {
			if (!skip) {
				if (count == 8) {
					fh << "," << endl << "     ";
					count = 0;
				} else {
					fh << ", ";
				}
			}
		}
		skip = false;

		if (n_flag) {
			fh << "N=";
			n_flag = false;
		}
		if (node_flag) {
			if (*itr < 0) {
				node = -node_array [-(*itr)].Node ();
			} else {
				node = node_array [*itr].Node ();
			}
			if (abs (node) == last_node) {
				skip = true;
				continue;
			}
			last_node = abs (node);

			fh << node;
		} else {
			fh << *itr;
		}
		count++;

		if (speed_flag && i < (int) data.speeds.size ()) {
			speed = data.speeds [i];
			if (speed > 0) {
				if (count == 8) {
					fh << "," << endl << "     ";
					count = 0;
				} else {
					fh << ", ";
				}
				fh << (String ("SPEED=%.2lf") % speed);
				n_flag = true;
				count++;
			} else if (i == 0 && route_speed_flag) {
				speed = (data.headway [0] > 0) ? data.peak : data.offpeak;
				if (speed == 0) {
					Warning (String ("Default Speed for Line %s is Zero") % data.name);
				} else {
					fh << (String (", SPEED=%.2lf") % speed);
					n_flag = true;
					count++;
				}
			}
		}
	}
	fh << endl;
}
