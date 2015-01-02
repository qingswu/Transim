//*********************************************************
//	Get_Ridership_Data.cpp - read the Transit Ridership file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Ridership_Data
//---------------------------------------------------------

bool SubareaNet::Get_Ridership_Data (Ridership_File &file, Ridership_Data &rider_rec)
{
	//---- do standard processing ----

	if (Data_Service::Get_Ridership_Data (file, rider_rec)) {

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_RIDERSHIP);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nrider++;
	}
	return (false);
}
