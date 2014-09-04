//*********************************************************
//	Add_Node_Data.cpp - add new node records
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Add_Node_Data
//---------------------------------------------------------

void TransimsNet::Add_Node_Data (void)
{
	int node, count;
	Db_Sort_Itr db_itr;
	Int_Map_Stat map_stat;
	Node_Data node_rec;

	//---- read the data file ----

	Show_Message (String ("Adding %s -- Record") % node_data_file.File_Type ());
	Set_Progress ();

	count = 0;

	for (db_itr = node_data_array.begin (); db_itr != node_data_array.end (); db_itr++) {
		Show_Progress ();

		node = db_itr->first;
		if (node_map.find (node) != node_map.end ()) continue;

		update_node_range.Add_Range (node, node);

		node_data_array.Read_Record (node);

		count++;
		node_data_file.Copy_Fields (node_data_array);

		node_rec.Clear ();
		node_rec.Node (node_data_file.Node ());
		node_rec.X (node_data_file.X ());
		node_rec.Y (node_data_file.Y ());
		node_rec.Z (node_data_file.Z ());
		node_rec.Subarea (node_data_file.Subarea ());
		node_rec.Notes (node_data_file.Notes ());

		map_stat = node_map.insert (Int_Map_Data (node_rec.Node (), (int) node_array.size ()));

		if (!map_stat.second) {
			Warning ("Duplicate Node Number = ") << node_rec.Node ();
			continue;
		} else {
			node_array.push_back (node_rec);
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records Added = %d") % node_data_file.File_Type () % count);
}
