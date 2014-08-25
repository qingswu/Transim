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
		plan_file = (Plan_File *) System_File_Handle (PLAN);
	}
	Print (2, String ("%s Control Keys:") % Program ());
}
