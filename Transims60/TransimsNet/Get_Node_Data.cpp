//*********************************************************
//	Get_Node_Data.cpp - additonal node processing
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool TransimsNet::Get_Node_Data (Node_File &file, Node_Data &data)
{
	if (delete_node_flag && delete_node_range.In_Range (file.Node ())) {
		xnode++;
		return (false);
	}
	if (node_data_flag) {
		if (node_data_array.Read_Record (file.Node ())) {
			file.Copy_Fields (node_data_array);
		}
	}
	return (Data_Service::Get_Node_Data (file, data));
}
