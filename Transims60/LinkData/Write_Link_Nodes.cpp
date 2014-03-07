//*********************************************************
//	Write_Link_Nodes.cpp - Write the Link Node List File
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Write_Link_Nodes
//---------------------------------------------------------

void LinkData::Write_Link_Nodes (void)
{
	int count = 0;

	Int_Map_Itr itr;
	Int_Itr int_itr;
	Link_Nodes *ptr;

	Show_Message (String ("Writing %s -- Record") % new_link_node.File_Type ());
	Set_Progress ();

	fstream &file = new_link_node.File ();

	for (itr = link_map.begin (); itr != link_map.end (); itr++) {
		Show_Progress ();

		ptr = &link_node_array [itr->second];

		if (ptr->link == 0 || ptr->nodes.size () == 0) continue;

		file << ptr->link << " =";

		for (int_itr = ptr->nodes.begin (); int_itr != ptr->nodes.end (); int_itr++) {
			file << " " << *int_itr;
		}
		file << endl;
		count++;
	}
	End_Progress ();
	new_link_node.Close ();
	
	Print (2, String ("Number of %s Records = %d") % new_link_node.File_Type () % count);
}
