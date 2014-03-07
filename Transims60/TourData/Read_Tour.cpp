//*********************************************************
//	Read_Tour.cpp - Read the Tour File
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Read_Tour
//---------------------------------------------------------

void TourData::Read_Tour ()
{
	int purpose, type, income, point, org, des, org_group, des_group, org_index, des_index;
	double target, base, diff, future, share;
	Table_Itr table_itr;
	Int_Map_Itr org_itr, des_itr;
	Stats_Index index;
	Group_Stats stats;

	Show_Message (String ("Reading %s -- Record") % tour_file.File_Type ());
	Set_Progress ();

	//---- process each tour record ----

	while (tour_file.Read ()) {
		Show_Progress ();

		org = tour_file.Get_Integer (org_field);
		des = tour_file.Get_Integer (des_field);
		if (org < 1 || org > max_zone || des < 0 || des > max_zone) continue;

		point = tour_file.Get_Integer (point_field);
		purpose = tour_file.Get_Integer (purpose_field);
		type = tour_file.Get_Integer (type_field);
		income = tour_file.Get_Integer (income_field);

		org_group = zone_group [org];

		org_itr = group_map.find (org_group);
		if (org_itr == group_map.end ()) continue;
		org_index = org_itr->second;

		des_group = zone_group [des];

		des_itr = group_map.find (des_group);
		if (des_itr == group_map.end ()) continue;
		des_index = des_itr->second;

		for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
			if (table_itr->purpose.In_Range (purpose) && table_itr->type.In_Range (type) &&
				table_itr->income.In_Range (income)) {

				table_itr->zone_wt [des]++;
				table_itr->zone_pts [des].push_back (point);
				table_itr->tours [org_index] [des_index]++;
				break;
			}
		}
	}
	End_Progress ();

	tour_file.Rewind ();

	Print (2, String ("Number of %s Records = %d") % tour_file.File_Type () % Progress_Count ());

	if (stats_flag) {
		stats.moved = 0;
		stats.in_len = stats.out_len = 0;
		stats.new_des.clear ();
	}

	//---- calculate the correction factors ----
	
	for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {

		for (org_itr = group_map.begin (); org_itr != group_map.end (); org_itr++) {
			org_group = org_itr->first;
			org_index = org_itr->second;

			for (des_itr = group_map.begin (); des_itr != group_map.end (); des_itr++) {
				des_group = des_itr->first;
				des_index = des_itr->second;

				target_matrix->Get_Cell (0, org_group, des_group, table_itr->target, target);

				base_matrix->Get_Cell (0, org_group, des_group, table_itr->base, base);

				future = table_itr->tours [org_index] [des_index];

				if (future > 0 && base > 0 && target > 0) {
					diff = fabs (base - target);
					share = diff / base;
					diff = (diff + share * future) / 2.0;
					share = diff / future;

					if (base < target) {
						share = -share;
					} else if (share > 0 && stats_flag) {
						index.origin = org_group;
						index.destination = des_group;
						index.type = table_itr->group;
						stats.input = DTOI (future);
						stats.target = DTOI (diff);

						group_stats.insert (Stats_Map_Data (index, stats));
					}
				} else {
					share = 0.0;
				}
				table_itr->tours [org_index] [des_index] = share;
			}
		}
	}
}

