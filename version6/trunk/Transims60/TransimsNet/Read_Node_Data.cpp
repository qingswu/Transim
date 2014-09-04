//*********************************************************
//	Read_Node_Data.cpp - Read the Data File into Memory
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Node_Data
//---------------------------------------------------------

void TransimsNet::Read_Node_Data (void)
{
	int node, count;

	//---- read the data file ----

	Show_Message (String ("Reading %s -- Record") % node_data_file.File_Type ());
	Set_Progress ();

	node_data_array.Replicate_Fields (&node_data_file);

	count = 0;

	while (node_data_file.Read ()) {
		Show_Progress ();

		node = node_data_file.Node ();
		if (node <= 0) continue;
		
		update_node_range.Add_Range (node, node);

		//---- copy the data fields ----

		node_data_array.Copy_Fields (node_data_file);

		//---- save the database record ----

		if (!node_data_array.Write_Record (node)) {
			Error ("Writing Node Data File Database");
		}
		count++;
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % node_data_file.File_Type () % count);
		
	node_data_file.Close ();
}
