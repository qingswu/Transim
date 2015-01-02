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

	//---- compare the connection files ----

	if (connect_flag) {
		num_connect = (int) connect_array.size ();

		Read_Connections (connect_file);

		Print (1, "Number of Additional Connection Records = ") << ((int) connect_array.size () - num_connect);

		//---- reset the connection list ----

		int num;
		Dir_Itr dir_itr;
		Connect_Itr connect_itr;
		Connect_Data *connect_ptr;
		Dir_Data *from_ptr, *to_ptr;

		for (dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++) {
			dir_itr->First_Connect_To (-1);
			dir_itr->First_Connect_From (-1);
		}
		for (connect_itr = connect_array.begin (); connect_itr != connect_array.end (); connect_itr++) {
			connect_itr->Next_From (-1);
			connect_itr->Next_To (-1);
		}
		num = (int) connect_array.size ();

		while (num--) {
			connect_ptr = &connect_array [num];

			if (connect_ptr->Dir_Index () < 0 || connect_ptr->Dir_Index () >= (int) dir_array.size ()) continue;
			if (connect_ptr->To_Index () < 0 || connect_ptr->To_Index () >= (int) dir_array.size ()) continue;

			from_ptr = &dir_array [connect_ptr->Dir_Index ()];
			to_ptr = &dir_array [connect_ptr->To_Index ()];

			if (from_ptr->Sign () != 0 && to_ptr->Sign () != 0) {
				connect_ptr->Next_To (from_ptr->First_Connect_To ());
				from_ptr->First_Connect_To (num);

				connect_ptr->Next_From (to_ptr->First_Connect_From ());
				to_ptr->First_Connect_From (num);
			}
		}
	}

	//---- compare the parking files ----

	if (parking_flag) {
		num_parking = (int) parking_array.size ();

		Read_Parking_Lots (parking_file);

		Print (1, "Number of Additional Parking Records = ") << ((int) parking_array.size () - num_parking);

		Map_Parking ();
	}

	//---- compare the access files ----

	if (access_flag) {
		num_access = (int) access_array.size ();

		Read_Access_Links (access_file);

		Print (1, "Number of Additional Access Records = ") << ((int) access_array.size () - num_access);
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
	Write (2, "Number of Plans with Problems = ") << num_problems;

<<<<<<< .working

	//---- performance ----

	if (System_File_Flag (NEW_PERFORMANCE) && System_File_Flag (PERFORMANCE)) {
		Read_Performance (*(System_Performance_File ()), perf_period_array);
	}

	//---- turn_delay ----

	if (System_File_Flag (NEW_TURN_DELAY) && System_File_Flag (TURN_DELAY)) {
		Read_Turn_Delays (*(System_Turn_Delay_File ()), turn_period_array);
	}
=======
	//---- performance ----

	if (System_File_Flag (NEW_PERFORMANCE) && System_File_Flag (PERFORMANCE)) {
		Read_Performance (*(System_Performance_File ()), perf_period_array);

		Write (2, "Number of Performance Records Written = ") << num_perf;
	}

	//---- turn_delay ----

	if (System_File_Flag (NEW_TURN_DELAY) && System_File_Flag (TURN_DELAY)) {
		Read_Turn_Delays (*(System_Turn_Delay_File ()), turn_period_array);

		Write (2, "Number of Turn Delay Records Written = ") << num_turn;
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LOCATION_MAP:			//---- Location Map Report ----
				Location_Map_Report ();
				break;
			case PARKING_MAP:			//---- Parking Map Report ---
				Parking_Map_Report ();
				break;
			default:
				break;
		}
	}

	//---- end the program ----

>>>>>>> .merge-right.r1529
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void Relocate::Page_Header (void)
{
	switch (Header_Number ()) {
		case LOCATION_MAP:			//---- Location Map Report ----
			Location_Map_Header ();
			break;
		case PARKING_MAP:			//---- Parking Map Report ---
			Parking_Map_Header ();
			break;
		default:
			break;
	}
}
