//*********************************************************
//	Check_Behind.cpp - check speed constraints from behind
//*********************************************************

#include "Sim_Link_Process.hpp"
#include "Sim_Connection.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Check_Behind
//---------------------------------------------------------

bool Sim_Link_Process::Check_Behind (Sim_Veh_Data veh_data, Travel_Step &step, int num_cells, bool use_flag)
{
	int dir_index, lane, offset, index, control, driver, cell, traveler;
	int speed, gap, min_gap, max_gap, max_cells, min_step, max_step, min_cell;
	double permission;
	bool link_flag;

	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Ptr leg_ptr;
	Veh_Type_Data *veh_type_ptr;
	Sim_Veh_Ptr veh_ptr;
	Connect_Data *connect_ptr;

	//---- check the lane range ----

	if (veh_data.Parked ()) return (false);

	dir_index = veh_data.link;
	lane = veh_data.lane;
	offset = veh_data.offset;
	cell = sim->Offset_Cell (offset);

	sim_dir_ptr = &sim->sim_dir_array [dir_index];

	if (sim_dir_ptr->Method () < MESOSCOPIC) return (true);

	index = sim_dir_ptr->Index (lane, cell);

	//---- check the use restrictions ----

	if (offset >= sim_dir_ptr->In_Offset () && offset < sim_dir_ptr->Out_Offset () && !use_flag) {
		if (!Cell_Use (sim_dir_ptr, lane, cell, step)) return (false);
	}

	//---- set the search and gap requirements ----

	min_gap = num_cells - 1;
	max_gap = num_cells + step.veh_type_ptr->Cells ();
	speed = step.sim_travel_ptr->Speed ();

	for (min_step=0, max_step=1; max_gap > 0; max_step++) {
		speed += step.veh_type_ptr->Max_Accel ();
		if (speed > sim_dir_ptr->Speed ()) {
			speed = sim_dir_ptr->Speed ();
		}
		if (speed > step.veh_type_ptr->Max_Speed ()) {
			speed = step.veh_type_ptr->Max_Speed ();
		}
		max_gap -= speed;
		min_gap -= speed;
		if (min_gap > 0) min_step++;
	}

	//---- search for upstream vehicles ----

	driver = -1;
	index = dir_index;
	link_flag = false;
	max_cells = max_step * sim_dir_ptr->Speed ();

	//if (!boundary_flag && sim_dir_ptr->Boundary ()) {
	//	min_cell = sim_dir_ptr->Split_Offset ();
	//	if (offset == min_cell) return (true);
	//	if (offset < min_cell) min_cell = -1;
	//} else {
		min_cell = -1;
	//}

	//---- find the vehicle behind ----

	gap = 0;
	traveler = -1;

	for (--cell; cell >= 0; cell--) {
		traveler = sim_dir_ptr->Get (lane, cell);
		if (traveler >= 0) break;
	}

	//---- check for your own tail ----

	if (traveler == step.Traveler ()) return (true);

	sim_travel_ptr = &sim->sim_travel_array [traveler];

	veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];

	speed = sim_travel_ptr->Speed ();

	sim_plan_ptr = sim_travel_ptr->Get_Plan ();

	if (gap > 0) {
		
		//---- check gap / speed relationships ----

		min_gap = gap;
		veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];

		for (min_step=0; min_gap > 0; min_step++) {
			speed += veh_type_ptr->Max_Accel ();
			if (speed > sim_dir_ptr->Speed ()) {
				speed = sim_dir_ptr->Speed ();
			}
			if (speed > veh_type_ptr->Max_Speed ()) {
				speed = veh_type_ptr->Max_Speed ();
			}
			min_gap -= speed;
		}
		speed = sim_travel_ptr->Speed ();
	} else {
		min_step = 0;
	}
	//if (sim->veh_status [vehicle] != 0) min_step++;
	if (sim_travel_ptr->Next_Event () > sim->time_step) {
		min_step += sim_travel_ptr->Next_Event () - sim->time_step;
	}
	if (min_step > max_step) return (true);

	//---- priority vehicle check ----

	//if (step.sim_veh_ptr->Priority () > 1) {
	//	return (speed == 0 || min_step > max_step / 2);
	//}

	//---- non-priority vehicle check ---
	
	sim_travel_ptr = &sim->sim_travel_array [traveler];

	if ((speed == 0 && gap == 0) || min_step == max_step) {

		//---- relative priority check ----

		//if (veh_ptr->Priority () != step.sim_veh_ptr->Priority ()) {
		//	return (veh_ptr->Priority () < step.sim_veh_ptr->Priority ());
		//}

		//---- permissive probablity ----

		permission = sim->param.permission_prob [sim_dir_ptr->Type ()];
		if (sim->param.traveler_flag) permission *= sim->param.traveler_fac.Best (sim_plan_ptr->Type ());

		return (sim_travel_ptr->random.Probability () <= permission);	
	}

	//---- check the link change ----

	if (link_flag) {

		//---- check the traveler's path ----

		if (sim_plan_ptr == 0) return (true);

		leg_ptr = sim_plan_ptr->Get_Leg ();
		if (leg_ptr == 0 || leg_ptr->Connect () < 0) return (true);
		
		connect_ptr = &sim->connect_array [leg_ptr->Connect ()];
		if (connect_ptr->To_Index () != dir_index) return (true);

		//---- check for a traffic control ----

		if (offset <= sim_dir_ptr->Out_Offset ()) {
			control = connect_ptr->Control ();

			if (control == RED_LIGHT || control == STOP_SIGN || control == STOP_GREEN) {
				return (true);
			}
		}
	}
	return (false);
}
