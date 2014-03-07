//*********************************************************
//	Write_Link_Map.cpp - write link id map file
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Write_Link_Map
//---------------------------------------------------------

void CountSum::Write_Link_Map (void)
{
	int link;
	ID_Offset_Itr map_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	//---- read link id map----

	Show_Message (String ("Writing %s -- Record") % new_map_file.File_Type ());
	Set_Progress ();

	for (map_itr = id_offset_map.begin (); map_itr != id_offset_map.end (); map_itr++) {
		Show_Progress ();

		new_map_file.Put_Field (0, map_itr->first);

		dir_ptr = &dir_array [map_itr->second.first];

		link_ptr = &link_array [dir_ptr->Link ()];

		link = link_ptr->Link ();
		if (dir_ptr->Dir () == 1) link = -link;

		new_map_file.Put_Field (1, link);
		new_map_file.Put_Field (2, map_itr->second.second / 100.0);

		new_map_file.Write ();
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % new_map_file.File_Type () % Progress_Count ());
}
