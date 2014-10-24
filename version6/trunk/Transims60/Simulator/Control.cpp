//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void Simulator::Program_Control (void)
{
	String key;

	Simulator_Service::Program_Control ();

	if (System_File_Flag (PLAN)) {
		plan_file = System_Plan_File ();
	}
	Print (2, String ("%s Control Keys:") % Program ());

#ifdef ROUTING
	System_Read_False (TRIP);
	System_Data_Reserve (TRIP, 0);
#else
	System_Read_False (PLAN);
	System_Data_Reserve (PLAN, 0);
#endif
}
