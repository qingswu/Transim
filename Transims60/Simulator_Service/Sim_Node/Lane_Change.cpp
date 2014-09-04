//*********************************************************
//	Lane_Change.cpp - attempt a lane change
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Lane_Change
//---------------------------------------------------------

bool Sim_Node_Process::Lane_Change (Sim_Dir_Ptr sim_dir_ptr, int &lane, int cell, Travel_Step &step)
{
	if (Cell_Use (sim_dir_ptr, lane-1, cell, step)) {
		lane--;
		return (true);
	} else if (Cell_Use (sim_dir_ptr, lane+1, cell, step)) {
		lane++;
		return (true);
	}
	return (false);
}
