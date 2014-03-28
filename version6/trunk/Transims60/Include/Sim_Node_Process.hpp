//*********************************************************
//	Sim_Node_Process.hpp - simulate links entering a node
//*********************************************************

#ifndef SIM_NODE_PROCESS_HPP
#define SIM_NODE_PROCESS_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Sim_Dir_Data.hpp"
#include "Sim_Veh_Data.hpp"
#include "Travel_Step.hpp"
#include "Sim_Statistics.hpp"

//---------------------------------------------------------
//	Sim_Node_Process - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Node_Process : public Static_Service
{
public:
	Sim_Node_Process (void);

	void operator()();

	void Initialize (void);

	bool Node_Processing (int node);
	bool Load_Vehicle (int traveler);
	bool Cell_Use (Sim_Dir_Ptr sim_dir_ptr, int lane, int cell, Travel_Step &step, bool use_flag = false);
	bool Lane_Change (Sim_Dir_Ptr sim_dir_ptr, int &lane, int cell, Travel_Step &step);
	bool Move_Vehicle (Travel_Step &step);
	bool Check_Behind (Sim_Veh_Data veh_cell, Travel_Step &travel_step, int num_cells = 1, bool use_flag = false);
	bool Output_Step (Travel_Step &step);

	int  Num_Vehicles (void)            { return (num_vehicles); }
	int  Num_Waiting (void)             { return (num_waiting); }

	void Reset_Counters (void)          { num_vehicles = num_waiting = 0; }

	Sim_Statistics & Get_Statistics (void) { return (stats); }

private:
	Sim_Statistics stats;
	int num_vehicles, num_waiting;
};
#endif
