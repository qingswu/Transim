//*********************************************************
//	Get_Location_Data.cpp - additional location processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool ArcNet::Get_Location_Data (Location_File &file, Location_Data &location_rec)
{
	double setback, side;

	Point_Map_Stat map_stat;

	if (Data_Service::Get_Location_Data (file, location_rec)) {
		if (arcview_location.Is_Open ()) {
			arcview_location.Copy_Fields (file);

			setback = UnRound (location_rec.Setback ());
			side = location_side;

			if (setback > side) side = setback;

			Point_Offset (location_rec.Link (), location_rec.Dir (), location_rec.Offset (), side, arcview_location);

			if (!arcview_location.Write_Record ()) {
				Error (String ("Writing %s") % arcview_location.File_Type ());
			}

			//---- save the location point data ----

			map_stat = location_pt.insert (Point_Map_Data (location_rec.Location (), arcview_location [0]));

			if (!map_stat.second) {
				Warning ("Duplicate Location Record = ") << location_rec.Location ();
			}
		}
	}
	return (false);
}
