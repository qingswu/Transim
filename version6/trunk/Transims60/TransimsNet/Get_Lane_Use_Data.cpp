//*********************************************************
//	Get_Lane_Use_Data.cpp - read the lane use file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Lane_Use_Data
//---------------------------------------------------------

bool TransimsNet::Get_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &lane_use_rec)
{
	if ((delete_link_flag && delete_link_range.In_Range (file.Link ())) ||
		(update_link_flag && update_link_range.In_Range (file.Link ()))) {
		xuse++;
		return (false);
	}
	return (Data_Service::Get_Lane_Use_Data (file, lane_use_rec));
}
