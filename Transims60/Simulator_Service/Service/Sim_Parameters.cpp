//*********************************************************
//	Sim_Parameters.cpp - Simulator parameters
//*********************************************************

#include "Sim_Parameters.hpp"

//---------------------------------------------------------
//	Clear_Parameters
//---------------------------------------------------------

void Sim_Parameters::Clear_Parameters (void)
{
	start_time_step = 0;
	end_time_step = 0;
	step_size = 0;
	cell_size = 0;
	max_start_variance = 0;
	min_load_time = 0;
	max_load_time = 0;
	priority_load = 0;
	min_wait_time = 0;
	max_wait_time = 0;
	priority_wait = 0;
	max_end_variance = 0;
	max_swap_speed = 0;
	max_swap_diff = 0;
	lane_factor = 0.0;
	time_factor = 0.0;
	veh_factor = 0.0;
	cap_factor = 1.0;
	look_ahead = 0;
	plan_follow = 0;
	lane_change_levels = 0;
	change_priority = 0;
	connect_lane_weight = 0;
	lane_use_weight = 0;

	memset (reaction_time, '\0', sizeof (reaction_time));
	memset (permission_prob, '\0', sizeof (permission_prob));
	memset (slow_down_percent, '\0', sizeof (slow_down_percent));
	memset (slow_down_prob, '\0', sizeof (slow_down_prob));
	
	traveler_fac.clear ();
	comfort_speed.clear ();

	traveler_flag = false;
	look_ahead_flag = false;
	slow_down_flag = false;
	reaction_flag = false;
	permit_none = false;
	permit_all = false;
	comfort_flag = false;
	reload_problems = false;
	count_warnings = false;
	print_problems = false;
	parking_lanes = false;

	problem_flag = false;
	control_flag = false;
	transit_flag = false;
}

