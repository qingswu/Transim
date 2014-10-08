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

	//----- try to move away from lane use restrictions ----

	if (cell > sim_dir_ptr->In_Cell () && cell < sim_dir_ptr->Out_Cell ()) {
		if (Cell_Use (sim_dir_ptr, lane, cell, step)) return (false);
		int ln = -1;

		for (int i=1; i < sim_dir_ptr->Lanes (); i++) {
			l = lane - i;
			if (l >= 0) {
				if (Cell_Use (sim_dir_ptr, l, cell, step)) {
					ln = l;
					break;
				}
			}
			l = lane + i;
			if (l < sim_dir_ptr->Lanes ()) {
				if (Cell_Use (sim_dir_ptr, l, cell, step)) {
					ln = l;
					break;
				}
			}
		}
		if (ln >= 0) {
			if (sim_dir_ptr->Check (ln, cell)) {
				lane = ln;
				return (true);
			}
		}
	}
	return (false);
}
