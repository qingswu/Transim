//*********************************************************
//	Get_Connect_Data.cpp - read the connection file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Connect_Data
//---------------------------------------------------------

bool SubareaNet::Get_Connect_Data (Connect_File &file, Connect_Data &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Connect_Data (file, data)) {

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_CONNECTION);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nconnect++;
	}

	//---- don't save the record ----

	return (false);
}

