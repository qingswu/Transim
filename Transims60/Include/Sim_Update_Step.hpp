//*********************************************************
//	Sim_Update_Step.hpp - update simulation environment
//*********************************************************

#ifndef SIM_UPDATE_STEP_HPP
#define SIM_UPDATE_STEP_HPP

#include "Sim_Update_Data.hpp"

#include <vector>
using namespace std;

//---------------------------------------------------------
//	Sim_Update_Step - update the temporal attributes
//---------------------------------------------------------

class SYSLIB_API Sim_Update_Step : public Static_Service
{
	friend class Sim_Update_Data;	
	friend class Sim_Control_Data;
	friend class Sim_Network_Data;
	friend class Sim_Transit_Data;

public:
	Sim_Update_Step (void);
	~Sim_Update_Step (void)      { Stop_Processing (); }

	void Initialize (void);
	void Start_Processing (void);
	void Stop_Processing (void);

private:
	int num_threads;

	typedef vector <Sim_Update_Data *>    Update_Array;
	typedef Update_Array::iterator        Update_Itr;

	Update_Array update_array;
	
	Barrier update_barrier;

#ifdef THREADS
	Threads threads;
#endif
};
#endif
