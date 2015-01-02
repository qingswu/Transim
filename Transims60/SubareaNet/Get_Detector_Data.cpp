//*********************************************************
//	Get_Detector_Data.cpp - read the detector file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Detector_Data
//---------------------------------------------------------

bool SubareaNet::Get_Detector_Data (Detector_File &file, Detector_Data &data)
{
	//---- do standard processing ----

	if (Data_Service::Get_Detector_Data (file, data)) {

		//---- check the subarea boundary ----

		Dir_Data *dir_ptr = &dir_array [data.Dir_Index ()];
		Link_Data *link_ptr = &link_array [dir_ptr->Link ()];

		if (link_ptr->Type () == 0)  return (false);

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_DETECTOR);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		ndetector++;
		return (true);
	}
	return (false);
}