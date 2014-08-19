//*********************************************************
//	Read_Link_Dir.cpp - read the link direction file
//*********************************************************

#include "ArcPerf.hpp"

//---------------------------------------------------------
//	Read_Link_Dir
//---------------------------------------------------------

void ArcPerf::Read_Link_Dir (void)
{
	int link, dir, num_rec;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int_Map_Itr map_itr;

	Show_Message (String ("Reading %s -- Record") % arcview_link_dir.File_Type ());
	Set_Progress ();
	num_rec = 0;

	//---- process each link ----

	while (link_dir_file.Read ()) {
		Show_Progress ();

		//---- get the link direction ----

		link = link_dir_file.Link ();

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) continue;

		link_ptr = &link_array [map_itr->second];
		if (link_ptr->Use () == 0) continue;

		dir = link_dir_file.Dir ();

		if (dir == 1) {
			if (link_ptr->BA_Dir () < 0) continue;
			dir_ptr = &dir_array [link_ptr->BA_Dir ()];
		} else {
			if (link_ptr->AB_Dir () < 0) continue;
			dir_ptr = &dir_array [link_ptr->AB_Dir ()];
		}

		//---- copy the data fields ----

		arcview_link_dir.Copy_Fields (link_dir_file);

		if (index_flag) {
			arcview_link_dir.Put_Field (1, link * 2 + dir);
		}

		Draw_Link (arcview_link_dir, link_ptr, dir);

		num_rec++;
	}
	End_Progress ();
	arcview_link_dir.Close ();

	Print (2, "Number of Arcview Link Direction Records = ") << num_rec;
}
