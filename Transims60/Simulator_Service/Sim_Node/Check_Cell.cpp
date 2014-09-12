//*********************************************************
//	Check_Cell.cpp - check the cell occupancy and use restrictions
//*********************************************************

#include "Sim_Node_Process.hpp"

//---------------------------------------------------------
//	Check_Cell
//---------------------------------------------------------

bool Sim_Node_Process::Check_Cell (Sim_Dir_Ptr sim_dir_ptr, int lane, int cell, Travel_Step &step)
{
	if (lane < 0 || lane >= sim_dir_ptr->Lanes () || cell < 0 || cell > sim_dir_ptr->Max_Cell ()) return (false);
	if (!sim_dir_ptr->Check (lane, cell)) return (false);
	if (cell <= sim_dir_ptr->In_Cell () || cell >= sim_dir_ptr->Out_Cell ()) return (true);
	return (Cell_Use (sim_dir_ptr, lane, cell, step));
}
