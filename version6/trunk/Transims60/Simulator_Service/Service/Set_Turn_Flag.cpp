//*********************************************************
//	Set_Turn_Flag.cpp - flag active turning penalties
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Set_Turn_Flag
//---------------------------------------------------------

void Simulator_Service::Set_Turn_Flag (Sim_Dir_Data *sim_dir_ptr, Dir_Data *dir_ptr)
{
	int index;
	Turn_Pen_Data *turn_ptr;

	//---- set the turn prohibition flag ----

	sim_dir_ptr->Turn (false);

	for (index = dir_ptr->First_Turn (); index >= 0; index = turn_ptr->Next_Index ()) {
		turn_ptr = &turn_pen_array [index];

		if (turn_ptr->Penalty () != 0) continue;

		if (turn_ptr->Start () > time_step) {
			if (turn_ptr->Start () < turn_update_time) {
				turn_update_time = turn_ptr->Start ();
			}
		} else if (time_step < turn_ptr->End ()) {
			if (turn_ptr->End () < turn_update_time) {
				turn_update_time = turn_ptr->End ();
			}
			sim_dir_ptr->Turn (true);
			break;
		}
	}
}
