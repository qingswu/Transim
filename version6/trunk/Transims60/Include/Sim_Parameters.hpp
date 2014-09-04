//*********************************************************
//	Sim_Parameters.hpp - Simulator parameters
//*********************************************************

#ifndef SIM_PARAMETERS_HPP
#define SIM_PARAMETERS_HPP

#include "APIDefs.hpp"
#include "Dtime.hpp"
#include "Best_List.hpp"

class SYSLIB_API Sim_Parameters
{
public:
	Sim_Parameters (void) { Clear_Parameters ();};
	
	void Clear_Parameters (void);

	Dtime  start_time_step;                        //---- simulation start time step ----
	Dtime  end_time_step;                          //---- simulation end time step ----
	Dtime  step_size;
	int    cell_size;
	int    half_cell;
	Dtime  max_start_variance;
	Dtime  min_load_time;
	Dtime  max_load_time;
	Dtime  priority_load;
	Dtime  min_wait_time;
	Dtime  max_wait_time;
	Dtime  priority_wait;
	Dtime  max_end_variance;
	int    max_swap_speed;
	int    max_swap_diff;
	double lane_factor;
	double time_factor;
	double veh_factor;
	double cap_factor;
	int    look_ahead;
	int    plan_follow;
	int    lane_change_levels;
	int    change_priority;
	int    connect_lane_weight;
	int    lane_use_weight;

	double reaction_time [EXTERNAL+1];
	double permission_prob [EXTERNAL+1];
	double slow_down_percent [EXTERNAL+1];
	double slow_down_prob [EXTERNAL+1];
	
	Double_List traveler_fac;
	Integer_List comfort_speed;
	bool   traveler_flag;

	bool   look_ahead_flag;
	bool   slow_down_flag;
	bool   reaction_flag;
	bool   permit_none;
	bool   permit_all;
	bool   comfort_flag;
	bool   reload_problems;
	bool   count_warnings;
	bool   print_problems;
	bool   parking_lanes;

	bool   problem_flag;
	bool   control_flag;
	bool   transit_flag;

private:

};

#endif
