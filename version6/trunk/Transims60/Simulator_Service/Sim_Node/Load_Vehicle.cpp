//*********************************************************
//	Load_Vehicle.cpp - attempt to load a vehicle to the link
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Load_Vehicle
//---------------------------------------------------------

bool Sim_Node_Process::Load_Vehicle (int traveler)
{
	int lane, cell, subarea, dir_index, offset;
	bool loaded, type_flag, bound_flag, side_flag;
	Dtime time;
	
	Travel_Step step;

	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Ptr leg_ptr, next_leg;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Sim_Stop_Ptr sim_stop_ptr;
	Veh_Type_Data *veh_type_ptr;
	Sim_Veh_Data veh_cell, *sim_veh_ptr;

	subarea = 0;

	//---- build the travel step ----

	step.Process_ID (ID ());
	step.Traveler (traveler);
	sim_travel_ptr = &sim->sim_travel_array [traveler];

	sim_plan_ptr = sim_travel_ptr->Get_Plan ();
	if (sim_plan_ptr == 0) return (false);

	step.sim_travel_ptr = sim_travel_ptr;

	leg_ptr = sim_plan_ptr->Get_Leg ();
	if (leg_ptr == 0) return (false);

	type_flag = (sim_travel_ptr->Status () == OFF_ON_DRIVE);

	if (type_flag) {
		next_leg = leg_ptr;
	} else {
		next_leg = sim_plan_ptr->Get_Next (leg_ptr);
	}
	if (next_leg == 0) return (false);

	if (next_leg->Type () != DIR_ID) goto access_problem;

	dir_index = next_leg->Index ();
	step.Dir_Index (dir_index);
	step.sim_dir_ptr = sim_dir_ptr = &sim->sim_dir_array [dir_index];

	//---- locate the load point ----

	if (type_flag) {

		//---- external link load ----

		offset = 0;
		bound_flag = true;
		side_flag = (sim_travel_ptr->random.Probability () >= 0.5);

	} else {

		//---- transit loading ----

		if (sim_travel_ptr->Person () == 0) {
			if (leg_ptr->Type () != STOP_ID) goto access_problem;

			sim_stop_ptr = &sim->sim_stop_array [leg_ptr->Index ()];

			bound_flag = false;
			side_flag = true;
			offset = sim_stop_ptr->Offset ();

		} else {	//---- parking lot load ----

			if (leg_ptr->Type () != PARKING_ID) goto access_problem;

			sim_park_ptr = &sim->sim_park_array [leg_ptr->Index ()];

			bound_flag = (sim_park_ptr->Type () == BOUNDARY);

			if (bound_flag) {
				side_flag = (sim_travel_ptr->random.Probability () >= 0.5);
			} else {
				side_flag = (sim_park_ptr->Dir () == sim_dir_ptr->Dir ());
			}
			if (sim_dir_ptr->Dir () == 0) {
				offset = sim_park_ptr->Offset_AB ();
			} else {
				offset = sim_park_ptr->Offset_BA ();
			}
		}
	}

	//---- set vehicle type attributes ----
	
	step.veh_type_ptr = veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];

	//---- set the access point ----

	loaded = false;
	lane = 0;
	cell = sim->Offset_Cell (offset);

	//---- load from the right ----

	if (side_flag) {

		//---- try to load onto the best lanes ----

		for (lane = next_leg->In_Best_High (); lane >= next_leg->In_Best_Low (); lane--) {
			loaded = Cell_Use (sim_dir_ptr, lane, cell, step);
			if (loaded) break;
		}
		if (!loaded) {

			//---- try to load on the permitted lanes ----

			for (lane = next_leg->In_Lane_High (); lane > next_leg->In_Best_High (); lane--) {
				loaded = Cell_Use (sim_dir_ptr, lane, cell, step);
				if (loaded) break;
			}
			if (!loaded) {
				for (lane = next_leg->In_Best_Low ()-1; lane >= next_leg->In_Lane_Low (); lane--) {
					loaded = Cell_Use (sim_dir_ptr, lane, cell, step);
					if (loaded) break;
				}
			}
		}

	} else {	//---- load from the left ----

		//---- try to load onto the best lanes ----

		for (lane = next_leg->In_Best_Low (); lane <= next_leg->In_Best_High (); lane++) {
			loaded = Cell_Use (sim_dir_ptr, lane, cell, step);
			if (loaded) break;
		}
		if (!loaded) {

			//---- try to load on the permitted lanes ----

			for (lane = next_leg->In_Lane_Low (); lane < next_leg->In_Best_Low (); lane++) {
				loaded = Cell_Use (sim_dir_ptr, lane, cell, step);
				if (loaded) break;
			}
			if (!loaded) {
				for (lane = next_leg->In_Best_High ()+1; lane <= next_leg->In_Lane_High (); lane++) {
					loaded = Cell_Use (sim_dir_ptr, lane, cell, step);
					if (loaded) break;
				}
			}
		}
	}
	if (!loaded) {
		if (sim_travel_ptr->Wait () > sim->param.min_load_time && sim_dir_ptr->Load_Cell () < 0) {
			sim_dir_ptr->Load_Cell (cell);
		}
		return (false);
	}

	//---- put the vehicle on the network ----

	step.sim_veh_ptr = sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];
	
	sim_veh_ptr->Location (dir_index, -1, offset);

	step.push_back (*sim_veh_ptr);

	sim_veh_ptr->Location (dir_index, lane, offset);

	if (bound_flag) {
		sim_travel_ptr->Speed (veh_type_ptr->Max_Speed ());
	} else {
		sim_travel_ptr->Speed (veh_type_ptr->Max_Accel ());
	}
	if (sim_travel_ptr->Speed () > sim_dir_ptr->Speed ()) {
		sim_travel_ptr->Speed (sim_dir_ptr->Speed ());
	}
	sim_travel_ptr->Wait (0);
	sim_travel_ptr->Step_Code (sim->Step_Code ());

	sim_dir_ptr->Add (lane, cell, traveler);

	//---- record the load event ----

	if (!type_flag) {
		sim_plan_ptr->Next_Leg ();

		if (sim_travel_ptr->Person () == 0) {
			stats.num_run_start++;
		} else {
			stats.num_veh_start++;

			//---- output vehicle start event ----

			step.Event_Type (VEH_START_EVENT);
			sim->sim_output_step.Event_Check (step);
		}
	}
	sim_travel_ptr->Next_Event (time);
	step.sim_travel_ptr->Status (ON_NET_DRIVE);

	//---- try to move forward ---

	Move_Vehicle (step);

	return (true);

access_problem:
	step.Problem (ACCESS_PROBLEM);
	sim->Output_Step (step);
	return (false);
}
