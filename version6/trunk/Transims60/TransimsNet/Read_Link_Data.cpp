//*********************************************************
//	Read_Link_Data.cpp - Read the Data File into Memory
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Link_Data
//---------------------------------------------------------

void TransimsNet::Read_Link_Data (void)
{
	int link, count, node;

	//---- read the data file ----

	Show_Message (String ("Reading %s -- Record") % link_data_file.File_Type ());
	Set_Progress ();

	link_data_array.Replicate_Fields (&link_data_file);

	count = 0;

	while (link_data_file.Read ()) {
		Show_Progress ();

		link = link_data_file.Link ();
		if (link <= 0) continue;
			
		update_link_range.Add_Range (link, link);

		node = link_data_file.Node_A ();
		if (!update_node_range.In_Range (node)) {
			update_node_range.Add_Range (node, node);
			update_node_flag = true;
		}
		node = link_data_file.Node_B ();
		if (!update_node_range.In_Range (node)) {
			update_node_range.Add_Range (node, node);
			update_node_flag = true;
		}

		//---- copy the data fields ----

		link_data_array.Copy_Fields (link_data_file);

		//---- save the database record ----

		if (!link_data_array.Write_Record (link)) {
			Error ("Writing Link Data File Database");
		}
		count++;
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % link_data_file.File_Type () % count);
		
	link_data_file.Close ();
}
