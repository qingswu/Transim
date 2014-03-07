//*********************************************************
//	Write_Node.cpp - write the subarea node file
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Write_Node
//---------------------------------------------------------

void SubareaNet::Write_Node (void)
{
	int node, node_field;

	Int_Map_Itr map_itr;
	Node_Data *node_ptr;

	Node_File *node_file = (Node_File *) System_File_Header (NEW_NODE);

	char *ext_dir [] = {
		"Subarea Node", "External Node"
	};

	//---- process each subarea nodes ----
	
	Show_Message ("Writing Subarea Node Data -- Record");
	Set_Progress ();

	node_field = node_db.Required_Field ("NODE", "ID", "N");

	node_db.Rewind ();

	while (node_db.Read_Record ()) {
		Show_Progress ();

		node = node_db.Get_Integer (node_field);

		map_itr = node_map.find (node);

		if (map_itr == node_map.end ()) continue;

		node_ptr = &node_array [map_itr->second];

		if (node_ptr->Subarea () == 0) continue;

		node_file->Copy_Fields (node_db);

		node_file->Put_Field ("NOTES", ext_dir [node_ptr->Subarea () - 1]);

		if (!node_file->Write ()) {
			Error (String ("Writing %s") % node_file->File_Type ());
		}
		nnode++;
	}
	End_Progress ();
}
