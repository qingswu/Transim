//*********************************************************
//	Signal_Timing.cpp - Signal Timing Update Class
//*********************************************************

#include "Sim_Control_Update.hpp"

//---------------------------------------------------------
//	Check_Detector
//---------------------------------------------------------

bool Sim_Control_Update::Check_Detector (int index)
{
	//int lane, cell, high, low;
	int high, low;
	Detector_Data *detector_ptr;
	Sim_Dir_Ptr sim_dir_ptr;
	//Sim_Veh_Ptr sim_veh_ptr;

	detector_ptr = &sim->detector_array [index];

	sim_dir_ptr = &sim->sim_dir_array [detector_ptr->Dir_Index ()];

	low = detector_ptr->Offset () / sim->param.cell_size;
	high = (detector_ptr->Offset () + detector_ptr->Length () + sim->param.cell_size - 1) / sim->param.cell_size;

	//if (high >= sim_dir_ptr->Cells ()) high = sim_dir_ptr->Cells () - 1;

	//if (sim_dir_ptr->Method () == MACROSCOPIC) {
	//	sim_cap_ptr = sim_dir_ptr->Sim_Cap ();

	//	if (sim_cap_ptr->First_Veh () >= 0) {
	//		sim_veh_ptr = exe->sim_veh_array [sim_cap_ptr->First_Veh ()];
	//		if (sim_veh_ptr == 0) return (false);

	//		cell = sim_veh_ptr->begin ()->Cell ();
	//		//if (cell >= low && cell <= high) return (true);
	//		if (cell >= low) return (true);
	//	}
	//} else if (sim_dir_ptr->Method () == MESOSCOPIC) {
	//	for (lane = detector_ptr->Low_Lane (); lane <= detector_ptr->High_Lane (); lane++) {
	//		sim_lane_ptr = sim_dir_ptr->Lane (lane);

	//		for (cell = low; cell <= high; cell++) {
	//			if (sim_lane_ptr->at (cell) >= 0) return (true);
	//		}
	//	}
	//}
	return (false);
}
