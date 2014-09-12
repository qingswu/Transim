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
	int l;

	if (step.sim_leg_ptr != 0) {
		if (step.sim_leg_ptr->Out_Best_Low () < lane) {
			l = lane - 1;
			if (Check_Cell (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
		if (step.sim_leg_ptr->Out_Best_High () > lane) {
			l = lane + 1;
			if (Check_Cell (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
		if (step.sim_leg_ptr->Out_Lane_Low () < lane) {
			l = lane - 1;
			if (Check_Cell (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
		if (step.sim_leg_ptr->Out_Lane_High () > lane) {
			l = lane + 1;
			if (Check_Cell (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
	}
	//if (Check_Cell (sim_dir_ptr, lane-1, cell, step)) {
	//	lane--;
	//	return (true);
	//} else if (Check_Cell (sim_dir_ptr, lane+1, cell, step)) {
	//	lane++;
	//	return (true);
	//}
	return (false);
}
