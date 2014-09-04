//*********************************************************
//	Sim_Link_Process.hpp - simulate directional links
//*********************************************************

#ifndef SIM_LINK_PROCESS_HPP
#define SIM_LINK_PROCESS_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"
#include "Static_Service.hpp"
#include "Sim_Statistics.hpp"
#include "Sim_Dir_Data.hpp"
#include "Sim_Veh_Data.hpp"
#include "Travel_Step.hpp"
#include "Work_Queue.hpp"

//---------------------------------------------------------
//	Sim_Link_Process - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Link_Process : public Static_Service
{
public:
	Sim_Link_Process (void);

#ifdef THREADS
	Sim_Link_Process (Work_Queue *queue, int id = 0); 

	Work_Queue *link_queue;

	void operator()();
#endif
	bool Link_Processing (int link);
	bool Load_Vehicle (int traveler);
	bool Cell_Use (Sim_Dir_Ptr sim_dir_ptr, int lane, int cell, Travel_Step &step, bool use_flag = false);
	bool Lane_Change (Sim_Dir_Ptr sim_dir_ptr, int &lane, int cell, Travel_Step &step);
	bool Move_Vehicle (Travel_Step &step);
	bool Check_Behind (Sim_Veh_Data veh_cell, Travel_Step &travel_step, int num_cells = 1, bool use_flag = false);

	int  ID (void)                      { return (id); }
	int  Num_PCE (void)                 { return (num_pce); }
	int  Num_Vehicles (void)            { return (num_vehicles); }
	int  Num_Waiting (void)             { return (num_waiting); }

	void Reset_Counters (void)          { num_vehicles = num_waiting = num_pce = 0; }

	Sim_Statistics & Get_Statistics (void) { return (stats); }

private:
	Sim_Statistics stats;
	int num_pce, num_vehicles, num_waiting, id;
};
#endif
