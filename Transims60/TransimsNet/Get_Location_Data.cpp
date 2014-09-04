//*********************************************************
//	Get_Location_Data.cpp - read the location file
//*********************************************************

#include "TransimsNet.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool TransimsNet::Get_Location_Data (Location_File &file, Location_Data &location_rec)
{
	if ((delete_link_flag && delete_link_range.In_Range (file.Link ())) ||
		(update_link_flag && update_link_range.In_Range (file.Link ()))) {
		xlocation++;
		return (false);
	}
	return (Data_Service::Get_Location_Data (file, location_rec));
}
