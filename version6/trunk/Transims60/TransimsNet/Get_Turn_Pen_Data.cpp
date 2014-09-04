//*********************************************************
//	Get_Turn_Pen_Data.cpp - read the turn penalty file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Get_Turn_Pen_Data
//---------------------------------------------------------

bool TransimsNet::Get_Turn_Pen_Data (Turn_Pen_File &file, Turn_Pen_Data &turn_rec)
{
	if (file.Node_Based ()) {
		if (delete_node_flag) {
			if (delete_node_range.In_Range (file.Dir ()) || 
				delete_node_range.In_Range (file.In_Node ()) || 
				delete_node_range.In_Range (file.Out_Node ())) {
				xturn++;
				return (false);
			}
		}
	} else {
		if (delete_link_flag) {
			if (delete_link_range.In_Range (file.Link ()) || 
				delete_link_range.In_Range (file.To_Link ())) {
				nturn++;
				return (false);
			}
		}
	}
	return (Data_Service::Get_Turn_Pen_Data (file, turn_rec)); 
}
