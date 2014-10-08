//*********************************************************
//	Get_Stop_Data.cpp - read the transit stop file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Stop_Data
//---------------------------------------------------------

bool TransimsNet::Get_Stop_Data (Stop_File &file, Stop_Data &stop_rec)
{
	if ((delete_link_flag && delete_link_range.In_Range (file.Link ()))) {
		xstop++;
		return (false);
	}	
	return (Data_Service::Get_Stop_Data (file, stop_rec));
}
