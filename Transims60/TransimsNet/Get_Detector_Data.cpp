//*********************************************************
//	Get_Detector_Data.cpp - read the detector file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Detector_Data
//---------------------------------------------------------

bool TransimsNet::Get_Detector_Data (Detector_File &file, Detector_Data &detector_rec)
{
	if (file.Detector () == 0) return (false);

	if (delete_link_flag && delete_link_range.In_Range (file.Link ())) {
		xdetector++;
		return (false);
	}
	if (update_link_flag && update_link_range.In_Range (file.Link ())) {
		int link, dir, offset, dir_index, lanes, low, high;

		Link_Data *link_ptr;

		link = file.Link ();
		dir = file.Dir ();
		offset = Round (file.Offset ());

		link_ptr = Set_Link_Direction (file, link, dir, offset);

		if (link_ptr == 0) return (false);

		if (dir) {
			dir_index = link_ptr->BA_Dir ();
		} else {
			dir_index = link_ptr->AB_Dir ();
		}
		if (dir_index < 0) {
			xdetector++;
			return (false);
		}

		//---- lane number ----

		lanes = file.Lanes ();
		Fix_Lane_Range (dir_index, lanes, low, high);
		file.Lanes (Make_Lane_Range (dir_index, low, high));
	}
	return (Data_Service::Get_Detector_Data (file, detector_rec));
}
