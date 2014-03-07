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
	int lane, subarea, dir_index, cell, offset;
	bool loaded, type_flag, bound_flag, side_flag;
	Dtime time;
	
	Travel_Step step;

	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Itr leg_itr, next_leg, check_leg;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Sim_Stop_Ptr sim_stop_ptr;
	Veh_Type_Data *veh_type_ptr;
	Sim_Veh_Data veh_cell, *sim_veh_ptr;

	subarea = 0;

	//---- build the travel step ----

	step.Traveler (traveler);
	sim_travel_ptr = &sim->sim_travel_array [traveler];
	sim_plan_ptr = sim_travel_ptr->sim_plan_ptr;

	if (sim_plan_ptr == 0) return (false);

	step.sim_travel_ptr = sim_travel_ptr;

	leg_itr = sim_plan_ptr->begin ();
	if (leg_itr == sim_plan_ptr->end ()) return (false);

	next_leg = leg_itr + 1;
	if (next_leg == sim_plan_ptr->end ()) return (false);

	if (next_leg->Type () != DIR_ID) goto access_problem;

	dir_index = next_leg->Index ();

	step.sim_dir_ptr = sim_dir_ptr = &sim->sim_dir_array [dir_index];
	step.Dir_Index (dir_index);

	//---- locate the load point ----

	type_flag = (sim_travel_ptr->Status () != 2);

	if (type_flag) {

		//---- external link load ----

		offset = 0;
		bound_flag = true;
		side_flag = (sim_travel_ptr->random.Probability () >= 0.5);

	} else {

		//---- transit loading ----

		if (sim_travel_ptr->Person () == 0) {
			if (leg_itr->Type () != STOP_ID) goto access_problem;

			sim_stop_ptr = &sim->sim_stop_array [leg_itr->Index ()];

			bound_flag = false;
			side_flag = true;
			offset = sim_stop_ptr->Offset ();

		} else {	//---- parking lot load ----

			if (leg_itr->Type () != PARKING_ID) goto access_problem;

			sim_park_ptr = &sim->sim_park_array [leg_itr->Index ()];

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
	cell = offset / sim->param.cell_size;

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
	if (!loaded) return (false);

	//---- put the vehicle on the network ----

	sim_plan_ptr->Next_Leg ();
	
	veh_cell.Parked (true);
	step.push_back (veh_cell);

	step.sim_veh_ptr = sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];

	sim_veh_ptr->Location (dir_index, lane, offset);

	step.push_back (*sim_veh_ptr);

	if (bound_flag) {
		sim_travel_ptr->Speed (veh_type_ptr->Max_Speed ());
	} else {
		sim_travel_ptr->Speed (veh_type_ptr->Max_Accel ());
	}
	if (sim_travel_ptr->Speed () > sim_dir_ptr->Speed ()) {
		sim_travel_ptr->Speed (sim_dir_ptr->Speed ());
	}
	sim_travel_ptr->Wait (0);
	sim_travel_ptr->Step_Flag (sim->Step_Flag ());

	sim_dir_ptr->Add (lane, cell, traveler);

	//---- try to move forward ---

	Move_Vehicle (step);

	//---- record the load event ----

	if (step.sim_travel_ptr->Status () != -4) {
		if (sim_travel_ptr->Person () == 0) {
			stats.num_run_start++;
		} else {
			stats.num_veh_start++;

			//---- output event record ----

			//if (sim->event_output.In_Range (VEH_START_EVENT, sim_plan_ptr->Mode (), subarea)) {
			//	Event_Data event_data;

			//	event_data.Household (sim_travel_ptr->Household ());
			//	event_data.Person (sim_travel_ptr->Person ());
			//	event_data.Tour (sim_plan_ptr->Tour ());
			//	event_data.Trip (sim_plan_ptr->Trip ());
			//	event_data.Mode (sim_plan_ptr->Mode ());
			//	event_data.Schedule (sim_travel_ptr->Next_Event ().Round_Seconds ());
			//	event_data.Actual (sim->time_step);
			//	event_data.Event (VEH_START_EVENT);
			//	event_data.Dir_Index (dir_index);
			//	event_data.Offset (offset);
			//	event_data.Lane (lane);
			//	event_data.Route (-1);

			//	sim->event_output.Output_Event (event_data);
			//}
		}
	}

	sim_travel_ptr->Next_Event (time);
	step.sim_travel_ptr->Status (1);
	return (true);

access_problem:
	step.Problem (ACCESS_PROBLEM);
	step.Status (2);

	Output_Step (step);
	return (false);
}
