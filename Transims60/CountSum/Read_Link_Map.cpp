//*********************************************************
//	Read_Link_Map.cpp - read link id map file
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Read_Link_Map
//---------------------------------------------------------

void CountSum::Read_Link_Map (void)
{
	int id, link, dir_index;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Int2_Key dir_offset;

	//---- read link id map----

	Show_Message (String ("Reading %s -- Record") % link_map_file.File_Type ());
	Set_Progress ();

	while (link_map_file.Read ()) {
		Show_Progress ();

		//---- get the link number ----

		id = link_map_file.Get_Integer (link_id_field);
		if (id == 0) continue;

		link = link_map_file.Get_Integer (link_dir_field);

		map_itr = link_map.find (abs (link));
		if (map_itr == link_map.end ()) {
			Warning (String ("Link %d was Not Found in the Link File") % abs (link));
			continue;
		}
		link_ptr = &link_array [map_itr->second];

		if (link > 0) {
			dir_index = link_ptr->AB_Dir ();

			if (dir_index < 0) {
				Warning (String ("Link %d is Not Available in the AB Direction") % link);
			}
		} else {
			dir_index = link_ptr->BA_Dir ();

			if (dir_index < 0) {
				Warning (String ("Link %d is Not Available in the BA Direction") % abs (link));
			}
		}
		dir_offset.first = dir_index;

		dir_offset.second = DTOI (link_map_file.Get_Double (link_offset_field) * 100.0);

		id_offset_map.insert (ID_Offset_Data (id, dir_offset));

		offset_index_map.insert (Offset_Index_Data (dir_offset, (int) offset_index_map.size ()));
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % link_map_file.File_Type () % Progress_Count ());
}
