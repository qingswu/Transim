//*********************************************************
//	Get_Parking_Data.cpp - additional parking processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Parking_Data
//---------------------------------------------------------

bool ArcNet::Get_Parking_Data (Parking_File &file, Parking_Data &parking_rec)
{
	Point_Map_Stat map_stat;
	Park_Nest_Itr nest_itr;

	if (Data_Service::Get_Parking_Data (file, parking_rec)) {
		if (arcview_parking.Is_Open ()) {
			arcview_parking.Copy_Fields (file, true);

			if (!file.Nested ()) {
				Point_Offset (parking_rec.Link (), parking_rec.Dir (), parking_rec.Offset (), parking_side, arcview_parking);

				//---- save the parking point data ----

				map_stat = parking_pt.insert (Point_Map_Data (parking_rec.Parking (), arcview_parking [0]));

				if (!map_stat.second && file.Version () > 40) {
					Warning ("Duplicate Parking Record = ") << parking_rec.Parking ();
				}
				if (file.Num_Nest () == 0) {
					file.Blank_Nested_Fields ();
					file.Nested (true);
					arcview_parking.Copy_Fields (file, true);
					file.Nested (false);
				}
			}
			if (file.Nest () || file.Num_Nest () == 0 || file.Version () <= 40) {
				if (time_flag && parking_rec.size () > 0) {
					nest_itr = parking_rec.begin ();

					if (nest_itr->Start () > time || time > nest_itr->End ()) {
						parking_rec.clear ();
						return (false);
					}
				}
				if (!arcview_parking.Write_Record ()) {
					Error (String ("Writing %s") % arcview_parking.File_Type ());
				}
				parking_rec.clear ();
			}
		}
	}
	return (false);
}
