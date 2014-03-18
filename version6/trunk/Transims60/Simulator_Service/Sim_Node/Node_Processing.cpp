//*********************************************************
//	Node_Processing.cpp - simulate links entering a node
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Node_Processing
//---------------------------------------------------------

bool Sim_Node_Process::Node_Processing (int node)
{
	Dtime time;

	Int_Itr load_itr;
	Sim_Dir_Ptr sim_dir_ptr;
	Int_Itr cell_itr;
	Sim_Travel_Ptr sim_travel_ptr;
	Travel_Step step;
	Sim_Veh_Data sim_veh_rec, *sim_veh_ptr;
	Sim_Leg_Itr leg_itr, next_itr;

	for (int index = sim->node_link [node]; index >= 0; index = sim->link_list [index]) {

		step.Dir_Index (index);
		step.sim_dir_ptr = sim_dir_ptr = &sim->sim_dir_array [index];

		if (sim_dir_ptr->Count () <= 0) goto load_check;

		for (cell_itr = sim_dir_ptr->begin (); cell_itr != sim_dir_ptr->end (); cell_itr++) {
			if (*cell_itr <= 0) continue;

			step.Traveler (*cell_itr);
			step.sim_travel_ptr = sim_travel_ptr = &sim->sim_travel_array [*cell_itr];
		
			//---- check if the traveler has already been processed during this time step ----

			if (sim_travel_ptr->Step_Flag () == sim->Step_Flag ()) continue;

			sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];

			if (sim_veh_ptr->link != index) {
				step.Problem (PATH_PROBLEM);
				step.Status (3);
				Output_Step (step);
				continue;
			}
			step.assign (1, *sim_veh_ptr);

			if (sim_travel_ptr->sim_plan_ptr == 0) {
				//step.Problem (PLAN_PROBLEM);
				step.Problem (PATH_PROBLEM);
				step.Status (3);

				Output_Step (step);
				continue;
			}

			if (sim_travel_ptr->Next_Event () > sim->time_step) {
				sim_travel_ptr->Speed (0);

				Output_Step (step);

				num_vehicles++;
				continue;
			}

			//---- check the wait time constraint ----

			if (sim_travel_ptr->Wait () > sim->param.max_wait_time) {
				sim_veh_rec.Parked (true);

				step.Problem (WAIT_PROBLEM);
				step.Status (3);

				Output_Step (step);

				sim_travel_ptr->Next_Plan ();
				continue;
			}

			//---- get the current leg ----

			leg_itr = sim_travel_ptr->sim_plan_ptr->begin ();

			if (leg_itr == sim_travel_ptr->sim_plan_ptr->end ()) {
				sim_travel_ptr->Next_Plan ();
				continue;
			}
				
			//---- check the arrival time constraint ----

			if ((sim_travel_ptr->sim_plan_ptr->End () + sim->param.max_end_variance) < sim->time_step) {
				sim_veh_rec.Parked (true);

				step.Problem (ARRIVAL_PROBLEM);
				step.Status (3);

				Output_Step (step);

				sim_travel_ptr->Next_Plan ();
				continue;
			}

			//---- try to move the vehicle ----

			Move_Vehicle (step);

			num_vehicles++;	
			sim_travel_ptr->Step_Flag (sim->Step_Flag ());

			Output_Step (step);

		}


				//---- attempt a lane change ----

				//if (change_flag) {
				//	lane_change = step.sim_veh_ptr->Change ();

				//	if (Lane_Change (step)) {
				//		from_lane = lane;
				//		lane += lane_change;

				//		sim_lane_ptr = sim_dir_ptr->Lane (lane);

				//		change_flag = false;
				//		last_flag = true;
				//	}
				//}

				//---- check the intersection approach lane ----

				//if (cell >= out_cell) {

					//---- check the exit lanes ----

					//if (lane < leg_itr->Out_Lane_Low () || lane > leg_itr->Out_Lane_High ()) {
					//	change_flag = true;
					//	if (lane < leg_itr->Out_Lane_Low ()) {
					//		step.sim_veh_ptr->Change (1);
					//	} else {
					//		step.sim_veh_ptr->Change (-1);
					//	}
					//	if (speed > 1 && !last_flag) goto slow_down;
					//	step.Problem (LANE_PROBLEM);
					//	break;
					//}	

			//	}
			//}

		//---- try to move forward ----

		//cell++;

		//---- try to enter the next link ----

		//if (cell > max_cell) {
		//	if (connect_ptr == 0) break;

		//}

		////---- check the cell availability -----

		//traveler = sim_dir_ptr->Get (lane, cell);

		//if (traveler == -2) {

		//	//---- pocket lane ----

		//	if (cell > 0) {
		//		cell--;

		//		//---- attempt a lane change ----
		//	}
		//	//change_flag = true;
		//	//if (last_flag) {
		//	//	step.sim_veh_ptr->Change (lane - from_lane);
		//	//} else {
		//	//	if (lane < 1) {
		//	//		step.sim_veh_ptr->Change (1);
		//	//	} else {
		//	//		step.sim_veh_ptr->Change (-1);
		//	//	}
		//	//}
		//	if (speed > 1 && !last_flag) goto slow_down;
		//	step.Veh_Speed (0);
		//	step.Problem (LINK_ACCESS_PROBLEM);
		//	break;

		//} else if (traveler >= 0) {

		//	//---- check the vehicle spacing ----

		//	next_travel_ptr = &sim->sim_travel_array [traveler];

		//	sim_veh_ptr = &sim->sim_veh_array [next_travel_ptr->Vehicle ()];
		//	if (sim_veh_ptr == 0) break;

		//	veh_speed = sim_veh_ptr->Speed ();

		//	if (veh_speed <= speed && speed > 0 && !last_flag) {

		//	} else if (veh_speed == 0 && speed == 0 && !change_flag && exe->veh_status [vehicle] >= 0 &&
		//		step.size () == 0 && step.sim_veh_ptr->Leader () < 0) {
		//		
		//		cell_rec.Location (dir_index, lane, cell);
		//		Reserve_Cell (cell_rec, step);
		//	}
		//	if (speed < min_speed) step.Problem (SPACING_PROBLEM);
		//	break;
		//}


		////---- check the use restrictions ----

		//step.Delay (0);

		//if (cell >= first_cell && cell < last_cell && !Cell_Use (sim_lane_ptr, lane, cell, step)) {

		//	//---- check if lane change is possible ----

		//	if (cell > 0) {
		//		cell--;
		//	}

		//	if (speed > 1 && !last_flag) goto slow_down;
		//	step.Veh_Speed (0);
		//	step.Problem (USE_PROBLEM);
		//	break;
		//}

		////---- add the cell to the path ----

		//cell_rec.Location (dir_index, lane, cell);
		//step.push_back (cell_rec);
		//sim_dir_ptr->Cell (cell_rec, step.Vehicle ());

		////---- set the random delay 

		//if (step.Delay () > 0) {
		//	step.sim_veh_ptr->Next_Event (exe->step + step.Delay ());
		//	step.Veh_Speed (0);		//---- stop the vehicle ----
		//	break;
		//}
		//step.Veh_Speed (++speed);
		//last_flag = false;

		////---- check speed constraints ----
//
//		if ((cell + 1) == end_cell && speed > leg_itr->Max_Speed ()) {
//			step.Veh_Speed (leg_itr->Max_Speed () + 1);
//			break;
//		}
//	}
//
//	//---- remove the last cell from the current move ----
//
//slow_down:
//	cell_rec = step.back ();
//	exe->sim_dir_array.Cell (cell_rec, -1);
//
//	step.pop_back ();		
//	step.Veh_Speed (--speed);
//	return (step.size () > 0);


		//---- update the load list ----

load_check:
		if (sim_dir_ptr->load_queue.size () > 0) {
			Integers reload;

			for (load_itr = sim_dir_ptr->load_queue.begin (); load_itr != sim_dir_ptr->load_queue.end (); load_itr++) {
				if (!Load_Vehicle (*load_itr)) {
					sim_travel_ptr = &sim->sim_travel_array [*load_itr];
					sim_travel_ptr->Add_Wait (sim->param.step_size);

					if (sim_travel_ptr->Wait () > sim->param.max_load_time) {
						step.clear ();
						step.Traveler (*load_itr);
						step.sim_travel_ptr = sim_travel_ptr;

						step.Problem (LOAD_PROBLEM);
						step.Status (3);

						Output_Step (step);

						sim_travel_ptr->Next_Plan ();
					} else {
						reload.push_back (*load_itr);
						num_waiting++;			//---- number of vehicles still in the loading queue ----
					}
				} else {
					num_vehicles++;
				}
			}
			sim_dir_ptr->load_queue.swap (reload);
		}
	}
	return (node > 0);
}
