//*********************************************************
//	Write_Node_Map.cpp - write node map file
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Write Node Map
//---------------------------------------------------------

void RoutePrep::Write_Node_Map (void)
{
	int count;
	Int_Map_Itr map_itr;
	Node_Data *node_ptr;

	fstream &fh = new_map_file.File ();

	Show_Message (String ("Writing %s -- Record") % new_map_file.File_Type ());
	Set_Progress ();

	count = 0;
	fh << "IN_NODE\tOUT_NODE" << endl;

	for (map_itr = input_map.begin (); map_itr != input_map.end (); map_itr++) {
		Show_Progress ();

		fh << map_itr->first << "\t";

		node_ptr = &node_array [map_itr->second];

		fh << node_ptr->Node () << endl;
		count++;
	}
	End_Progress ();

	new_map_file.Close ();

	Print (2, String ("Number of %s Records = %d") % new_map_file.File_Type () % count);
}
