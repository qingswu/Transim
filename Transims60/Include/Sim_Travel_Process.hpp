//*********************************************************
//	Sim_Travel_Process.hpp - simulate travel events
//*********************************************************

#ifndef SIM_TRAVEL_PROCESS_HPP
#define SIM_TRAVEL_PROCESS_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Sim_Statistics.hpp"
#include "Sim_Travel_Data.hpp"
#include "Travel_Step.hpp"

#ifdef THREADS
#include "Ordered_Work.hpp"

typedef Ordered_Work <Sim_Travel_Data, Sim_Travel_Data> Travel_Queue;
#endif

//---------------------------------------------------------
//	Sim_Travel_Process - execution class definition
//---------------------------------------------------------

class SYSLIB_API Sim_Travel_Process : Static_Service
{
public:
	Sim_Travel_Process (void)  { Initialize (); }

#ifdef THREADS
	Sim_Travel_Process (Travel_Queue &queue) 
	{ 
		travel_queue = &queue; 
		Initialize (); 
	}
	Travel_Queue *travel_queue;

	void operator()();
#endif
	void Initialize (void);

	bool Travel_Processing (Sim_Travel_Ptr sim_travel_ptr);
	bool Travel_Update (Travel_Step &step);

	Sim_Statistics & Get_Statistics (void) { return (stats); }

private:
	Sim_Statistics stats;
};
#endif
