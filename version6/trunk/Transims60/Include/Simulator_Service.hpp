//*********************************************************
//	Simulator_Service.hpp - simulation service
//*********************************************************

#ifndef SIMULATOR_SERVICE_HPP
#define SIMULATOR_SERVICE_HPP

//#include "Router_Service.hpp"
#include "Data_Service.hpp"
#include "Select_Service.hpp"
#include "TypeDefs.hpp"
#include "Dtime.hpp"

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
#include "Performance_Output.hpp"
#include "Turn_Delay_Output.hpp"
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
	friend class Performance_Output;
	friend class Turn_Delay_Output;
	friend class Ridership_Output;
	friend class Occupancy_Output;
	friend class Event_Output;
	friend class Traveler_Output;

	friend class Sim_Control_Update;
	friend class Sim_Network_Update;
	friend class Sim_Transit_Update;

	friend class Sim_Plan_Data;
	friend class Sim_Travel_Data;
	friend class Sim_Dir_Data;
	friend class Sim_Dir_Array;

public:

	Simulator_Service (void);

protected:
	enum Simulator_Service_Keys { 
		SIMULATION_START_TIME = SIM_SERVICE_OFFSET, SIMULATION_END_TIME, SIMULATION_TIME_BREAKS, 
		SIMULATION_GROUP_SUBAREAS, GROUP_PERIOD_METHODS, RANDOM_NODE_INCREMENT, 
		UNSIMULATED_TIME_STEPS, MACROSCOPIC_TIME_STEPS, MESOSCOPIC_TIME_STEPS, MICROSCOPIC_TIME_STEPS,
		CELL_SIZE, PLAN_FOLLOWING_DISTANCE, LOOK_AHEAD_DISTANCE, LOOK_AHEAD_LANE_FACTOR, 
		LOOK_AHEAD_TIME_FACTOR, LOOK_AHEAD_VEHICLE_FACTOR, MAXIMUM_SWAPPING_SPEED, 
		MAXIMUM_SPEED_DIFFERENCE, CAPACITY_FACTOR, ENFORCE_PARKING_LANES, 
		DRIVER_REACTION_TIME, PERMISSION_PROBABILITY, SLOW_DOWN_PROBABILITY, SLOW_DOWN_PERCENTAGE, 
		MAX_COMFORTABLE_SPEED, TRAVELER_TYPE_FACTORS, PRIORITY_LOADING_TIME, MAXIMUM_LOADING_TIME, 
		PRIORITY_WAITING_TIME, MAXIMUM_WAITING_TIME, MAX_DEPARTURE_TIME_VARIANCE, 
		MAX_ARRIVAL_TIME_VARIANCE, RELOAD_CAPACITY_PROBLEMS, COUNT_PROBLEM_WARNINGS, 
		PRINT_PROBLEM_MESSAGES, READ_ALL_PLANS_INTO_MEMORY, NUMBER_OF_TRAVELERS, AVERAGE_LEGS_PER_TRIP
	};
	void Simulator_Service_Keys (int *keys = 0);

	virtual void Program_Control (void);
	virtual void Execute (void);

	Sim_Statistics * Stop_Simulation (void);

	void Global_Data (void);
	Dtime Set_Sim_Period (void);
	bool Output_Step (Travel_Step &step);
	void Remove_Vehicle (Travel_Step &step);

	virtual bool Get_Node_Data (Node_File &file, Node_Data &data);

	void Transit_Vehicles (void);
	void Transit_Plans (void);

	bool Active (void)                     { return (active); }
	void Active (bool flag)                { active = flag; }

	int  Step_Code (void)                  { return (step_code); }
	void Step_Code (Dtime step)            { step_code = (step & 0x7FFF); }

	int  Num_Subareas (void)               { return (num_subareas); }
	void Num_Subareas (int value)          { num_subareas = value; }

	int  Num_Simulators (void)             { return (num_sims); }
	void Num_Simulators (int value)        { num_sims = value; }

	int  Num_Vehicles (void)               { return (num_vehicles); }
	void Num_Vehicles (int value)          { num_vehicles = value; }
	void Add_Vehicles (int value = 1)      { num_vehicles += value; }

	int Offset_Cell (int offset)           { return ((offset > 0) ? (offset - 1) / param.cell_size : 0); }

	Sim_Statistics * Get_Statistics (void);
	Sim_Parameters param;

	int sim_period, max_method;
	Time_Periods sim_periods;
	Shts_Array period_subarea_method;
	bool random_node_flag, read_all_flag;
	bool method_time_flag [MICROSCOPIC + 1];
	Dtime method_time_step [MICROSCOPIC + 1];
	Dtime time_step, end_period, half_second, one_second, one_minute, one_hour, random_time;
	Dtimes period_step_size;

	//---- simulation groups ----

	struct Simulation_Group
	{
		int        group;
		Data_Range subareas;
		Integers   methods;
	};
	typedef vector <Simulation_Group>    Sim_Group_Array;
	typedef Sim_Group_Array::iterator    Sim_Group_Itr;

	Sim_Group_Array sim_group_array;

	Sim_Travel_Array sim_travel_array;

	Vehicle_Map  sim_veh_map;
	Sim_Veh_Array sim_veh_array;

	Sim_Plan_Pool transit_plans;
	Sim_Leg_Pool  transit_legs;

	Sim_Plan_Pool  sim_plan_array;
	Leg_Pool_Array sim_leg_array;

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

	Int2s_Array transfers;

#ifdef MPI_EXE
	Ints_Array   mpi_parts;
	Int2_Array   mpi_range;
	Integers     part_rank;
	Int2_Set     mpi_exchange;
	I2_Ints_Map  mpi_boundary;
#endif

private:
	int num_subareas, max_subarea, num_sims, num_vehicles, transit_id;
	int num_travelers, average_legs, step_code;
	double avg_cell_per_veh;
	bool active;

	Integers node_link, link_list;

	Sim_Statistics stats;
};

extern SYSLIB_API Simulator_Service *sim;

#endif
