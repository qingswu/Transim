//*********************************************************
//	Get_Performance_Data - read the performance file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Performance_Data
//---------------------------------------------------------

bool Relocate::Get_Performance_Data (Performance_File &file, Performance_Data &data)
{
	Int_Map_Itr itr;
	Link_Data *link_ptr;

	itr = link_map.find (file.Link ());

	if (itr != link_map.end ()) {
		link_ptr = &link_array [itr->second];

		if (link_ptr->Divided () == 1) {

			//---- copy the fields to the subarea file ----

			Db_Header *new_file = System_File_Header (NEW_PERFORMANCE);

			new_file->Copy_Fields (file);

			if (!new_file->Write ()) {
				Error (String ("Writing %s") % new_file->File_Type ());
			}
			num_perf++;
		}
	}
	data.Count (0);
	
	//---- don't save the record ----

	return (false);
}
