//*********************************************************
//	Get_Stop_Data.cpp - additional transit stop processing
//*********************************************************

#include "ArcNet.hpp"

//---------------------------------------------------------
//	Get_Stop_Data
//---------------------------------------------------------

bool ArcNet::Get_Stop_Data (Stop_File &file, Stop_Data &stop_rec)
{
	Point_Map_Stat map_stat;

	if (Data_Service::Get_Stop_Data (file, stop_rec)) {
		if (arcview_stop.Is_Open ()) {
			arcview_stop.Copy_Fields (file);

			Point_Offset (stop_rec.Link (), stop_rec.Dir (), stop_rec.Offset (), stop_side, arcview_stop);

			if (!arcview_stop.Write_Record ()) {
				Error (String ("Writing %s") % arcview_stop.File_Type ());
			}

			//---- save the location point data ----

			map_stat = stop_pt.insert (Point_Map_Data (stop_rec.Stop (), arcview_stop [0]));

			if (!map_stat.second) {
				Warning ("Duplicate Stop Record = ") << stop_rec.Stop ();
			}
		}
		return (true);
	}
	return (false);
}
