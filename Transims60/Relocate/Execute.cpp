//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Relocate::Execute (void)
{
	//---- read the network ----

	Data_Service::Execute ();

	//---- read the target files ----

	target_flag = true;

	//---- compare the node files ----

	num_node = (int) node_array.size ();

	Read_Nodes (node_file);

	Print (1, "Number of Additional Node Records = ") << ((int) node_array.size () - num_node);

	//---- compare the shape files ----

	if (shape_flag) {
		num_shape = (int) shape_array.size ();

		Read_Shapes (shape_file);

		Print (1, "Number of Additional Shape Records = ") << ((int) shape_array.size () - num_shape);
	}

	//---- compare the link files ----

	num_link = (int) link_array.size ();

	Read_Links (link_file);

	Print (1, "Number of Additional Link Records = ") << ((int) link_array.size () - num_link);

	//---- compare the location files ----

	num_location = (int) location_array.size ();

	Read_Locations (location_file);

	Print (1, "Number of Additional Location Records = ") << ((int) location_array.size () - num_location);

	Map_Locations ();

	//---- compare the parking files ----

	if (parking_flag) {
		num_parking = (int) parking_array.size ();

		Read_Parking_Lots (parking_file);

		Print (1, "Number of Additional Parking Records = ") << ((int) parking_array.size () - num_parking);

		Map_Parking ();
	}

	//---- convert the trip file ----

	if (trip_flag) {
		Trip_File *file = (Trip_File *) System_File_Handle (TRIP);

		Read_Trips (*file);

		file->Print_Summary ();

		file = (Trip_File *) System_File_Handle (NEW_TRIP);

		file->Print_Summary ();
	}

	//---- convert the plan file ----

	if (plan_flag) {
		Read_Plans ();
	}

	if (new_select_flag) {
		Write_Selections ();
	}
	Write (2, "Number of Plans with Link Problems = ") << num_problems;

	Exit_Stat (DONE);
}
