//*********************************************************
//	Sim_Output_Data.cpp - base class for simulator output
//*********************************************************

#include "Sim_Output_Data.hpp"
#include "Sim_Output_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Output_Data -- operator
//---------------------------------------------------------

void Sim_Output_Data::operator()()
{
	while (sim->sim_output_step.output_barrier.Go ()) {
		Output_Check ();
		sim->sim_output_step.output_barrier.Finish ();
	}
}
