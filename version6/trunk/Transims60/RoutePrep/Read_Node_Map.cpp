//*********************************************************
//	Read_Node_Map.cpp - read node map file
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Node_Map
//---------------------------------------------------------

void RoutePrep::Read_Node_Map (void)
{
	String record;
	Strings fields;

	int node, index;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Node_Data *node_ptr;

	Show_Message (String ("Reading %s -- Record") % node_map_file.File_Type ());
	Set_Progress ();

	while (node_map_file.Read ()) {
		Show_Progress ();

		record = node_map_file.Record_String ();

		record.Parse (fields);

		node = fields [0].Integer ();
		if (node <= 0) continue;

		index = fields [1].Integer ();

		if (index > 0) {
			map_itr = node_map.find (index);

			if (map_itr == node_map.end ()) {
				Warning (String ("Node Map %d to %d was Not Found in the Node File") % node % index);
				continue;
			}
			index = map_itr->second;
		}

		//---- insert the node;

		map_stat = input_map.insert (Int_Map_Data (node, index));

		if (!map_stat.second) {
			Warning (String ("Duplicate Node Map %d to %d and %d") % node % index % map_stat.first->second);
			continue;
		}
		if (index >= 0) {
			node_ptr = &node_array [index];

			node_ptr->Control (node);
		}

	}
	End_Progress ();

	node_map_file.Close ();

	Print (2, String ("Number of %s Records = %d") % node_map_file.File_Type () % Progress_Count ());
}
