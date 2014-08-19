//*********************************************************
//	Sim_Travel_Process.hpp - travel event manager
//*********************************************************

#ifndef SIM_TRAVEL_PROCESS_HPP
#define SIM_TRAVEL_PROCESS_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Sim_Statistics.hpp"
#include "Sim_Travel_Data.hpp"
#include "Work_Queue.hpp"
#include "Threads.hpp"

//---------------------------------------------------------
//	Sim_Travel_Process - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Travel_Process : Static_Service
{
public:
	Sim_Travel_Process (void);

#ifdef THREADS
	Sim_Travel_Process (Work_Queue *queue, int id = 0); 

	Work_Queue *travel_queue;

	void operator()();
#endif

	void Travel_Processing (Sim_Travel_Ptr sim_travel_ptr);

	int  ID (void)                         { return (id); }

	Sim_Statistics & Get_Statistics (void) { return (stats); }

private:
	int id;
	Sim_Statistics stats;
};

#endif
