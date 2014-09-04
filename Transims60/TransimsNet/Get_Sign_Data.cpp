//*********************************************************
//	Get_Sign_Data.cpp - read the sign file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Sign_Data
//---------------------------------------------------------

bool TransimsNet::Get_Sign_Data (Sign_File &file, Sign_Data &sign_rec)
{
	if ((delete_link_flag && delete_link_range.In_Range (file.Link ())) ||
		(update_link_flag && update_link_range.In_Range (file.Link ()))) {
		xsign++;
		return (false);
	}
	return (Data_Service::Get_Sign_Data (file, sign_rec));
}
