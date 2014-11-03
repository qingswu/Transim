//*********************************************************
//	Get_Turn_Delay_Data.cpp - read the turn volume file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Turn_Delay_Data
//---------------------------------------------------------

bool Relocate::Get_Turn_Delay_Data (Turn_Delay_File &file, Turn_Delay_Data &data)
{
	Int_Map_Itr itr;
	Link_Data *link_ptr;

	itr = link_map.find (file.Link ());

	if (itr != link_map.end ()) {
		link_ptr = &link_array [itr->second];

		if (link_ptr->Divided () == 1) {

			//---- copy the fields to the subarea file ----

			Db_Header *new_file = System_File_Header (NEW_TURN_DELAY);

			new_file->Copy_Fields (file);

			if (!new_file->Write ()) {
				Error (String ("Writing %s") % new_file->File_Type ());
			}
			num_perf++;
		}
	}
	data.Dir_Index (0);
	
	//---- don't save the record ----

	return (false);
}
