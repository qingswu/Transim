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

	//---- create the network files ----

	if (Slave ()) {
		System_File_False (PLAN);

		if (System_File_Flag (NEW_PROBLEM)) {
			problem_output.Output_Flag (true);
			System_File_False (NEW_PROBLEM);
		}
	}
	Simulator_Service::Program_Control ();

	if (Master ()) {
		if (System_File_Flag (PLAN)) {
			plan_file = (Plan_File *) System_File_Handle (PLAN);
		}
		Print (2, String ("%s Control Keys:") % Program ());
	}
}
