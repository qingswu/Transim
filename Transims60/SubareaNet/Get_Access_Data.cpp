//*********************************************************
//	Get_Access_Data.cpp - read the access file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Access_Data
//---------------------------------------------------------

bool SubareaNet::Get_Access_Data (Access_File &file, Access_Data &data)
{
	Node_Data *node_ptr;

	//---- find the highest ID ----

	int id = file.Link ();
	if (id > max_access) max_access = id;

	//---- do standard processing ----

	if (Data_Service::Get_Access_Data (file, data)) {

		if (data.From_Type () == NODE_ID) {
			node_ptr = &node_array [data.From_ID ()];
			if (node_ptr->Subarea () == 0) return (false);
		}
		if (data.To_Type () == NODE_ID) {
			node_ptr = &node_array [data.To_ID ()];
			if (node_ptr->Subarea () == 0) return (false);
		}

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_ACCESS_LINK);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		naccess++;
	}

	//---- don't save the record ----

	return (false);
}
