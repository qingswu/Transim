//*********************************************************
//	Read_ParkRide.cpp - Read the Park and Ride File
//*********************************************************

#include "TransitNet.hpp"

//---------------------------------------------------------
//	Read_ParkRide
//---------------------------------------------------------

void TransitNet::Read_ParkRide (void)
{
	int node_field, capacity_field, hourly_field, daily_field, pnr_access, speed, pnr_index;
	int dir, node, capacity, hourly, daily, lot_offset, min_length, offset, type;
	bool walk_flag, stop_flag;
	double dx, dy, distance;
	Dtime time;
	String notes;

	Int_Map_Itr map_itr;
	Int_Map *stop_list;
	Dir_Data *dir_ptr, *best_dir;
	Link_Data *link_ptr;
	Parking_Nest park_nest;
	Parking_Data park_rec;
	Access_Data access_rec;
	Points points;
	XYZ_Point pt;
	Stop_Itr stop_itr;
	Point_Map_Itr pt_itr;

	//---- read the park and ride file ----

	Show_Message (String ("Reading %s -- Record") % parkride_file.File_Type ());
	Set_Progress ();
	Print (1);

	node_field = parkride_file.Required_Field ("NODE");
	capacity_field = parkride_file.Optional_Field ("CAPACITY", "PARKING", "SPACE", "SPACES");
	hourly_field = parkride_file.Optional_Field ("HOURLY", "RATE");
	daily_field = parkride_file.Optional_Field ("DAILY", "MAXIMUM");
	parkride_file.Notes_Field (parkride_file.Optional_Field (NOTES_FIELD_NAMES));

	lot_offset = Round (Internal_Units (30, METERS));
	min_length = lot_offset * 3 / 2;
	pnr_access = 0;
	speed = Round (Internal_Units (3.0, MPH));

	while (parkride_file.Read ()) {
		Show_Progress ();

		node = parkride_file.Get_Integer (node_field);
		if (node == 0) continue;

		map_itr = node_map.find (node);
		if (map_itr == node_map.end ()) {
			Warning (String ("Park&Ride Node %d was Not Found in the Node File") % node);
			parking_warnings++;
			continue;
		}

		capacity = parkride_file.Get_Integer (capacity_field);
		hourly = parkride_file.Get_Integer (hourly_field);
		daily = parkride_file.Get_Integer (daily_field);

		notes = parkride_file.Notes ();
		if (notes.empty ()) notes = "Park&Ride Lot";

		//---- select the best link ----

		offset = 0;
		best_dir = 0;
		walk_flag = stop_flag = false;

		for (dir = node_list [map_itr->second]; dir >= 0; dir = dir_list [dir]) {
			dir_ptr = &dir_array [dir];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Length () < min_length) continue;

			type = link_ptr->Type ();
			if (type == FREEWAY || type == BRIDGE || type == TUNNEL || type == HEAVYRAIL || type == FERRY) continue;

			if (!Use_Permission (link_ptr->Use (), CAR)) continue;

			stop_list = &dir_stop_array [dir];

			map_itr = stop_list->begin ();

			if (map_itr != stop_list->end ()) {
				if (best_dir == 0 || !stop_flag || map_itr->first < offset ||
					(!walk_flag && Use_Permission (link_ptr->Use (), WALK))) {
					best_dir = dir_ptr;
					offset = map_itr->first;
					walk_flag = Use_Permission (link_ptr->Use (), WALK);
					stop_flag = true;
				}
			} else if (best_dir == 0 || (!walk_flag && Use_Permission (link_ptr->Use (), WALK))) {
				best_dir = dir_ptr;
				offset = lot_offset;
				walk_flag = Use_Permission (link_ptr->Use (), WALK);
				stop_flag = false;
			}
		}
		if (best_dir == 0) {
			Warning (String ("Park&Ride Node %d does not have Auto Access") % node);
			parking_warnings++;
			continue;
		}
		nparkride++;

		//---- insert the parking record ----

		park_rec.Clear ();

		park_rec.Parking (++max_parking);
		park_rec.Link_Dir (best_dir->Link_Dir ());
		if (offset < lot_offset) offset = lot_offset;
		park_rec.Offset (offset);
		park_rec.Type (PARKRIDE);

		park_nest.Clear ();

		if (capacity > 0 || hourly > 0 || daily > 0) {
			park_nest.Use (CAR);
			park_nest.Start (Model_Start_Time ());
			park_nest.End (Model_End_Time ());
			park_nest.Space (capacity);
			park_nest.Time_In (0);
			park_nest.Time_Out (0);
			park_nest.Hourly (hourly);
			park_nest.Daily (daily);

			park_rec.push_back (park_nest);
		}
		pnr_index = (int) parking_array.size ();
		parking_map.insert (Int_Map_Data (park_rec.Parking (), pnr_index));
		parking_array.push_back (park_rec);
		nparking++;

		if (access_flag && PNR_distance > 0) {
			link_ptr = &link_array [park_rec.Link ()];

			Link_Shape (link_ptr, park_rec.Dir (), points, UnRound (offset), 0.0);

			pt = points [0];

			for (pt_itr = stop_pt.begin (); pt_itr != stop_pt.end (); pt_itr++) {
				dx = pt_itr->second.x - pt.x;
				dy = pt_itr->second.y - pt.y;

				distance = sqrt (dx * dx + dy * dy);

				if (distance <= PNR_distance) {
					map_itr = stop_map.find (pt_itr->first);

					if (map_itr != stop_map.end ()) {
						time.Seconds (Round (distance) / speed);

						access_rec.Link (++max_access);
						access_rec.From_Type (PARKING_ID);
						access_rec.From_ID (pnr_index);
						access_rec.To_Type (STOP_ID);
						access_rec.To_ID (map_itr->second);
						access_rec.Dir (2);
						access_rec.Time (time);
						access_rec.Cost (0);
						access_rec.Notes ("PNR Access");

						access_map.insert (Int_Map_Data (access_rec.Link (), (int) access_array.size ()));
						access_array.push_back (access_rec);
						pnr_access++;
						naccess++;
					}
				}
			}
		}
	}
	End_Progress ();

	Print (1, "Number of Park and Ride Records = ") << nparkride;

	if (access_flag && PNR_distance > 0) {
		Print (1, "Number of Park and Ride Access Links = ") << pnr_access;
	}
}
