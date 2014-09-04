//*********************************************************
//	Get_Parking_Data.cpp - read the parking file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Parking_Data
//---------------------------------------------------------

bool TransimsNet::Get_Parking_Data (Parking_File &file, Parking_Data &parking_rec)
{
	if (!file.Nested ()) {
		if ((delete_link_flag && delete_link_range.In_Range (file.Link ())) ||
			(update_link_flag && update_link_range.In_Range (file.Link ()))) {
			xparking++;
			return (false);
		}
	}
	return (Data_Service::Get_Parking_Data (file, parking_rec));
}
