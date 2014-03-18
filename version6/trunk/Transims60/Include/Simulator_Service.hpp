//*********************************************************
//	Simulator_Service.hpp - simulation service
//*********************************************************

#ifndef SIMULATOR_SERVICE_HPP
#define SIMULATOR_SERVICE_HPP

//#include "Router_Service.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "Barrier.hpp"
#include "Work_Step.hpp"
#include "TypeDefs.hpp"

#include "Sim_Parameters.hpp"
#include "Sim_Statistics.hpp"
#include "Sim_Travel_Data.hpp"
#include "Sim_Plan_Data.hpp"
#include "Sim_Dir_Data.hpp"
#include "Sim_Connection.hpp"
#include "Sim_Park_Data.hpp"
#include "Sim_Stop_Data.hpp"
#include "Sim_Signal_Data.hpp"
#include "Sim_Veh_Data.hpp"
#include "Travel_Step.hpp"

#include "Sim_Update_Step.hpp"
#include "Sim_Plan_Step.hpp"
#include "Sim_Travel_Step.hpp"
#include "Sim_Node_Step.hpp"
#include "Sim_Output_Step.hpp"

#include "Problem_Output.hpp"
#include "Snapshot_Output.hpp"
#include "Link_Delay_Output.hpp"
#include "Performance_Output.hpp"
#include "Turn_Vol_Output.hpp"
#include "Ridership_Output.hpp"
#include "Occupancy_Output.hpp"
#include "Event_Output.hpp"
#include "Traveler_Output.hpp"

#include "Sim_Control_Update.hpp"
#include "Sim_Network_Update.hpp"
#include "Sim_Transit_Update.hpp"

//---------------------------------------------------------
//	Simulator_Service - simulation service class definition
//---------------------------------------------------------

//class SYSLIB_API Simulator_Service : public Router_Service, public Select_Service
class SYSLIB_API Simulator_Service : public Data_Service, public Select_Service
{
	friend class Sim_Update_Step;
	friend class Sim_Update_Data;
	friend class Sim_Plan_Step;
	friend class Sim_Plan_Process;
	friend class Sim_Travel_Step;
	friend class Sim_Travel_Process;
	friend class Sim_Node_Step;
	friend class Sim_Node_Process;
	friend class Sim_Output_Step;
	friend class Sim_Output_Data;

	friend class Problem_Output;
	friend class Simulator_Output;
	friend class Snapshot_Output;
	friend class Link_Delay_Output;
	friend class Performance_Output;
	friend class Ridership_Output;
	friend class Turn_Vol_Output;
	friend class Occupancy_Output;
	friend class Event_Output;
	friend class Traveler_Output;

	friend class Sim_Control_Update;
	friend class Sim_Network_Update;
	friend class Sim_Transit_Update;

public:

	Simulator_Service (void);

protected:
	enum Simulator_Service_Keys { 
		SIMULATION_START_TIME = SIM_SERVICE_OFFSET, SIMULATION_END_TIME, TIME_STEPS, 
		CELL_SIZE, PLAN_FOLLOWING_DISTANCE, LOOK_AHEAD_DISTANCE, LOOK_AHEAD_LANE_FACTOR, 
		LOOK_AHEAD_TIME_FACTOR, LOOK_AHEAD_VEHICLE_FACTOR, MAXIMUM_SWAPPING_SPEED, 
		MAXIMUM_SPEED_DIFFERENCE, ENFORCE_PARKING_LANES, 
		DRIVER_REACTION_TIME, PERMISSION_PROBABILITY, SLOW_DOWN_PROBABILITY, SLOW_DOWN_PERCENTAGE, 
		MAX_COMFORTABLE_SPEED, TRAVELER_TYPE_FACTORS, PRIORITY_LOADING_TIME, MAXIMUM_LOADING_TIME, 
		PRIORITY_WAITING_TIME, MAXIMUM_WAITING_TIME, MAX_DEPARTURE_TIME_VARIANCE, 
		MAX_ARRIVAL_TIME_VARIANCE, RELOAD_CAPACITY_PROBLEMS, COUNT_PROBLEM_WARNINGS, 
		PRINT_PROBLEM_MESSAGES, UNSIMULATED_SUBAREAS, MACROSCOPIC_SUBAREAS, MESOSCOPIC_SUBAREAS, 
		MICROSCOPIC_SUBAREAS, TURN_POCKET_FACTOR, MERGE_POCKET_FACTOR, OTHER_POCKET_FACTOR,
		NUMBER_OF_TRAVELERS,
	};
	void Simulator_Service_Keys (int *keys = 0);

	virtual void Program_Control (void);
	virtual void Execute (void);

	void Global_Data (void);

	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);

	void Transit_Vehicles (void);
	void Transit_Plans (void);

	bool Active (void)                     { return (active); }
	void Active (bool flag)                { active = flag; }

	bool Step_Flag (void)                  { return (step_flag); }
	void Step_Flag (Dtime step)            { step_flag = ((step % two_step) == 0); }

	bool No_Sim_Flag (void)                { return (no_sim_flag); }
	void No_Sim_Flag (bool flag)           { no_sim_flag = flag; }

	int  Num_Subareas (void)               { return (num_subareas); }
	void Num_Subareas (int value)          { num_subareas = value; }

	int  Num_Parts (void)                  { return (num_parts); }
	void Num_Parts (int value)             { num_parts = value; }
	
	int  First_Part (void)                 { return (first_part); }
	void First_Part (int num)              { first_part = num; }

	int  Last_Part (void)                  { return (last_part); }
	void Last_Part (int num)               { last_part = num; }

	int  Num_Simulators (void)             { return (num_sims); }
	void Num_Simulators (int value)        { num_sims = value; }

	int  Num_Vehicles (void)               { return (num_vehicles); }
	void Num_Vehicles (int value)          { num_vehicles = value; }
	void Add_Vehicles (int value = 1)      { num_vehicles += value; }

	//void Add_Statistics (Sim_Statistics &_stats);
	//Sim_Statistics & Get_Statistics (void) { return (stats); }
	Sim_Statistics & Get_Statistics (void);
	Sim_Parameters param;

	Dtime time_step, one_second;

	Data_Range no_range, macro_range, meso_range, micro_range;

	Sim_Travel_Array sim_travel_array;
	Sim_Plan_Array transit_plans;
	Vehicle_Map  sim_veh_map;

	Sim_Veh_Array sim_veh_array;

	Sim_Dir_Array  sim_dir_array;
	Sim_Connect_Array sim_connection;
	Sim_Park_Array sim_park_array;
	Sim_Stop_Array sim_stop_array;
	Sim_Signal_Array sim_signal_array;

	Int_Map subarea_map;

	Sim_Update_Step sim_update_step;
	Sim_Plan_Step sim_plan_step;
	Sim_Travel_Step sim_travel_step;
	Sim_Node_Step sim_node_step;
	Sim_Output_Step sim_output_step;

	Barrier node_barrier;
	Integers node_list, node_link, link_list;

	Problem_Output problem_output;

	Int2s_Array transfers;
	Work_Step work_step;

#ifdef MPI_EXE
	Ints_Array   mpi_parts;
	Int2_Array   mpi_range;
	Integers     part_rank;
	Int2_Set     mpi_exchange;
	I2_Ints_Map  mpi_boundary;
#endif

private:
	int two_step, num_subareas, max_subarea, num_sims, num_vehicles, transit_id;
	int first_part, last_part, num_parts, num_travelers;
	double avg_cell_per_veh;
	bool active, step_flag, no_sim_flag;
	Sim_Statistics stats;
};

extern SYSLIB_API Simulator_Service *sim;

#endif
