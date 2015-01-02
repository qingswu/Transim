//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Reschedule::Execute (void)
{
	//---- read the network ----

	Data_Service::Execute ();

	//---- read detailed schedules ----

	if (run_flag) {
		Read_Runs ();
	}
	
	//---- update schedules based on link travel time changes ----

	if (update_flag) {
		Read_Performance (update_file, update_array);

		Update_Schedules ();
	}

	//---- write the selection file ----

	Write_Schedules ();

	Exit_Stat (DONE);
}
