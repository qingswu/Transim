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

	if (run_flag) {
		Read_Runs ();
	}

	//---- write the selection file ----

	Write_Schedules ();

	Exit_Stat (DONE);
}
