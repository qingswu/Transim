//*********************************************************
//	Get_Timing_Data.cpp - read the timing plan file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Timing_Data
//---------------------------------------------------------

bool SubareaNet::Get_Timing_Data (Timing_File &file, Timing_Record &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Timing_Data (file, data)) {

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_TIMING_PLAN);

		new_file->Copy_Fields (file);

		if (!new_file->Write (file.Nested ())) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		ntiming++;
	}
	return (false);
}
