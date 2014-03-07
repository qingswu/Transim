//*********************************************************
//	Sim_Output_Step.hpp - simulate output processing
//*********************************************************

#ifndef SIM_OUTPUT_STEP_HPP
#define SIM_OUTPUT_STEP_HPP

#include "Sim_Output_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Output_Step - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Output_Step : Static_Service
{
	friend class Sim_Output_Data;	
	friend class Snapshot_Output;
	friend class Link_Delay_Output;
	friend class Performance_Output;
	friend class Turn_Vol_Output;
	friend class Ridership_Output;
	friend class Occupancy_Output;
	friend class Event_Output;
	friend class Traveler_Output;

public:
	Sim_Output_Step (void);
	~Sim_Output_Step (void)       { End_Output (); }

	void Initialize (void);

	void Add_Keys (void);
	void Read_Controls (void);

	bool Step_Output (void);
	void End_Output (void);

	void Check_Output (Travel_Step &travel);

protected:
	enum Snaptshot_Keys { 
		NEW_SNAPSHOT_FILE = SNAPSHOT_OUTPUT_OFFSET, NEW_SNAPSHOT_FORMAT, NEW_SNAPSHOT_TIME_FORMAT,
		NEW_SNAPSHOT_INCREMENT, NEW_SNAPSHOT_TIME_RANGE, NEW_SNAPSHOT_LINK_RANGE, NEW_SNAPSHOT_SUBAREA_RANGE,
		NEW_SNAPSHOT_COORDINATES, NEW_SNAPSHOT_MAX_SIZE, NEW_SNAPSHOT_LOCATION_FLAG, NEW_SNAPSHOT_CELL_FLAG,
		NEW_SNAPSHOT_STATUS_FLAG, NEW_SNAPSHOT_COMPRESSION
	};
	enum Link_Delay_Keys { 
		NEW_LINK_DELAY_FILE = LINK_DELAY_OUTPUT_OFFSET, NEW_LINK_DELAY_FORMAT, 
		NEW_LINK_DELAY_TIME_FORMAT, NEW_LINK_DELAY_INCREMENT, NEW_LINK_DELAY_TIME_RANGE, 
		NEW_LINK_DELAY_LINK_RANGE, NEW_LINK_DELAY_SUBAREA_RANGE, NEW_LINK_DELAY_COORDINATES, 
		NEW_LINK_DELAY_VEH_TYPES, NEW_LINK_DELAY_TURN_FLAG, NEW_LINK_DELAY_FLOW_TYPE,
		NEW_LINK_DELAY_LANE_FLOWS
	};
	enum Performance_Keys { 
		NEW_PERFORMANCE_FILE = PERFORMANCE_OUTPUT_OFFSET, NEW_PERFORMANCE_FORMAT, 
		NEW_PERFORMANCE_TIME_FORMAT, NEW_PERFORMANCE_INCREMENT, NEW_PERFORMANCE_TIME_RANGE, 
		NEW_PERFORMANCE_LINK_RANGE, NEW_PERFORMANCE_SUBAREA_RANGE, NEW_PERFORMANCE_COORDINATES, 
		NEW_PERFORMANCE_VEH_TYPES, NEW_PERFORMANCE_TURN_FLAG, NEW_PERFORMANCE_FLOW_TYPE,
		NEW_PERFORMANCE_LANE_FLOWS
	};	
	enum Turn_Vol_Keys { 
		NEW_TURN_VOLUME_FILE = TURN_VOL_OUTPUT_OFFSET, NEW_TURN_VOLUME_FORMAT, 
		NEW_TURN_VOLUME_FILTER, NEW_TURN_VOLUME_TIME_FORMAT, NEW_TURN_VOLUME_INCREMENT, 
		NEW_TURN_VOLUME_TIME_RANGE, NEW_TURN_VOLUME_NODE_RANGE, NEW_TURN_VOLUME_SUBAREA_RANGE
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
	int num_threads;

	typedef vector <Sim_Output_Data *>    Output_Array;
	typedef Output_Array::iterator        Output_Itr;

	Output_Array output_array;
	
	Barrier output_barrier;

#ifdef THREADS
	Threads threads;
#endif
};
#endif
