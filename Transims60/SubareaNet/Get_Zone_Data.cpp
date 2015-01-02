//*********************************************************
//	Get_Zone_Data.cpp - read the zone file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Get_Zone_Data
//---------------------------------------------------------

bool SubareaNet::Get_Zone_Data (Zone_File &file, Zone_Data &data)
{
	//---- find the highest ID ----

	int id = file.Zone ();
	if (id > max_zone) max_zone = id;

	//---- do standard processing ----

	if (Data_Service::Get_Zone_Data (file, data)) {

		//---- check the subarea boundary ----

		if (box_flag) {
			if (data.X () < x_min || data.X () > x_max || data.Y () < y_min || data.Y() > y_max) {
				if (!outside_flag) return (false);
			} else if (outside_flag) {
				return (false);
			}
		} else if (!In_Polygon (subarea_boundary, UnRound (data.X ()), UnRound (data.Y ()))) {
			if (!outside_flag) return (false);
		} else if (outside_flag) {
			return (false);
		}

		//---- copy the fields to the subarea file ----

		Db_Header *new_file = System_File_Header (NEW_ZONE);

		new_file->Copy_Fields (file);

		if (!new_file->Write ()) {
			Error (String ("Writing %s") % new_file->File_Type ());
		}
		nzone++;
	}
	
	//---- don't save the record ----

	return (false);
}
