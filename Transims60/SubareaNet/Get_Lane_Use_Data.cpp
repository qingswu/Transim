//*********************************************************
//	Get_Lane_Use_Data.cpp - read the lane use file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Lane_Use_Data
//---------------------------------------------------------

bool SubareaNet::Get_Lane_Use_Data (Lane_Use_File &file, Lane_Use_Data &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Lane_Use_Data (file, data)) {

		//---- check the subarea boundary ----

		Dir_Data *dir_ptr = &dir_array [data.Dir_Index ()];
		Link_Data *link_ptr = &link_array [dir_ptr->Link ()];
	
		if (link_ptr->Type () == 0)  return (false);

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_LANE_USE);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nlane_use++;
	}
	
	//---- don't save the record ----

	return (false);
}
