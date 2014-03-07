//*********************************************************
//	Get_Location_Data.cpp - additional location processing
//*********************************************************

#include "ArcPlan.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool ArcPlan::Get_Location_Data (Location_File &file, Location_Data &location_rec)
{
	double setback, side;

	Point_Map_Stat map_stat;

	if (Data_Service::Get_Location_Data (file, location_rec)) {

		setback = UnRound (location_rec.Setback ());
		side = location_side;

		if (setback > side) side = setback;

		Point_Offset (location_rec.Link (), location_rec.Dir (), location_rec.Offset (), side, points);

		//---- save the location point data ----

		map_stat = location_pt.insert (Point_Map_Data (location_rec.Location (), points [0]));

		if (!map_stat.second) {
			Warning ("Duplicate Location Record = ") << location_rec.Location ();
		}
		return (true);
	}
	return (false);
}
