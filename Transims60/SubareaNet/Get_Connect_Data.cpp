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
		
		//---- check the subarea boundary ----

		if (transit_flag) {
			Dir_Data *dir_ptr = &dir_array [data.Dir_Index ()];
			Link_Data *link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Type () == 0)  return (false);

			dir_ptr = &dir_array [data.To_Index ()];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Type () == 0) return (false);
		}

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_CONNECTION);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nconnect++;
	}
	return (true);
}

