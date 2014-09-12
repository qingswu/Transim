//*********************************************************
//	Signal_Timing.cpp - Signal Timing Update Class
//*********************************************************

#include "Sim_Control_Update.hpp"

//---------------------------------------------------------
//	Check_Detector
//---------------------------------------------------------

bool Sim_Control_Update::Check_Detector (int index)
{
	int lane, cell, high, low;
	Detector_Data *detector_ptr;
	Sim_Dir_Ptr sim_dir_ptr;

	detector_ptr = &sim->detector_array [index];

	sim_dir_ptr = &sim->sim_dir_array [detector_ptr->Dir_Index ()];

	if (sim_dir_ptr->Method () < MESOSCOPIC) return (false);

	low = sim->Offset_Cell (detector_ptr->Offset ());
	high = sim->Offset_Cell (detector_ptr->Offset () + detector_ptr->Length ());

	if (high > sim_dir_ptr->Max_Cell ()) high = sim_dir_ptr->Max_Cell ();

	for (lane = detector_ptr->Low_Lane (); lane <= detector_ptr->High_Lane (); lane++) {
		for (cell = low; cell <= high; cell++) {
			if (sim_dir_ptr->Get (lane, cell) > 0) return (true);
		}
	}
	return (false);
}
