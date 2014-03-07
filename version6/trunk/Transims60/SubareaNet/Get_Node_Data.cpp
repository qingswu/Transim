//*********************************************************
//	Get_Node_Data.cpp - read the node file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Node_Data
//---------------------------------------------------------

bool SubareaNet::Get_Node_Data (Node_File &file, Node_Data &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Node_Data (file, data)) {

		//---- copy the record to the node database ----

		node_db.Copy_Fields (file);

		if (!node_db.Add_Record ()) {
			Error ("Writing Node Database");
		}

		//---- check the subarea boundary ----

		if (box_flag) {
			if (data.X () >= x_min && data.X () <= x_max && data.Y () >= y_min && data.Y() <= y_max) {
				data.Subarea (1);
			} else {
				data.Subarea (0);
			}
		} else if (In_Polygon (subarea_boundary, UnRound (data.X ()), UnRound (data.Y ()))) {
			data.Subarea (1);
		} else {
			data.Subarea (0);
		}
		return (true);
	}
	return (false);
}
