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
	//if (Cell_Use (sim_dir_ptr, lane-1, cell, step)) {
	//	lane--;
	//	return (true);
	//} else if (Cell_Use (sim_dir_ptr, lane+1, cell, step)) {
	//	lane++;
	//	return (true);
	//}
	//return (false);

	int l;

	if (step.sim_leg_ptr != 0) {
		if (step.sim_leg_ptr->Out_Best_Low () < lane) {
			l = lane - 1;
			if (Cell_Use (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
		if (step.sim_leg_ptr->Out_Best_High () > lane) {
			l = lane + 1;
			if (Cell_Use (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
		if (step.sim_leg_ptr->Out_Lane_Low () < lane) {
			l = lane - 1;
			if (Cell_Use (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
		if (step.sim_leg_ptr->Out_Lane_High () > lane) {
			l = lane + 1;
			if (Cell_Use (sim_dir_ptr, l, cell, step)) {
				lane = l;
				return (true);
			}
		}
	}
	return (false);

		//			l = low - 1;
		//			if (l >= 0 && new_dir_ptr->Get (low, cell) < 0) {
		//				if (Cell_Use (new_dir_ptr, l, cell, step)) {
		//					lane = l;
		//					move_flag = true;
		//				}
		//			}
		//			if (!move_flag) {
		//				l = high + 1;
		//				if (l < new_dir_ptr->Lanes () && new_dir_ptr->Get (high, cell) < 0) {
		//					if (Cell_Use (new_dir_ptr, l, cell, step)) {
		//						lane = l;
		//						move_flag = true;
		//					}
		//				}
		//			}
		//		}
		//	}
		//}
}
