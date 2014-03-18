//*********************************************************
//	Sim_Update_Data.cpp - update simulation environment
//*********************************************************

#include "Sim_Update_Data.hpp"
#include "Sim_Update_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Update_Data -- operator
//---------------------------------------------------------

void Sim_Update_Data::operator()()
{
	while (sim->sim_update_step.update_barrier.Go ()) {
		Update_Check ();
		sim->sim_update_step.update_barrier.Finish ();
	}
}
