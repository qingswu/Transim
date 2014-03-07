//*********************************************************
//	Get_Timing_Data.cpp - read the timing plan file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Timing_Data
//---------------------------------------------------------

int SubareaNet::Get_Timing_Data (Timing_File &file, Timing_Data &data)
{
	//---- do standard processing ----

	int signal = Data_Service::Get_Timing_Data (file, data);

	if (signal > 0) {

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_TIMING_PLAN);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		ntiming++;
	}
	return (signal);
}
