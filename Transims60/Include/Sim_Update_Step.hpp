//*********************************************************
//	Sim_Update_Step.hpp - update simulation environment
//*********************************************************

#ifndef SIM_UPDATE_STEP_HPP
#define SIM_UPDATE_STEP_HPP

#include "Sim_Update_Data.hpp"
#include "Threads.hpp"
#include "Work_Queue.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Update_Step - update the temporal attributes
//---------------------------------------------------------

class SYSLIB_API Sim_Update_Step : public Static_Service
{
public:
	Sim_Update_Step (void);
	~Sim_Update_Step (void);

	void Initialize (void);
	void Start_Processing (void);
	void Stop_Processing (void);

private:
	int num_updates;

	typedef vector <Sim_Update_Data *>    Update_Array;
	typedef Update_Array::iterator        Update_Itr;

	Update_Array update_array;

#ifdef THREADS
	int num_threads;
	Threads threads;
	Work_Queue update_queue;

	//---------------------------------------------------------
	//	Sim_Update_Process - update class definition
	//---------------------------------------------------------

	class Sim_Update_Process
	{
	public:
		Sim_Update_Process (Sim_Update_Step *ptr) 	{ step_ptr = ptr; }
		Sim_Update_Step *step_ptr;

		void operator()();

	} **sim_update_process;
#endif
};
#endif
