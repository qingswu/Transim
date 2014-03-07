//*********************************************************
//	Read_Stations.cpp - read station node file
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Stations
//---------------------------------------------------------

void RoutePrep::Read_Stations (void)
{
	int node, link;
	double x, y, len, length, dx, dy;

	Int_Map_Itr int_itr;
	Link_Itr link_itr;
	Node_Itr node_itr;
	Station_Data station_data;
	Station_Map_Stat station_stat;
	Station_Map_Itr station_itr;

	//---- read station node file----

	Show_Message (String ("Reading %s -- Record") % station_file.File_Type ());
	Set_Progress ();

	while (station_file.Read ()) {
		Show_Progress ();

		if (station_node_field >= 0) {
			node = station_file.Get_Integer (station_node_field);
		} else {
			x = Round (station_file.Get_Double (station_x_field));
			y = Round (station_file.Get_Double (station_y_field));

			len = resolution;
			node = 0;

			for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
				dx = node_itr->X () - x;
				dy = node_itr->Y () - y;
				length = sqrt (dx * dx + dy * dy);

				if (length < len) {
					len = length;
					node = node_itr->Node ();
					if (len == 0.0) break;
				}
			}
			if (node == 0) continue;
		}
		int_itr = node_map.find (node);
		if (int_itr == node_map.end ()) continue;
		node = int_itr->second;

		station_data.name = station_file.Get_String (station_name_field);
		station_data.name.Trim ();

		station_stat = station_map.insert (Station_Map_Data (node, station_data));

		if (!station_stat.second) {
			Error ("Inserting Station Node Data");
		}
	}
	End_Progress ();

	station_file.Close ();

	Print (2, String ("Number of %s Records = %d") % station_file.File_Type () % Progress_Count ());
	if ((int) station_map.size () < Progress_Count ()) {
		Print (1, "Number of Station Nodes Found = ") << station_map.size ();
	}

	//---- find links attached to stations ----

	for (link=0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {

		//---- check for a station at the anode ----

		station_itr = station_map.find (link_itr->Anode ());
		if (station_itr != station_map.end ()) {
			station_itr->second.links.push_back (link);
		}

		//---- check fors a station at the bnode ----

		station_itr = station_map.find (link_itr->Bnode ());
		if (station_itr != station_map.end ()) {
			station_itr->second.links.push_back (link);
		}
	}

	//---- insert station data ----

	Insert_Stations ();
}
