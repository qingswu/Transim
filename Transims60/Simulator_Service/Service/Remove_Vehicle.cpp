//*********************************************************
//	Remove_Vehicle.cpp - remove vehicle cells from the links
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Output_Step
//---------------------------------------------------------

void Simulator_Service::Remove_Vehicle (Travel_Step &step)
{
	int i, index, lane, cell, dir_index, traveler;
	bool park_flag;

	Sim_Dir_Ptr sim_dir_ptr = 0;
	Sim_Veh_Ptr sim_veh_ptr;
	Veh_Type_Data *veh_type_ptr;
	Sim_Plan_Ptr sim_plan_ptr;

	//---- check the traveler data ----

	if (step.sim_travel_ptr == 0) {
		if (step.Traveler () < 2) return;
		step.sim_travel_ptr = &sim->sim_travel_array [step.Traveler ()];
	}

	//---- check the vehicle data ----

	if (step.sim_veh_ptr == 0) {
#ifdef CHECK
		if (step.sim_travel_ptr->Vehicle () < 1) sim->Error ("Simulator_Service::Remove_Vehicle: Vehicle");
#endif
		step.sim_veh_ptr = &sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];
#ifdef CHECK
	} else {
		sim_veh_ptr = &sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];

		if (sim_veh_ptr != step.sim_veh_ptr) {
			sim->Error ("Simulator_Service::Remove_Vehicle: Vehicle Pointer");
		}
#endif
	}
	sim_veh_ptr = step.sim_veh_ptr;
#ifdef CHECK
	if (sim_veh_ptr == 0) sim->Error ("Simulator_Service::Remove_Vehicle: sim_veh_ptr");
#endif

	//---- get the vehicle type ----

	if (step.veh_type_ptr == 0) {
		if (step.sim_plan_ptr == 0) {
			step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
		}
		sim_plan_ptr = step.sim_plan_ptr;
#ifdef CHECK
		if (sim_plan_ptr == 0) sim->Error ("Simulator_Service::Remove_Vehicle: sim_plan_ptr");
		if (sim_plan_ptr->Veh_Type () < 0) sim->Error ("Simulator_Service::Remove_Vehicle: Veh_Type");
#endif
		step.veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];
	}
	veh_type_ptr = step.veh_type_ptr;
#ifdef CHECK
	sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
	if (veh_type_ptr == 0 || veh_type_ptr != &sim->veh_type_array [sim_plan_ptr->Veh_Type ()]) {
		sim->Error ("Simulator_Service::Remove_Vehicle: veh_type_ptr");
	}
#endif

	//---- remove the vehicle ----

	park_flag = sim_veh_ptr->Parked ();
	dir_index = -1;

	for (i=0; i < veh_type_ptr->Cells (); i++, sim_veh_ptr++) {
		if (park_flag) sim_veh_ptr->Parked (true);

		if (sim_veh_ptr->link < 0) continue;

		if (dir_index != sim_veh_ptr->link) {
			dir_index = sim_veh_ptr->link;
			sim_dir_ptr = &sim->sim_dir_array [dir_index];
		}
		lane = sim_veh_ptr->lane;
		if (lane < 0) continue;

		cell = sim->Offset_Cell (sim_veh_ptr->offset);

		index = sim_dir_ptr->Index (lane, cell);
		traveler = abs (sim_dir_ptr->Get (index));

		if (traveler == step.Traveler ()) {
			sim_dir_ptr->Remove (index);
#ifdef CHECK
		} else if (!park_flag && traveler > 0 && step.sim_travel_ptr->Status () == ON_NET_DRIVE) {
			sim->Error (String ("Simulator_Service::Remove_Vehicle: Traveler %d vs %d") % traveler % step.Traveler ());
#endif
		}
	}
}
