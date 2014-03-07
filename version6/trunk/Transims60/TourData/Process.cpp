//*********************************************************
//	Process.cpp - Move Tour Destination
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Process
//---------------------------------------------------------

bool TourData::Process (int org, int &des, int &point, Table_Itr table_itr)
{
	int org_group, des_group, org_index, des_index, attr, new_des;
	double share, weight, cum_wt, distance, distance2, dist;
	Doubles des_wt, *dist_ptr;
	Dbl_Itr wt_itr;
	Integers *zone_ptr;
	Int_Itr zone_itr;
	Int_Map_Itr map_itr;

	if (org < 1 || org > max_zone || des < 1 || des > max_zone) return (false);

	org_group = zone_group [org];
	des_group = zone_group [des];

	map_itr = group_map.find (org_group);
	if (map_itr == group_map.end ()) return (false);
	org_index = map_itr->second;

	map_itr = group_map.find (des_group);
	if (map_itr == group_map.end ()) return (false);
	des_index = map_itr->second;

	share = table_itr->tours [org_index] [des_index];

	if (share <= 0.0) return (false);

	if (random.Probability () > share) return (false);

	//---- find destination options ----

	des_wt.assign (max_zone+1, 0.0);
	dist_ptr = &od_distance [org];
	distance = dist_ptr->at (des);
	distance2 = 2.0 * distance;
	cum_wt = 0.0;

	for (map_itr = group_map.begin (); map_itr != group_map.end (); map_itr++) {
		if (map_itr->first == des_group) continue;

		des_index = map_itr->second;

		share = table_itr->tours [org_index] [des_index];
		if (share >= 0.0) continue;

		share = -share;
		zone_ptr = &group_zones [map_itr->first];

		for (zone_itr = zone_ptr->begin (); zone_itr != zone_ptr->end (); zone_itr++) {
			attr = table_itr->zone_wt [*zone_itr];
			if (attr == 0) continue;

			dist = dist_ptr->at (*zone_itr);
			if (dist >= distance2) continue;

			weight = ((distance - fabs (dist - distance)) / distance_factor) + (attr / attraction_factor);
			weight *= share;

			des_wt [*zone_itr] = weight;
			cum_wt += weight;
		}
	}
	if (cum_wt == 0.0) return (false);

	weight = cum_wt * random_select.Probability ();
	cum_wt = 0.0;

	for (new_des=0, wt_itr = des_wt.begin (); wt_itr != des_wt.end (); wt_itr++, new_des++) {
		cum_wt += *wt_itr;
		if (cum_wt >= weight) break;
	}
	if (new_des < 1 || new_des > max_zone) return (false);

	zone_ptr = &table_itr->zone_pts [new_des];

	org_index = (int) zone_ptr->size ();
	des_index = (int) (org_index * random_point.Probability ());

	if (des_index >= org_index) des_index = org_index - 1; 
	if (des_index < 0) return (false);

	point = zone_ptr->at (des_index);
	des = new_des;

	if (stats_flag) {
		Stats_Index index;
		Stats_Map_Itr itr;
		Group_Stats *data;

		index.origin = org_group;
		index.destination = des_group;
		index.type = table_itr->group;

		itr = group_stats.find (index);

		if (itr != group_stats.end ()) {
			data = &itr->second;
			dist = dist_ptr->at (new_des);

			data->moved++;
			data->out_len += dist;
			data->in_len += distance;
			data->new_des.insert (zone_group [new_des]);
		}
	}
	return (true);
}

