//*********************************************************
//	Sim_Dir_Data.cpp - direction link data
//*********************************************************

#include "Sim_Dir_Data.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Lock
//---------------------------------------------------------

void Sim_Dir_Array::Lock (Sim_Dir_Ptr ptr, int id)
{
#ifdef THREADS
	mutex_lock lock (lock_mutex);

	if (id > 0 && ptr->Lock () == id) {
#ifdef CHECK
		sim->Error ("Sim_Dir_Array::Lock: Already Locked");
#endif
		return;
	}

	while (ptr->Lock () != 0) {
		lock_wait.wait (lock);
	}
#endif
	ptr->Lock ((id > 0) ? id : 1);
}

//---------------------------------------------------------
//	UnLock
//---------------------------------------------------------

void Sim_Dir_Array::UnLock (Sim_Dir_Ptr ptr, int id)
{
#ifdef THREADS
	mutex_lock lock (lock_mutex);

	if (id > 0 && ptr->Lock () != id) {
#ifdef CHECK
		sim->Error ("Sim_Dir_Array::UnLock: Not Owned");
#endif
		return;
	}
	lock_wait.notify_all ();
#else
	id = 0;
#endif
	ptr->Lock (0);
}
