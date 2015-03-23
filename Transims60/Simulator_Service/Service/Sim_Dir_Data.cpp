//*********************************************************
//	Sim_Dir_Data.cpp - direction link data
//*********************************************************

#include "Sim_Dir_Data.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Load_Queue
//---------------------------------------------------------

void Sim_Dir_Data::Load_Queue (int traveler)
{
	sim->sim_travel_array [traveler].Next_Load (-1);

	sim->sim_dir_array.Lock (this);
//sim->Warning (String ("Sim_Dir_Data::Load_Queue: Lock %d, dir=%d") % this->Lock () % this->Dir ());
	if (first_load < 1) {
		first_load = last_load = traveler;
	} else {
#ifdef CHECK
		if (last_load < 1 || last_load >= (int) sim->sim_travel_array.size ()) {
			sim->Error (String ("Sim_Dir_Data::Load_Queue: Last_Load=%d vs %d") % last_load % sim->sim_travel_array.size ());
		}
#endif
		Sim_Travel_Ptr ptr = &sim->sim_travel_array [last_load];
#ifdef CHECK
		if (ptr->Next_Load () > 0) sim->Error (String ("Sim_Dir_Data::Load_Queue: Next_Load=%d") % ptr->Next_Load ());
#endif
		ptr->Next_Load (traveler);
		last_load = traveler;
	}
//sim->Warning (String ("Sim_Dir_Data::Load_Queue: UnLock %d, dir=%d") % this->Lock () % this->Dir ());
	sim->sim_dir_array.UnLock (this);
}

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
		sim->Error (String ("Sim_Dir_Array::UnLock: Not Owned (%d vs %d)") % id % ptr->Lock ());
#endif
		return;
	}
	lock_wait.notify_all ();
#else
	id = 0;
#endif
	ptr->Lock (0);
}
