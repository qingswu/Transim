//*********************************************************
//	Flow_Time_Service.hpp - flow-time service keys and data
//*********************************************************

#ifndef FLOW_TIME_SERVICE_HPP
#define FLOW_TIME_SERVICE_HPP

#include "APIDefs.hpp"
#include "TypeDefs.hpp"
#include "Equation.hpp"
#include "Data_Service.hpp"

//---------------------------------------------------------
//	Flow_Time_Service - flow time class definition
//---------------------------------------------------------

class SYSLIB_API Flow_Time_Service
{
public:

	Flow_Time_Service (void);

	enum Flow_Time_Service_Keys { 
		 UPDATE_FLOW_RATES = FLOW_TIME_SERVICE_OFFSET, UPDATE_TURNING_MOVEMENTS, CLEAR_INPUT_FLOW_RATES, 
		 UPDATE_TRAVEL_TIMES, TIME_UPDATE_RATE, LINK_FLOW_FACTOR, EQUATION_PARAMETERS
	};

	void Flow_Time_Service_Keys (int *keys = 0);

	void Build_Perf_Arrays (Perf_Period_Array &link_array);
	void Build_Perf_Arrays (void) { Build_Perf_Arrays (dat->perf_period_array); }
	
	void Build_Turn_Arrays (Turn_Period_Array &turn_array);
	void Build_Turn_Arrays (void)      { Build_Turn_Arrays (dat->turn_period_array); }

	bool Flow_Updates (void)         { return (flow_updates); }
	void Flow_Updates (bool flag)    { flow_updates = flag; }

	bool Turn_Updates (void)         { return (turn_updates); }
	void Turn_Updates (bool flag)    { turn_updates = flag; }

	bool Time_Updates (void)         { return (time_updates); }
	void Time_Updates (bool flag)    { time_updates = flag; }

protected:

	void Read_Flow_Time_Keys (void);
	
	void Update_Travel_Times (int mpi_size = 1, Dtime first_time = 0, bool avg_times = false, bool zero_flows = false);
	void Update_Travel_Times (bool avg_times, bool zero_flows)  { Update_Travel_Times (1, 0, avg_times, zero_flows); }
	void Update_Travel_Times (bool avg_times)                   { Update_Travel_Times (1, 0, avg_times); }

	bool flow_updates, turn_updates, time_updates;
	int update_rate;
	double flow_factor;

	Equation_Array equation;

private:

};

#endif
