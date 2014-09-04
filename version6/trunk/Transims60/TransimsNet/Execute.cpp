//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TransimsNet::Execute (void)
{
	Int_Map_Itr map_itr;

	//---- read update data ----

	if (node_data_flag) {
		Read_Node_Data ();
	}
	if (zone_data_flag) {
		Read_Zone_Data ();
	}
	if (link_data_flag) {
		Read_Link_Data ();
	}

	//---- read the network ----

	Data_Service::Execute ();

	if (update_shape_flag) {
		Read_Shape_Data ();
	}
	//---- insert new records ----

	if (node_data_flag) {
		Add_Node_Data ();
	}
	if (zone_data_flag) {
		Add_Zone_Data ();
	}
	if (link_data_flag) {
		Add_Link_Data ();
	}

	//---- set the record numbers ----

	location_base = (int) location_array.size ();
	if (location_base > 0) {
		map_itr = --location_map.end ();
		location_id = ((map_itr->first / 100) + 1) * 100 - 1;
	}
	parking_base = (int) parking_array.size ();
	if (parking_base > 0) {
		map_itr = --parking_map.end ();
		parking_id = ((map_itr->first / 100) + 1) * 100 - 1;
	}
	
	//---- read the approach link file ----

	if (approach_flag) {
		Read_Approach_Links ();
	}

	//---- read the zone boundary file ----

	if (boundary_flag) {
		Read_Boundary ();
	}

	//---- add link access points ----

	if (replicate_flag) {
		Zone_Access ();

		Write_Access_Links ();
	} else {
		Link_Access ();
	}

	Write_Locations ();
	Write_Parking_Lots ();

	//---- build the link connection list ----

	Connections ();

	//---- create traffic controls ----

	Traffic_Controls ();

	Write_Signs ();

	Write_Signals ();

	//---- create pocket lanes ----

	Pocket_Lanes ();

	Write_Pockets ();

	//---- create lane ranges ----

	Lane_Ranges ();

	Write_Connections ();

	//---- write the network ----

	nnode = (int) node_array.size ();
	nlink = (int) link_array.size ();
	nshapes = (int) shape_array.size ();

	Write_Nodes ();

	if (zout_flag) {
		nzout = (int) zone_array.size ();
		Write_Zones ();
	}
	if (shape_flag) {
		nshape = Write_Shapes ();
	}
	Write_Links ();

	//---- link detail lane use ----

	if (link_use_flag && System_File_Flag (NEW_LANE_USE)) {
		Read_Link_Uses ();

		Lane_Use ();
		nuse = (int) lane_use_array.size ();

		Write_Lane_Uses ();
	}

	//---- write the turn penalties ----

	if (turn_flag && Network_File_Flag (NEW_TURN_PENALTY)) {
		Write_Turn_Pens ();
	}

	//---- write summary statistics ----

	Break_Check (6);
	Write (2, "Number of Input Node Records = ") << node_array.size ();
	Write (1, "Number of Input Link Records = ") << link_array.size ();
	Write (1, "Number of Input Zone Records = ") << zone_array.size ();

	Write (2, "Highest Zone Number = ") << Max_Zone_Number ();

	if (delete_flag) {
		Break_Check (10);
		Write (1);
		if (xnode) Write (1, "Number of Deleted Node Records = ") << xnode;
		if (xzone) Write (1, "Number of Deleted Zone Records = " ) << xzone;
		if (xshape) Write (1, "Number of Deleted Shape Records = ") << xshape;
		if (xlink) Write (1, "Number of Deleted Link Records = ") << xlink;
		if (xlocation) Write (1, "Number of Deleted Location Records = ") << xlocation;
		if (xparking) Write (1, "Number of Deleted Parking Lot Records = ") << xparking;
		if (xaccess) Write (1, "Number of Deleted Access Link Records = ") << xaccess;
		if (xpocket) Write (1, "Number of Deleted Pocket Lane Records = ") << xpocket;
		if (xconnect) Write (1, "Number of Deleted Connection Records = ") << xconnect;
		if (xuse) Write (1, "Number of Deleted Lane Use Records = ") << xuse;
		if (xturn) Write (1, "Number of Deleted Turn Penalty Records = ") << xturn;
		if (xsign) Write (1, "Number of Deleted Sign Records = ") << xsign;
		if (xsignal) Write (1, "Number of Deleted Signal Records = ") << xsignal;	
	}

	Break_Check (15);
	Write (2, "Number of New Node Records = ") << nnode;
	if (zout_flag) {
		Write (1, "Number of New Zone Records = ") << nzout;
	}
	Write (1, "Number of New Link Records = ") << nlink;

	if (shape_flag) {
		Write (1, "Number of New Link Shapes = ") << nshapes;
		Write (1, "Number of New Shape_Records = ") << nshape;
	}
	Write (1, "Number of New Location Records = ") << ((int) location_array.size () - location_base);
	Write (1, "Number of New Parking Lot Records = ") << ((int) parking_array.size () - parking_base);
	Write (1, "Number of New Access Link Records = ") << naccess;
	Write (1, "Number of New Pocket Lane Records = ") << npocket;
	Write (1, "Number of New Connection Records = ") << nconnect;
	if (nturn > 0) Write (1, "Number of New Turn Penalty Records = ") << nturn;
	if (nuse > 0) Write (1, "Number of New Lane Use Records = ") << nuse;
	Write (1, "Number of New Sign Records = ") << nsign;
	Write (1, "Number of New Signal Records = ") << nsignal;

	if (nexternal) {
		Write (2, "Number of External Connections = ") << nexternal;
	}
	if (nshort) {
		Print (2, "Number of Short Links Increased in Length = ") << nshort;
	}
	if (nlength) {
		Print (2, "Number of Coordinate-Based Length Adjustments = ") << nlength;
	}
	if (nsign > 0) {
		Break_Check (2);
		Print (1);
		if (nstop > 0) {
			Print (1, "Number of Stop Signs = ") << nstop;
		}
		if (nyield > 0) {
			Print (1, "Number of Yield Signs = ") << nyield;
		}
	}
	if (nsignal > 0) {
		Break_Check (5);
		Print (1);
		if (nfixed1 > 0) {
			Print (1, "Number of Fixed Timed Single Ring Signals = ") << nfixed1;
		}
		if (nfixed2 > 0) {
			Print (1, "Number of Fixed Timed Dual Ring Signals = ") << nfixed2;
		}
		if (nfixed3 > 0) {
			Print (1, "Number of Fixed Timed Triple Ring Signals = ") << nfixed3;
		}
		if (nactuated1 > 0) {
			Print (1, "Number of Demand Actuated Single Ring Signals = ") << nactuated1;
		}
		if (nactuated2 > 0) {
			Print (1, "Number of Demand Actuated Dual Ring Signals = ") << nactuated2;
		}
		if (nactuated3 > 0) {
			Print (1, "Number of Demand Actuated Triple Ring Signals = ") << nactuated3;
		}
	}
	Exit_Stat (DONE);
}
