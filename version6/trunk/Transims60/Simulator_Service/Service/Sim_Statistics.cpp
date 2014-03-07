//*********************************************************
//	Sim_Statistics.cpp - Simulator statistics
//*********************************************************

#include "Sim_Statistics.hpp"

//---------------------------------------------------------
//	Clear_Statistics
//---------------------------------------------------------

void Sim_Statistics::Clear_Statistics (void)
{
	tot_hours = 0;
	num_trips = 0;
	num_vehicles = 0;
	num_start = 0;
	num_end = 0;
	num_veh_trips = 0;
	num_veh_start = 0;
	num_veh_end = 0;
	num_veh_lost = 0;
	num_change = 0;
	num_swap = 0;
	num_look_ahead = 0;
	num_slow_down = 0;
	num_runs = 0;
	num_run_start = 0;
	num_run_end = 0;
	num_run_lost = 0;
	num_transit = 0;
	num_board = 0;
	num_alight = 0;
}

//---------------------------------------------------------
//	Add_Statistics
//---------------------------------------------------------

void Sim_Statistics::Add_Statistics (Sim_Statistics &s)
{
	tot_hours += s.tot_hours;
	num_trips += s.num_trips;
	num_vehicles += s.num_vehicles;
	num_start += s.num_start;
	num_end += s.num_end;
	num_veh_trips += s.num_veh_trips;
	num_veh_start += s.num_veh_start;
	num_veh_end += s.num_veh_end;
	num_veh_lost += s.num_veh_lost;
	num_change += s.num_change;
	num_swap += s.num_swap;
	num_look_ahead += s.num_look_ahead;
	num_slow_down += s.num_slow_down;
	num_runs += s.num_runs;
	num_run_start += s.num_run_start;
	num_run_end += s.num_run_end;
	num_run_lost += s.num_run_lost;
	num_transit += s.num_transit;
	num_board += s.num_board;
	num_alight += s.num_alight;
}
