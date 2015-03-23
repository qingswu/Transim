//*********************************************************
//	Read_Trips.cpp - Read the Trip File
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool Converge_Service::Get_Trip_Data (Trip_File &file, Trip_Data &trip_rec, int partition)
{
	if (Data_Service::Get_Trip_Data (file, trip_rec, partition)) {
		if (priority_flag) {
			trip_rec.Priority (initial_priority);
		}
		if (trip_rec.Household () > 0 && Selection (&trip_rec)) return (true);
	}
	return (false);
}
