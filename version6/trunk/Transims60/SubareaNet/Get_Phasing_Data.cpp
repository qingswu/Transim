//*********************************************************
//	Get_Phasing_Data.cpp - read the phasing plan file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Phasing_Data
//---------------------------------------------------------

bool SubareaNet::Get_Phasing_Data (Phasing_File &file, Phasing_Record &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Phasing_Data (file, data)) {

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_PHASING_PLAN);

		new_file->Copy_Fields (file);

		if (!new_file->Write (file.Nested ())) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nphasing++;
	}
	return (false);
}
