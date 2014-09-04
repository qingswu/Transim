//*********************************************************
//	Get_Zone_Data.cpp - read the zone file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool TransimsNet::Get_Zone_Data (Zone_File &file, Zone_Data &zone_rec)
{
	if (delete_zone_flag && delete_zone_range.In_Range (file.Zone ())) {
		xzone++;
		return (false);
	}
	if (zone_data_flag) {
		if (zone_data_array.Read_Record (file.Zone ())) {
			file.Copy_Fields (zone_data_array);
		}
	}
	return (Data_Service::Get_Zone_Data (file, zone_rec));
}
