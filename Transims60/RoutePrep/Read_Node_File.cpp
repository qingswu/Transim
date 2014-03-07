//*********************************************************
//	Read_Node_File.cpp - read input node file
//*********************************************************

#include "RoutePrep.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Node_File
//---------------------------------------------------------

void RoutePrep::Read_Node_File (void)
{
	int node, index, best, node_field, x_field, y_field;
	double x, y, len, dx, dy, length;

	Node_Itr node_itr;
	Node_Data *node_ptr;
	Int_Map_Itr map_itr;

	Show_Message (String ("Reading %s -- Record") % input_node_file.File_Type ());
	Set_Progress ();
	
	node_field = input_node_file.Required_Field ("NODE", "ID", "N");
	x_field = input_node_file.Required_Field (X_FIELD_NAMES);
	y_field = input_node_file.Required_Field (Y_FIELD_NAMES);

	while (input_node_file.Read ()) {
		Show_Progress ();

		node = input_node_file.Get_Integer (node_field);
		if (node <= 0) continue;

		x = Round (input_node_file.Get_Double (x_field));
		y = Round (input_node_file.Get_Double (y_field));

		//---- check for a skipped node ----

		map_itr = input_map.find (node);
		if (map_itr != input_map.end ()) continue;

		//---- check for a number match ----

		map_itr = node_map.find (node);
		if (map_itr != node_map.end ()) {
			index = map_itr->second;
			node_ptr = &node_array [index];

			dx = node_ptr->X () - x;
			dy = node_ptr->Y () - y;
			length = sqrt (dx * dx + dy * dy);

			if (length < resolution) {
				input_map.insert (Int_Map_Data (node, index));
				node_ptr->Control (node);
				continue;
			}
		}

		//---- match the coordinates ----

		len = resolution;
		best = -1;

		for (index=0, node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++, index++) {

			dx = node_itr->X () - x;
			dy = node_itr->Y () - y;
			length = sqrt (dx * dx + dy * dy);

			if (length < len) {
				len = length;
				best = index;
				if (len == 0.0) break;
			}
		}
		if (best >= 0) {
			node_ptr = &node_array [best];
			if (node_ptr->Control () >= 0) {
				Warning (String ("Input Nodes %d and %d both Match to Node %d") % node % node_ptr->Control () % node_ptr->Node ());
				continue;
			}
			node_ptr->Control (node);
		}
		input_map.insert (Int_Map_Data (node, best));
	}
	End_Progress ();

	input_node_file.Close ();

	Print (2, String ("Number of %s Records = %d") % input_node_file.File_Type () % Progress_Count ());
}
