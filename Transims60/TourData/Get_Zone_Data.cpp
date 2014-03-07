//*********************************************************
//	Get_Zone_Data.cpp - Read the Zone File
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool TourData::Get_Zone_Data (Zone_File &file, Zone_Data &data)
{
	if (Data_Service::Get_Zone_Data (file, data)) {
		int zone = data.Zone ();
		int group = file.Get_Integer (group_field);

		zone_group [zone] = group;
		group_zones [group].push_back (zone);

		if (attr_flag) {
			Table_Itr table_itr;

			for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
				if (table_itr->special >= 0) {
					table_itr->zone_wt [zone] = file.Get_Integer (table_itr->special);
				}
			}
		}
		return (true);
	}
	return (false);
}

