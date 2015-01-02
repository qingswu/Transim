//*********************************************************
//	Get_Stop_Data.cpp - read the transit stop file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Stop_Data
//---------------------------------------------------------

bool SubareaNet::Get_Stop_Data (Stop_File &file, Stop_Data &data)
{
	//---- find the highest ID ----

	int id = file.Stop ();
	if (id > max_stop) max_stop = id;

	//---- do standard processing ----

	if (Data_Service::Get_Stop_Data (file, data)) {

		//---- check the subarea boundary ----

		Link_Data *link_ptr = &link_array [data.Link ()];

		if (link_ptr->Type () == 0)  return (false);

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_TRANSIT_STOP);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nstop++;

		return (true);
	}
	return (false);
}
