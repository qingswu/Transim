//*********************************************************
//	Get_Parking_Data.cpp - read the parking file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Parking_Data
//---------------------------------------------------------

bool SubareaNet::Get_Parking_Data (Parking_File &file, Parking_Data &data)
{
	//---- find the highest ID ----

	if (!file.Nested ()) {
		int id = file.Parking ();
		if (id > max_parking) max_parking = id;
	}

	//---- do standard processing ----

	if (Data_Service::Get_Parking_Data (file, data)) {

		//---- check the subarea boundary ----

		Link_Data *link_ptr = &link_array [data.Link ()];

		if (link_ptr->Type () > 0) {

			//---- copy the fields to the subarea file ----

			Db_Header *new_file = System_File_Header (NEW_PARKING);

			new_file->Copy_Fields (file);

			if (!new_file->Write (file.Nested ())) {
				Error (String ("Writing %s") % new_file->File_Type ());
			}
			nparking++;
			return (true);
		}
	}
	return (false);
}
