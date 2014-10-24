//*********************************************************
//	Problem_Output.cpp - Output Interface Class
//*********************************************************

#include "Problem_Output.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Problem_Output constructor
//---------------------------------------------------------

Problem_Output::Problem_Output () : Sim_Output_Data ()
{
	Type (PROBLEM_OUTPUT_OFFSET);

	file = sim->System_Problem_File (true);
}

Problem_Output::~Problem_Output () 
{
	if (file != 0) {
		file->Close ();
	}
}

//---------------------------------------------------------
//	Output_Problem
//---------------------------------------------------------

void Problem_Output::Output_Problem (Problem_Data &problem)
{
	file->Lock ();
	sim->Put_Problem_Data (*file, problem);
	file->UnLock ();
}


