//*********************************************************
//	Get_Access_Data.cpp - read the access link file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Access_Data
//---------------------------------------------------------

bool TransimsNet::Get_Access_Data (Access_File &file, Access_Data &access_rec)
{
	if (delete_node_flag) {
		if (file.From_Type () == NODE_ID) {
			if (delete_node_range.In_Range (file.From_ID ())) {
				xaccess++;
				return (false);
			}
		}
		if (file.To_Type () == NODE_ID) {
			if (delete_node_range.In_Range (file.To_ID ())) {
				xaccess++;
				return (false);
			}
		}
	}
	return (Data_Service::Get_Access_Data (file, access_rec));
}
