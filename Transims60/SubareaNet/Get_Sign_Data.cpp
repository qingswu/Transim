//*********************************************************
//	Get_Sign_Data.cpp - read the sign file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Sign_Data
//---------------------------------------------------------

bool SubareaNet::Get_Sign_Data (Sign_File &file, Sign_Data &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Sign_Data (file, data)) {

		//---- check the subarea boundary ----

		Dir_Data *dir_ptr = &dir_array [data.Dir_Index ()];
		Link_Data *link_ptr = &link_array [dir_ptr->Link ()];

		if (link_ptr->Type () > 0) {

			//---- copy the fields to the subarea file ----

			Db_Header *new_file = System_File_Header (NEW_SIGN);

			new_file->Copy_Fields (file);

			if (!new_file->Write ()) {
				Error (String ("Writing %s") % new_file->File_Type ());
			}
			nsign++;
		}
	}
		
	//---- don't save the record ----

	return (false);
}
