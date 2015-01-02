//*********************************************************
//	Compare_Links.cpp - read the compare link map
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Compare_Links
//---------------------------------------------------------

void LinkSum::Compare_Links (void)
{
	int compare, link, dir, compare_field, link_field, dir_field;

	Link_Data *link_ptr;
	Ints_Map_Stat map_stat;
	Ints_Map_Itr map_itr;
	Integers link_list;

	//---- read the link map file ----

	Show_Message (String ("Reading %s -- Record") % link_map_file.File_Type ());
	Set_Progress ();

	compare_field = link_map_file.Optional_Field ("COMPARE", "COMPARE_LINK", "NEW_LINK");
	if (compare_field < 0) {
		compare_field = link_map_file.Required_Field (OUT_LINK_FIELD_NAMES);
	}
	link_field = link_map_file.Required_Field (IN_LINK_FIELD_NAMES);
	dir_field = link_map_file.LinkDir_Type_Field ();

	//---- read the compare link map ----

	while (link_map_file.Read ()) {
		Show_Progress ();

		compare = link_map_file.Get_Integer (compare_field);
		if (compare == 0) continue;

		link = link_map_file.Get_Integer (link_field);
		dir = link_map_file.Get_Integer (dir_field);

		link_ptr = Set_Link_Direction (link_map_file, link, dir);
		if (link_ptr == 0) continue;

		if (dir) {
			dir = link_ptr->BA_Dir ();
		} else {
			dir = link_ptr->AB_Dir ();
		}
		map_stat = compare_link_map.insert (Ints_Map_Data (compare, link_list));
		map_stat.first->second.push_back (dir);
	}
	End_Progress ();
	Print (2, String ("Number of %s Records = %d") % link_map_file.File_Type () % Progress_Count ());
	Print (1, String ("Number of Records Mapped = %d") % compare_link_map.size ());

	link_map_file.Close ();
}

