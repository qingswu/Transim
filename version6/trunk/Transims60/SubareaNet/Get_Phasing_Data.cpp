//*********************************************************
//	Get_Phasing_Data.cpp - read the phasing plan file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Phasing_Data
//---------------------------------------------------------

int SubareaNet::Get_Phasing_Data (Phasing_File &file, Phasing_Data &data)
{
	//---- do standard processing ----

	int signal = Data_Service::Get_Phasing_Data (file, data);

	if (signal > 0) {

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_PHASING_PLAN);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nphasing++;
	}
	return (signal);
}
