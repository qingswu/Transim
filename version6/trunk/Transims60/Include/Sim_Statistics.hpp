//*********************************************************
//	Sim_Statistics.hpp - Simulator statistics
//*********************************************************

#ifndef SIM_STATISTICS_HPP
#define SIM_STATISTICS_HPP

#include "APIDefs.hpp"

//---------------------------------------------------------
//	Sim_Statistics - process summary statistics
//---------------------------------------------------------

class SYSLIB_API Sim_Statistics
{
public:
	Sim_Statistics (void)  { Clear_Statistics (); }

	void Clear_Statistics (void);
	void Add_Statistics (Sim_Statistics &_stats);

	double tot_hours;
	int num_trips;
	int num_vehicles;
	int num_start;
	int num_end;
	int num_veh_trips;
	int num_veh_start;
	int num_veh_end;
	int num_veh_lost;
	int num_change;
	int num_swap;
	int num_look_ahead;
	int num_slow_down;
	int num_runs;
	int num_run_start;
	int num_run_end;
	int num_run_lost;
	int num_transit;
	int num_board;
	int num_alight;

private:

};
#endif

