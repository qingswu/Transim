//*********************************************************
//	Check_Behind.cpp - check speed constraints from behind
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Sim_Connection.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Check_Behind
//---------------------------------------------------------

bool Sim_Node_Process::Check_Behind (int link, int lane, int cell, Travel_Step &step, int num_cells, bool use_flag)
{
	int index, control, driver, traveler;
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

#ifdef CHECK
	if (link < 0 || link >= (int) sim->sim_dir_array.size ()) 
		sim->Error ("Sim_Node_Process::Check_Behind: link");
#endif	
	sim_dir_ptr = &sim->sim_dir_array [link];

	if (sim_dir_ptr->Method () < MESOSCOPIC) return (true);

	//---- check the use restrictions ----

	if (cell >= sim_dir_ptr->In_Cell () && cell < sim_dir_ptr->Out_Cell () && !use_flag) {
		if (!Cell_Use (sim_dir_ptr, lane, cell, step)) return (false);
	}

	//---- set the search and gap requirements ----
#ifdef CHECK
	if (step.veh_type_ptr == 0) sim->Error ("Sim_Node_Process::Check_Behind: veh_type_ptr");
#endif
	min_gap = (num_cells - 1) * sim->param.cell_size;
	max_gap = (num_cells + step.veh_type_ptr->Cells ()) * sim->param.cell_size;
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
	index = link;
	link_flag = false;
	max_cells = max_step * sim->Offset_Cell (sim_dir_ptr->Speed ());
	min_cell = -1;

	//---- find the vehicle behind ----

	gap = 0;
	traveler = -1;

	for (--cell; cell >= 0; cell--) {
#ifdef CHECK
		if (lane < 0 || lane >= sim_dir_ptr->Lanes ()) 
			sim->Error (String ("Sim_Node_Process::Check_Behind: lane=%d") % lane);
		if (cell < 0 || cell >= sim_dir_ptr->Max_Cell ()) 
			sim->Error (String ("Sim_Node_Process::Check_Behind: cell=%d") % cell);
#endif
		traveler = sim_dir_ptr->Get (lane, cell);
		if (traveler > 0) break;
	}

	//---- check for your own tail ----

	if (traveler < 2 || traveler == step.Traveler ()) return (true);
#ifdef CHECK
	if (traveler < 2 || traveler >= (int) sim->sim_travel_array.size ()) 
		sim->Error (String ("Sim_Node_Process::Check_Behind: traveler=%d") % traveler);
#endif
	sim_travel_ptr = &sim->sim_travel_array [traveler];

#ifdef CHECK
	if (sim_travel_ptr->Vehicle () < 1 || sim_travel_ptr->Vehicle () >= (int) sim->sim_veh_array.size ()) 
		sim->Error (String ("Sim_Node_Process::Check_Behind: vehicle=%d") % sim_travel_ptr->Vehicle ());
#endif
	veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];

	speed = sim_travel_ptr->Speed ();

	sim_plan_ptr = sim_travel_ptr->Get_Plan ();
#ifdef CHECK
	if (sim_plan_ptr == 0) sim->Error (String ("Sim_Node_Process::Check_Behind: sim_plan_ptr traveler=%d") % sim_travel_ptr->Traveler ());
	if (sim_plan_ptr->Veh_Type () < 0 || sim_plan_ptr->Veh_Type () >= (int) sim->veh_type_array.size ()) 
		sim->Error (String ("Sim_Node_Process::Check_Behind: veh_type=%d") % sim_plan_ptr->Veh_Type ());
#endif
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
			cell = sim->Offset_Cell (speed);
			min_gap -= cell;
		}
		speed = sim_travel_ptr->Speed ();
	} else {
		min_step = 0;
	}

	if (sim_travel_ptr->Next_Event () > sim->time_step) {
		min_step += sim_travel_ptr->Next_Event () - sim->time_step;
	}
	if (min_step > max_step) return (true);

	//---- priority vehicle check ----

	if (step.sim_travel_ptr->Priority () > 1) {
		return (speed == 0 || min_step > max_step / 2);
	}

	//---- non-priority vehicle check ---
	
	sim_travel_ptr = &sim->sim_travel_array [traveler];

	if ((speed == 0 && gap == 0) || min_step == max_step) {

		//---- relative priority check ----

		if (sim_travel_ptr->Priority () != step.sim_travel_ptr->Priority ()) {
			return (sim_travel_ptr->Priority () < step.sim_travel_ptr->Priority ());
		}

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
#ifdef CHECK
		if (leg_ptr->Connect () < 0 || leg_ptr->Connect () >= (int) sim->connect_array.size ()) 
			sim->Error ("Sim_Node_Process::Check_Behind: leg connect");
#endif

		connect_ptr = &sim->connect_array [leg_ptr->Connect ()];
		if (connect_ptr->To_Index () != link) return (true);

		//---- check for a traffic control ----

		if (cell <= sim_dir_ptr->Out_Cell ()) {
			control = connect_ptr->Control ();

			if (control == RED_LIGHT || control == STOP_SIGN || control == STOP_GREEN) {
				return (true);
			}
		}
	}
	return (false);
}
