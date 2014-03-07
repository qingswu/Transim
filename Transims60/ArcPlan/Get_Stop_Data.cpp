//*********************************************************
//	Get_Stop_Data.cpp - additional transit stop processing
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Get_Stop_Data
//---------------------------------------------------------

bool ArcPlan::Get_Stop_Data (Stop_File &file, Stop_Data &stop_rec)
{
	Point_Map_Stat map_stat;

	if (Data_Service::Get_Stop_Data (file, stop_rec)) {
		Point_Offset (stop_rec.Link (), stop_rec.Dir (), stop_rec.Offset (), stop_side, points);

		//---- save the location point data ----

		map_stat = stop_pt.insert (Point_Map_Data (stop_rec.Stop (), points [0]));

		if (!map_stat.second) {
			Warning ("Duplicate Stop Record = ") << stop_rec.Stop ();
		}
		return (true);
	}
	return (false);
}
