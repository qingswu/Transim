//*********************************************************
//	Sim_Output_Step.hpp - simulate output processing
//*********************************************************

#ifndef SIM_OUTPUT_STEP_HPP
#define SIM_OUTPUT_STEP_HPP

#include "Sim_Output_Data.hpp"
#include "Problem_Output.hpp"
#include "Threads.hpp"
#include "Work_Queue.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Output_Step - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Output_Step : Static_Service
{
	friend class Snapshot_Output;
	friend class Performance_Output;
	friend class Turn_Delay_Output;
	friend class Ridership_Output;
	friend class Occupancy_Output;
	friend class Event_Output;
	friend class Traveler_Output;
	friend class Problem_Output;

public:
	Sim_Output_Step (void);
	~Sim_Output_Step (void);

	void Initialize (void);

	void Add_Keys (void);
	void Read_Controls (void);
	
	void Start_Processing (void);
	void Stop_Processing (void);

	void Output_Check (Travel_Step &travel);

	void Output_Problem (Problem_Data &problem);

	void Event_Check (Travel_Step &travel);

protected:
	enum Snaptshot_Keys { 
		NEW_SNAPSHOT_FILE = SNAPSHOT_OUTPUT_OFFSET, NEW_SNAPSHOT_FORMAT, NEW_SNAPSHOT_TIME_FORMAT,
		NEW_SNAPSHOT_INCREMENT, NEW_SNAPSHOT_TIME_RANGE, NEW_SNAPSHOT_LINK_RANGE, NEW_SNAPSHOT_SUBAREA_RANGE,
		NEW_SNAPSHOT_COORDINATES, NEW_SNAPSHOT_MAX_SIZE, NEW_SNAPSHOT_LOCATION_FLAG, NEW_SNAPSHOT_CELL_FLAG,
		NEW_SNAPSHOT_STATUS_FLAG, NEW_SNAPSHOT_COMPRESSION
	};
	enum Performance_Keys { 
		NEW_PERFORMANCE_FILE = PERFORMANCE_OUTPUT_OFFSET, NEW_PERFORMANCE_FORMAT, 
		NEW_PERFORMANCE_TIME_FORMAT, NEW_PERFORMANCE_INCREMENT, NEW_PERFORMANCE_TIME_RANGE, 
		NEW_PERFORMANCE_LINK_RANGE, NEW_PERFORMANCE_SUBAREA_RANGE, NEW_PERFORMANCE_COORDINATES, 
		NEW_PERFORMANCE_VEH_TYPES, NEW_PERFORMANCE_LANE_USE_FLAG
	};	
	enum Turn_Delay_Keys { 
		NEW_TURN_DELAY_FILE = TURN_DELAY_OUTPUT_OFFSET, NEW_TURN_DELAY_FORMAT, 
		NEW_TURN_DELAY_FILTER, NEW_TURN_DELAY_TIME_FORMAT, NEW_TURN_DELAY_INCREMENT, 
		NEW_TURN_DELAY_TIME_RANGE, NEW_TURN_DELAY_NODE_RANGE, NEW_TURN_DELAY_SUBAREA_RANGE
	};
	enum Ridership_Keys { 
		NEW_RIDERSHIP_FILE = RIDERSHIP_OUTPUT_OFFSET, NEW_RIDERSHIP_FORMAT, NEW_RIDERSHIP_TIME_FORMAT,
		NEW_RIDERSHIP_TIME_RANGE, NEW_RIDERSHIP_ROUTE_RANGE, NEW_RIDERSHIP_ALL_STOPS
	};
	enum Occupancy_Keys { 
		NEW_OCCUPANCY_FILE = OCCUPANCY_OUTPUT_OFFSET, NEW_OCCUPANCY_FORMAT, NEW_OCCUPANCY_TIME_FORMAT,
		NEW_OCCUPANCY_INCREMENT, NEW_OCCUPANCY_TIME_RANGE, NEW_OCCUPANCY_LINK_RANGE, NEW_OCCUPANCY_SUBAREA_RANGE, 
		NEW_OCCUPANCY_COORDINATES, NEW_OCCUPANCY_MAX_FLAG
	};
	enum Event_Keys { 
		NEW_EVENT_FILE = EVENT_OUTPUT_OFFSET, NEW_EVENT_FORMAT, NEW_EVENT_FILTER, 
		NEW_EVENT_TIME_FORMAT, NEW_EVENT_TIME_RANGE, NEW_EVENT_TYPE_RANGE, NEW_EVENT_MODE_RANGE, 
		NEW_EVENT_LINK_RANGE, NEW_EVENT_SUBAREA_RANGE, NEW_EVENT_COORDINATES 
	};
	enum Traveler_Keys { 
		NEW_TRAVELER_FILE = TRAVELER_OUTPUT_OFFSET, NEW_TRAVELER_FORMAT, NEW_TRAVELER_ID_RANGE, 
		NEW_TRAVELER_TIME_FORMAT, NEW_TRAVELER_TIME_RANGE, NEW_TRAVELER_MODE_RANGE, 
		NEW_TRAVELER_LINK_RANGE, NEW_TRAVELER_SUBAREA_RANGE, NEW_TRAVELER_COORDINATES 
	};

private:
	int num_outputs;
	bool problem_flag;

	typedef vector <Sim_Output_Data *>    Output_Array;
	typedef Output_Array::iterator        Output_Itr;

	Output_Array output_array;

#ifdef THREADS
	int num_threads;
	Threads threads;
	Work_Queue output_queue;

	//---------------------------------------------------------
	//	Sim_Output_Process - output class definition
	//---------------------------------------------------------

	class Sim_Output_Process
	{
	public:
		Sim_Output_Process (Sim_Output_Step *ptr) 	{ step_ptr = ptr; }
		Sim_Output_Step *step_ptr;

		void operator()();

	} **sim_output_process;
#endif
};
#endif
