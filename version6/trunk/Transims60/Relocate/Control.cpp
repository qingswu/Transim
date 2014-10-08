//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Relocate::Program_Control (void)
{
	String key, target;

	trip_flag = (!System_Control_Empty (TRIP) && !System_Control_Empty (NEW_TRIP));
	plan_flag = (!System_Control_Empty (PLAN) && !System_Control_Empty (NEW_PLAN));

	if (!trip_flag && !plan_flag) {
		Error ("Input and Output Trip or Plan Files are Required");
	}
	if (!trip_flag) {
		System_File_False (TRIP);
		System_File_False (NEW_TRIP);
	}
	if (!plan_flag) {
		System_File_False (PLAN);
		System_File_False (NEW_PLAN);
		System_File_False (PARKING);
		System_File_False (ACCESS_LINK);
		System_File_False (TRANSIT_STOP);
		System_File_False (TRANSIT_ROUTE);
	}

	//---- open network and demand files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();

	select_flag = System_File_Flag (SELECTION);
	new_select_flag = System_File_Flag (NEW_SELECTION);

	Print (2, String ("%s Control Keys:") % Program ());
	Print (1);

	//---- target directory ----

	target = Get_Control_String (TARGET_DIRECTORY);

	if (!target.empty ()) {
		Print_Filename (1, "Target Directory", target);
		Fix_Directory (target);
	}

	//---- open target node file ----

	key = Get_Control_String (TARGET_NODE_FILE);

	if (!key.empty ()) {
		node_file.File_Type ("Target Node File");
		node_file.Open (target + key);
	}

	//---- open target link file ----

	key = Get_Control_String (TARGET_LINK_FILE);

	if (!key.empty ()) {
		link_file.File_Type ("Target Link File");
		link_file.Open (target + key);
	}

	//---- open target shape file ----

	key = Get_Control_String (TARGET_SHAPE_FILE);

	if (!key.empty ()) {
		shape_file.File_Type ("Target Shape File");
		shape_file.Open (target + key);
		shape_flag = true;
	}

	//---- open target location file ----

	key = Get_Control_String (TARGET_LOCATION_FILE);

	if (!key.empty ()) {
		location_file.File_Type ("Target Location File");
		location_file.Open (target + key);
	}

	//---- data used for plan file conversion ----

	if (plan_flag) {

		//---- open target parking file ----

		key = Get_Control_String (TARGET_PARKING_FILE);

		if (!key.empty ()) {
			parking_file.File_Type ("Target Parking File");
			parking_file.Open (target + key);
			parking_flag = true;
		}

		//---- open target access file ----

		key = Get_Control_String (TARGET_ACCESS_FILE);

		if (!key.empty ()) {
			access_file.File_Type ("Target Access File");
			access_file.Open (target + key);
			access_flag = true;
		}

		//---- open target stop file ----

		key = Get_Control_String (TARGET_STOP_FILE);

		if (!key.empty ()) {
			stop_file.File_Type ("Target Stop File");
			stop_file.Open (target + key);
			stop_flag = true;
		}

		//---- open target route file ----

		key = Get_Control_String (TARGET_ROUTE_FILE);

		if (!key.empty ()) {
			line_file.File_Type ("Target Route File");
			line_file.Open (target + key);
			line_flag = true;
		}
	}

	//---- maximum xy difference -----

	max_xy_diff = Round (Get_Control_Double (MAXIMUM_XY_DIFFERENCE));

	//---- delete problem plans ----

	delete_flag = Get_Control_Flag (DELETE_PROBLEM_PLANS);
} 
