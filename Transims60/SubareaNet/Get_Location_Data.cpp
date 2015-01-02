//*********************************************************
//	Get_Location_Data.cpp - read the location file 
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Location_Data
//---------------------------------------------------------

bool SubareaNet::Get_Location_Data (Location_File &file, Location_Data &data)
{
	//---- find the highest ID ----

	int id = file.Location ();
	if (id > max_location) max_location = id;

	id = file.Zone ();
	if (id > max_zone) max_zone = id;

	//---- do standard processing ----

	if (Data_Service::Get_Location_Data (file, data)) {

		//---- check the subarea boundary ----

		Link_Data *link_ptr = &link_array [data.Link ()];

		if (link_ptr->Type () == 0)  return (false);

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_LOCATION);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nlocation++;
		return (true);
	}
	return (false);
}
