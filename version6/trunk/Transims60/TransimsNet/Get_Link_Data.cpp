//*********************************************************
//	Get_Link_Data.cpp - additonal link processing
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Link_Data
//---------------------------------------------------------

bool TransimsNet::Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba)
{
	int link = file.Link ();
	if (delete_link_flag && delete_link_range.In_Range (link)) {
		xlink++;
		return (false);
	}
	if (delete_node_flag && delete_node_range.In_Range (file.Node_A ())) {
		if (!delete_link_flag || !delete_link_range.In_Range (link)) {
			delete_link_range.Add_Range (link, link);
			delete_link_flag = true;
		}
		xlink++;
		return (false);
	}
	if (delete_node_flag && delete_node_range.In_Range (file.Node_B ())) {
		if (!delete_link_flag || !delete_link_range.In_Range (link)) {
			delete_link_range.Add_Range (link, link);
			delete_link_flag = true;
		}
		xlink++;
		return (false);
	}
	if (link_data_flag) {
		if (link_data_array.Read_Record (link)) {
			Data_Service::Get_Link_Data (file, data, ab, ba);
			file.Copy_Fields (link_data_array);

			file.Bearing_A (ab.In_Bearing ());
			file.Bearing_B (ab.Out_Bearing ());
		}
	}
	return (Data_Service::Get_Link_Data (file, data, ab, ba));
}
