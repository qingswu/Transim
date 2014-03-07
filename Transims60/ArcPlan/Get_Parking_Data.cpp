//*********************************************************
//	Get_Parking_Data.cpp - additional parking processing
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Get_Parking_Data
//---------------------------------------------------------

bool ArcPlan::Get_Parking_Data (Parking_File &file, Parking_Data &parking_rec)
{
	Point_Map_Stat map_stat;

	if (Data_Service::Get_Parking_Data (file, parking_rec)) {
		if (!file.Nested ()) {
			Point_Offset (parking_rec.Link (), parking_rec.Dir (), parking_rec.Offset (), parking_side, points);

			//---- save the parking point data ----

			map_stat = parking_pt.insert (Point_Map_Data (parking_rec.Parking (), points [0]));

			if (!map_stat.second && file.Version () > 40) {
				Warning ("Duplicate Parking Record = ") << parking_rec.Parking ();
			}
		}
		return (true);
	}
	return (false);
}

