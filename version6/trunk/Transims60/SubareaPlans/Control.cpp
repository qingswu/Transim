//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SubareaPlans::Program_Control (void)
{
	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();
	
	plan_file = System_Plan_File ();
	if (!plan_file->Part_Flag ()) Num_Threads (1);

	new_plan_file = System_Plan_File (true);
	new_plan_file->Sort_Type (plan_file->Sort_Type ());
	//new_plan_file->Close ();

	if (Num_Threads () > 1 && !new_plan_file->Part_Flag ()) {
		Error ("Partitioned Input Plans require Partitioned Output Plans");
	}
	select_flag = System_File_Flag (SELECTION);
	access_flag = System_File_Flag (ACCESS_LINK);
	trip_flag = System_File_Flag (NEW_TRIP);

	new_trip_file = System_Trip_File (true);

	if (System_File_Flag (TRANSIT_STOP) || System_File_Flag (TRANSIT_ROUTE) ||
		System_File_Flag (TRANSIT_SCHEDULE) || System_File_Flag (TRANSIT_DRIVER)) {

		transit_flag = (System_File_Flag (TRANSIT_STOP) && System_File_Flag (TRANSIT_ROUTE) &&
			System_File_Flag (TRANSIT_SCHEDULE) && System_File_Flag (TRANSIT_DRIVER));

		if (!transit_flag) {
			Error ("Transit Stop, Route, Schedule, and Driver Files are Required");
		}
	}

	Print (2, String ("%s Control Keys:") % Program ());
} 
