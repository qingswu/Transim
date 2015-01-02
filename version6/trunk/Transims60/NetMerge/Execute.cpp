//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "NetMerge.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void NetMerge::Execute (void)
{
	//---- read the network ----

	Data_Service::Execute ();

	//---- read the merge files ----

	merge_flag = true;

	//---- merge the node files ----

	int num_node = (int) node_array.size ();

	Read_Nodes (node_file);

	Print (1, "Number of Merge Node Records = ") << ((int) node_array.size () - num_node);

	//---- merge the zone files ----

	if (zone_flag) {
		int num_zone = (int) zone_array.size ();

		Read_Zones (zone_file);

		Print (1, "Number of Merge Zone Records = ") << ((int) zone_array.size () - num_zone);
	}

	//---- merge the shape files ----

	if (shape_flag) {
		int num_shape = (int) shape_array.size ();

		Read_Shapes (shape_file);

		Print (1, "Number of Merge Shape Records = ") << ((int) shape_array.size () - num_shape);
	}

	//---- merge the link files ----

	int num_link = (int) link_array.size ();

	Read_Links (link_file);

	Print (1, "Number of Merge Link Records = ") << ((int) link_array.size () - num_link);

	//---- merge the pocket file ----

	if (pocket_flag) {
		int num_pocket = (int) pocket_array.size ();

		Read_Pockets (pocket_file);

		Print (1, "Number of Merge Pocket Records = ") << ((int) pocket_array.size () - num_pocket);
	}

	//---- merge the lane use file ----

	if (lane_use_flag) {
		int num_lane_use = (int) lane_use_array.size ();

		Read_Lane_Uses (lane_use_file);

		Print (1, "Number of Merge Lane Use Records = ") << ((int) lane_use_array.size () - num_lane_use);
	}

	//---- merge the connection files ----

	if (connect_flag) {
		int num_connect = (int) connect_array.size ();

		Read_Connections (connect_file);

		Print (1, "Number of Merge Connection Records = ") << ((int) connect_array.size () - num_connect);
	}

	//---- merge the location files ----

	if (location_flag) {
		int num_location = (int) location_array.size ();

		Read_Locations (location_file);

		Print (1, "Number of Merge Location Records = ") << ((int) location_array.size () - num_location);
	}

	//---- merge the parking files ----

	if (parking_flag) {
		int num_parking = (int) parking_array.size ();

		Read_Parking_Lots (parking_file);

		Print (1, "Number of Merge Parking Records = ") << ((int) parking_array.size () - num_parking);
	}

	//---- merge the transit stops files ----

	if (stop_flag) {
		int num_stop = (int) stop_array.size ();

		Read_Stops (stop_file);

		Print (1, "Number of Merge Transit Stop Records = ") << ((int) stop_array.size () - num_stop);
	}

	//---- merge the access files ----

	if (access_flag) {
		int num_access = (int) access_array.size ();

		Read_Access_Links (access_file);

		Print (1, "Number of Merge Access Records = ") << ((int) access_array.size () - num_access);
	}

	//---- merge the signal files ----

	if (signal_flag) {
		int num_signal = (int) signal_array.size ();

		Read_Signals (signal_file);

		Print (1, "Number of Merge Signal Records = ") << ((int) signal_array.size () - num_signal);
	}

	//---- merge the timing files ----

	if (timing_flag) {
		Read_Timing_Plans (timing_file);
	}

	//---- merge the phasing files ----

	if (phasing_flag) {
		Read_Phasing_Plans (phasing_file);
	}

	//---- merge the detector files ----

	if (detector_flag) {
		Read_Detectors (detector_file);
	}

	//---- merge the sign files ----

	if (sign_flag) {
		Read_Signs (sign_file);
	}

	//----- write the output files ----

	Write_Nodes ();
	Write_Links ();
	if (System_File_Flag (NEW_SHAPE)) Write_Shapes ();
	if (System_File_Flag (NEW_POCKET)) Write_Pockets ();
	if (System_File_Flag (NEW_LANE_USE)) Write_Lane_Uses ();
	if (System_File_Flag (NEW_CONNECTION)) Write_Connections ();
	if (System_File_Flag (NEW_LOCATION)) Write_Locations ();
	if (System_File_Flag (NEW_PARKING)) Write_Parking_Lots ();
	if (System_File_Flag (NEW_ACCESS_LINK)) Write_Access_Links ();
	if (System_File_Flag (NEW_TRANSIT_STOP)) Write_Stops ();
	if (System_File_Flag (NEW_TRANSIT_ROUTE)) Write_Lines ();
	if (System_File_Flag (NEW_SIGNAL)) Write_Signals ();
	if (System_File_Flag (NEW_TIMING_PLAN)) Write_Timing_Plans ();
	if (System_File_Flag (NEW_PHASING_PLAN)) Write_Phasing_Plans ();
	if (System_File_Flag (NEW_DETECTOR)) Write_Detectors ();
	if (System_File_Flag (NEW_SIGN)) Write_Signs ();
	if (System_File_Flag (NEW_ZONE)) Write_Zones ();

	//---- performance ----

	if (performance_flag) {
		Read_Performance (*(System_Performance_File ()), perf_period_array);

		int num_perf = (int) perf_period_array.size ();

		Read_Performance (performance_file, perf_period_array);

		Print (1, "Number of Merge Performance Records = ") << ((int) perf_period_array.size () - num_perf);

		if (System_File_Flag (NEW_PERFORMANCE)) Write_Performance ();
	}

	//---- turn_delay ----

	if (turn_delay_flag) {
		Read_Turn_Delays (*(System_Turn_Delay_File ()), turn_period_array);

		int num_delay = (int) turn_period_array.size ();

		Read_Turn_Delays (turn_delay_file, turn_period_array);

		Print (1, "Number of Merge Turn Delay Records = ") << ((int) turn_period_array.size () - num_delay);

		if (System_File_Flag (NEW_TURN_DELAY)) Write_Turn_Delays ();
	}

	//---- print reports ----

	//for (int i=First_Report (); i != 0; i=Next_Report ()) {
	//	switch (i) {
	//		default:
	//			break;
	//	}
	//}

	//---- end the program ----

	Exit_Stat (DONE);
}

////---------------------------------------------------------
////	Page_Header
////---------------------------------------------------------
//
//void NetMerge::Page_Header (void)
//{
//	switch (Header_Number ()) {
//		default:
//			break;
//	}
//}
