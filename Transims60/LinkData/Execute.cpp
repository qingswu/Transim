//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LinkData::Execute (void)
{
	Link_Nodes link_node_rec;

	//---- read the data service ----

	Data_Service::Execute ();

	//---- allocate link node data ----

	link_node_rec.link = 0;
	link_node_rec.nodes.clear ();

	link_node_array.assign (link_array.size (), link_node_rec);

	//---- generate a node map ----

	if (node_map_flag) {
		Read_Node_Map ();
	}
	if (input_flag) {
		Read_Node_File ();
	}

	//---- read the link node list file ----

	if (link_node_flag) {
		Read_Link_Nodes ();
	}

	//--- create link data structures ----

	Create_Link_Map ();

	//---- read the directional data file ----

	Read_Dir_Data ();

	//---- write the link data file ----

	if (data_flag) {
		Write_Dir_Data ();
	}
	if (new_map_flag) {
		Write_Node_Map ();
	}
	if (new_link_node_flag) {
		Write_Link_Nodes ();
	}
	Print (2, "Number of Link Node Equivalence Records = ") << nequiv;
	Print (1, "Number of From-To Link Combinations = ") << nab;
	Print (1, "Number of Directional Data Records = ") << ndir;
	Print (1, "Number of Link Data Records Written = ") << nlink;

	Exit_Stat (DONE);
}
