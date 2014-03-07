//*********************************************************
//	Sim_Travel_Step.hpp - travel event manager
//*********************************************************

#ifndef SIM_TRAVEL_STEP_HPP
#define SIM_TRAVEL_STEP_HPP

#include "APIDefs.hpp"
#include "Threads.hpp"
#include "Static_Service.hpp"
#include "Sim_Travel_Process.hpp"

//---------------------------------------------------------
//	Sim_Travel_Step - process travel events
//---------------------------------------------------------

class SYSLIB_API Sim_Travel_Step : public Static_Service
{
public:
	Sim_Travel_Step (void);
	~Sim_Travel_Step (void)   { Stop_Processing (); }

	void Initialize (void);
	int  Start_Processing (void);
	void Stop_Processing (void);

	void Add_Statistics (Sim_Statistics &_stats);

private:

#ifdef THREADS
	int num_threads;
	Threads threads;
	Travel_Queue travel_queue;
	Sim_Travel_Process **sim_travel_process;

	//---------------------------------------------------------
	//	Save_Results - process the event results
	//---------------------------------------------------------

	class Save_Results
	{
	public:
		Save_Results (Sim_Travel_Step *_ptr = 0)   { ptr = _ptr; }

		void Initialize (Sim_Travel_Step *_ptr)    { ptr = _ptr; };

		void operator()();

		Sim_Travel_Step *ptr;
	} save_results;

#else
	Sim_Travel_Process sim_travel_process;
#endif
};
#endif
