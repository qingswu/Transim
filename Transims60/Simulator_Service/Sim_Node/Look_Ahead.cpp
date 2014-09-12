//*********************************************************
//	Look_Ahead.cpp - attempt a lane change
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Look_Ahead
//---------------------------------------------------------

bool Sim_Node_Process::Look_Ahead (Sim_Dir_Ptr sim_dir_ptr, int &lane, int cell, Travel_Step &step)
{
	if (sim_dir_ptr->In_Cell () > cell || cell > sim_dir_ptr->Out_Cell ()) return (false);

	int link, new_lane, index, sum_change, sum_stay, step_size;

	step_size = sim->method_time_step [sim_dir_ptr->Method ()];	

	if (((sim->time_step / step_size) % 2) == 0) {
		new_lane = lane + 1;
	} else {
		new_lane = lane - 1;
	}

	//---- check the lane occupancy and use type ----

	if (!Check_Cell (sim_dir_ptr, new_lane, cell, step)) return (false);

	//---- check vehicles behind ----

	link = step.Dir_Index ();

	if (!Check_Behind (link, new_lane, cell, step)) return (false);

	//---- sum the speeds and lane changes using the new lane ----

	sum_change = Sum_Path (sim_dir_ptr, new_lane, cell, step) - (int) sim->param.lane_factor;

	//---- sum the speeds and lane changes using the current lane ----

	sum_stay = Sum_Path (sim_dir_ptr, lane, cell, step);

	//---- calculate the change probability ----

	if (sum_change <= sum_stay * 2) return (false);

	index = sum_stay + sum_change;
	sum_stay = DTOI (index * step.sim_travel_ptr->random.Probability ());

	if (index >= 0) {
		if (sum_change < sum_stay) return (false);
	} else {
		if (sum_change > sum_stay) return (false);
	}

	//---- make the lane change ----

	lane = new_lane;
	stats.num_look_ahead++;
	return (true);
}
