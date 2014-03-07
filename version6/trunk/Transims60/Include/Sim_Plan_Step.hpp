//*********************************************************
//	Sim_Plan_Step.hpp - read and process travel plans
//*********************************************************

#ifndef SIM_PLAN_STEP_HPP
#define SIM_PLAN_STEP_HPP

#include "APIDefs.hpp"
#include "Static_Service.hpp"
#include "Sim_Plan_Process.hpp"
#include "TypeDefs.hpp"
#include "Plan_Data.hpp"
#include "Plan_File.hpp"
#include "Partition_Files.hpp"

//---------------------------------------------------------
//	Sim_Plan_Step - read and process travel plans
//---------------------------------------------------------

class SYSLIB_API Sim_Plan_Step : public Static_Service
{
public:
	Sim_Plan_Step (void);
	~Sim_Plan_Step (void)      { Stop_Processing (); }

	void Initialize (void);
	void Stop_Processing (void);

	bool First_Plan (void);
	bool Read_Plans (void);

private:
	int num_files, first_num;
	bool stat, first;

	Integers next;
	Partition_Files <Plan_File> file_set;
	Partition_Data <Plan_Data> plan_set;
	Partition_Data <Time_Index> time_set;

	bool Sim_Plan_Result (Sim_Travel_Ptr ptr);

#ifdef THREADS
	int num_threads;
	Threads threads;
	Plan_Queue plan_queue;
	Sim_Plan_Process **sim_plan_process;

	//---------------------------------------------------------
	//	Save_Results - process the event results
	//---------------------------------------------------------

	class Save_Results
	{
	public:
		Save_Results (Sim_Plan_Step *_ptr = 0)   { ptr = _ptr; }

		void Initialize (Sim_Plan_Step *_ptr)    { ptr = _ptr; };

		void operator()();

		Sim_Plan_Step *ptr;
	} save_results;

#else
	Sim_Plan_Process sim_plan_process;
#endif
};
#endif