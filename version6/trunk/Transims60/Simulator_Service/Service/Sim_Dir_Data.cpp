//*********************************************************
//	Sim_Dir_Data.cpp - direction link data
//*********************************************************

#include "Sim_Dir_Data.hpp"

//---------------------------------------------------------
//	Lock
//---------------------------------------------------------

void Sim_Dir_Array::Lock (Sim_Dir_Ptr ptr, int id)
{
	if (id > 0 && ptr->Lock () == id) return;

#ifdef THREADS
	mutex_lock lock (lock_mutex);

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
	if (id > 0 && ptr->Lock () != id) return;
	ptr->Lock (0);

#ifdef THREADS
	lock_wait.notify_all ();
#endif
}
