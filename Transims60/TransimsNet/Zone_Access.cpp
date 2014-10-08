//*********************************************************
//	Zone_Access.cpp - add centroid locations and parking
//*********************************************************

#include "TransimsNet.hpp"

#include <math.h>

//---------------------------------------------------------
//	Zone_Access
//---------------------------------------------------------

void TransimsNet::Zone_Access (void)
{
	int j, link, min_len, area_type, zone, zone_num, location, parking;
	bool ab_flag, dir_flag;

	Link_Itr link_itr;
	Node_Data *node_ptr;
	Location_Data loc_rec;
	Parking_Data park_rec;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Park_Detail_Itr detail_itr;
	Parking_Nest park_nest;
	Zone_Itr zone_itr;
	Access_Data access_rec;

	naccess = (int) access_map.size ();

	naccess = (int) access_array.size ();
	if (naccess > 0) {
		Int_Map_Itr map_itr = --access_map.end ();
		naccess = ((map_itr->first / 10) + 1) * 10 - 1;
	}

	Show_Message (String ("Creating Zone Access Points -- Record"));
	Set_Progress ();

	for (link = 0, link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++, link++) {
		Show_Progress ();

		if (link_itr->Type () != EXTERNAL) continue;
		area_type = link_itr->Area_Type ();

		loc_rec.Link (link);
		loc_rec.Setback (loc_setback);

		park_rec.Link (link);

		min_len = external_offset;
		if (link_itr->Length () < external_offset * 2) min_len = link_itr->Length () / 2;

		node_ptr = &node_array [link_itr->Anode ()];
		zone = node_ptr->Node ();

		node_ptr = &node_array [link_itr->Bnode ()];

		if (zone < node_ptr->Node ()) {
			ab_flag = true;
		} else {
			ab_flag = false;
			zone = node_ptr->Node ();
		}
		map_itr = zone_map.find (zone);
		if (map_itr == zone_map.end ()) {
			Warning (String ("External Link %d Zone %d is Out of Range") % link_itr->Link () % zone);
			zone_num = -1;
		} else {
			zone_num = map_itr->second;
		}
		park_rec.Type (BOUNDARY);
		park_rec.Notes ("External Station");
		dir_flag = true;

		//---- insert activity locations ----

		map_itr = zone_centroid.find (zone);

		if (map_itr == zone_centroid.end ()) {
			loc_rec.Location (++location_id);

			location = (int) location_array.size ();

			zone_centroid.insert (Int_Map_Data (zone, location));

			if (ab_flag) {
				loc_rec.Dir (0);
			} else {
				loc_rec.Dir (1);
			}
			loc_rec.Offset (min_len);
			loc_rec.Notes ("External");
			loc_rec.Zone (zone_num);

			//---- insert the location record ----

			map_stat = location_map.insert (Int_Map_Data (location_id, location));

			if (!map_stat.second) continue;

			location_array.push_back (loc_rec);

		} else {
			location = map_itr->second;
		}

		for (j=0; j < 2; j++) {
			if (j == 0) {
				if (dir_flag && link_itr->AB_Dir () < 0) continue;

				if (ab_flag) {
					park_rec.Offset (min_len);
				} else {
					park_rec.Offset (link_itr->Length () - min_len);
				}
			} else {
				if (dir_flag && link_itr->BA_Dir () < 0) continue;

				if (ab_flag) {
					park_rec.Offset (link_itr->Length () - min_len);
				} else {
					park_rec.Offset (min_len);
				}
			}

			//---- add a parking lot ----

			park_rec.Parking (++parking_id);

			parking = (int) parking_array.size ();

			park_rec.Dir (j);

			map_stat = parking_map.insert (Int_Map_Data (parking_id, parking));

			if (!map_stat.second) continue;

			if (details_flag) {
				park_rec.clear ();

				for (detail_itr = parking_details.begin (); detail_itr != parking_details.end (); detail_itr++) {
					if (detail_itr->Area_Type1 () <= area_type && area_type <= detail_itr->Area_Type2 ()) {
						park_nest.Start (detail_itr->Start ());
						park_nest.End (detail_itr->End ());
						park_nest.Use (detail_itr->Use ());
						park_nest.Time_In (detail_itr->Time_In ());
						park_nest.Time_Out (detail_itr->Time_Out ());
						park_nest.Hourly (detail_itr->Hourly ());
						park_nest.Daily (detail_itr->Daily ());

						park_rec.push_back (park_nest);
					}
				}
			}
			parking_array.push_back (park_rec);

			//---- add the access link ----

			access_rec.Link (++naccess);
			access_rec.From_Type (LOCATION_ID);
			access_rec.From_ID (location);
			access_rec.To_Type (PARKING_ID);
			access_rec.To_ID (parking);
			access_rec.Dir (2);
			access_rec.Time (10);
			access_rec.Cost (0);
			access_rec.Notes ("Zone Access");

			access_map.insert (Int_Map_Data (access_rec.Link (), (int) access_array.size ()));
			access_array.push_back (access_rec);
		}
	}
	End_Progress ();
}
