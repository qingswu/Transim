//*********************************************************
//	Read_Zones.cpp - read the zone file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool Data_Service::Get_Zone_Data (Zone_File &file, Zone_Data &zone_rec)
{
	//---- read and save the data ----

	zone_rec.Zone (file.Zone ());
	zone_rec.X (file.X ());
	zone_rec.Y (file.Y ());
	zone_rec.Z (file.Z ());
	zone_rec.Area_Type (file.Area_Type ());
	zone_rec.Notes (file.Notes ());

	return (true);
}
