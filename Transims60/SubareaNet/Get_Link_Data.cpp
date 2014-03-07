//*********************************************************
//	Get_Link_Data.cpp - read the link file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Link_Data
//---------------------------------------------------------

bool SubareaNet::Get_Link_Data (Link_File &file, Link_Data &data, Dir_Data &ab, Dir_Data &ba)
{
	//---- do standard processing ----

	if (Data_Service::Get_Link_Data (file, data, ab, ba)) {
		Node_Data *anode_ptr, *bnode_ptr;

		int type = 0;

		anode_ptr = &node_array [data.Anode ()];
		if (anode_ptr->Subarea () == 1) type = 1;

		bnode_ptr = &node_array [data.Bnode ()];
		if (bnode_ptr->Subarea () == 1) type += 2;

		if (type > 0) {
			Db_Header *new_file = System_File_Header (NEW_LINK);

			//---- copy the fields to the subarea file ----

			new_file->Copy_Fields (file);

			if (!new_file->Write ()) {
				Error (String ("Writing %s") % new_file->File_Type ());
			}
			nlink++;

			//---- update the node flags ----

			if (type == 1) {
				bnode_ptr->Subarea (2);
				nboundary++;
			} else if (type == 2) {
				anode_ptr->Subarea (2);
				nboundary++;
			}

		} else {

			//---- delete shape points ----

			Int_Map_Itr itr = shape_map.find (data.Link ());

			if (itr != shape_map.end ()) {
				Shape_Data *shape_ptr = &shape_array [itr->second];
				shape_ptr->clear ();
			}
		}
		if (transit_flag || type > 0) {
			data.Type (type);
			return (true);
		}
	}
	return (false);
}
