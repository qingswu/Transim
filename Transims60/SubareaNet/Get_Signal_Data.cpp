//*********************************************************
//	Get_Signal_Data.cpp - read the signal file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Signal_Data
//---------------------------------------------------------

bool SubareaNet::Get_Signal_Data (Signal_File &file, Signal_Data &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Signal_Data (file, data)) {

		//---- check the subarea boundary ----

		if (data.nodes.size () == 0) return (false);

		Node_Data *node_ptr = &node_array [data.nodes [0]];
		
		if (node_ptr->Subarea () != 1)  return (false);

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_SIGNAL);

		new_file->Copy_Fields (file);

		if (!new_file->Write (file.Nested ())) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nsignal++;
		return (true);
	}
	return (false);
}
