//*********************************************************
//	Move_Vehicle.cpp - adjust the vehicle position and references
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Move_Vehicle
//---------------------------------------------------------

bool Sim_Node_Process::Move_Vehicle (Travel_Step &step)
{
	int in_offset, out_offset, length, in_cell, out_cell, max_cell, speed, min_speed, max_speed;
	int dir_index, move, offset, c, cell, l, lane, min_offset, max_offset, min_move, max_move, distance;
	bool new_link, lane_change;

	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Veh_Ptr sim_veh_ptr;
	Sim_Dir_Ptr sim_dir_ptr, ptr;
	Veh_Type_Data *veh_type_ptr;
	Sim_Leg_Itr leg_itr, next_itr;
	Sim_Park_Ptr sim_park_ptr;

	//---- initialize the pointers ----

	if (step.sim_travel_ptr == 0) {
		step.sim_travel_ptr = &sim->sim_travel_array [step.Traveler ()];
	}
	sim_travel_ptr = step.sim_travel_ptr;

	sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];

	if (step.Dir_Index () < 0) {
		step.Dir_Index (sim_veh_ptr->link);
	}
	dir_index = step.Dir_Index ();

	if (step.sim_dir_ptr == 0) {
		step.sim_dir_ptr = &sim->sim_dir_array [step.Dir_Index ()];
	}
	sim_dir_ptr = step.sim_dir_ptr;

	//---- initialize the link limits ----

	in_offset = sim_dir_ptr->In_Offset ();
	out_offset = sim_dir_ptr->Out_Offset ();
	length = sim_dir_ptr->Length ();

	in_cell = sim_dir_ptr->In_Cell ();
	out_cell = sim_dir_ptr->Out_Cell ();
	max_cell = sim_dir_ptr->Max_Cell ();

	//---- set the speed / movement range ----

	speed = sim_travel_ptr->Speed ();
			
	veh_type_ptr = &sim->veh_type_array [sim_travel_ptr->sim_plan_ptr->Veh_Type ()];

	min_speed = speed - veh_type_ptr->Max_Decel ();

	if (min_speed < 0) min_speed = 0;

	max_speed = speed + veh_type_ptr->Max_Accel ();

	if (max_speed > veh_type_ptr->Max_Speed ()) {
		max_speed = veh_type_ptr->Max_Speed ();
	}
	if (max_speed > sim_dir_ptr->Speed ()) {
		max_speed = sim_dir_ptr->Speed ();
	}

	//---- check for parking or stop events ----

	leg_itr = sim_travel_ptr->sim_plan_ptr->begin ();
	next_itr = leg_itr + 1;

	if (next_itr != sim_travel_ptr->sim_plan_ptr->end ()) {
		if (next_itr->Type () == PARKING_ID) {
			sim_park_ptr = &sim->sim_park_array [next_itr->Index ()];

			if (sim_park_ptr->Type () != BOUNDARY) {
				if (sim_dir_ptr->Dir () == 0) {
					length = sim_park_ptr->Offset_AB ();
				} else {
					length = sim_park_ptr->Offset_BA ();
				}
				max_cell = length / sim->param.cell_size;
			}
		} else if (next_itr->Type () == STOP_ID) {
		}
	}

	offset = sim_veh_ptr->offset;
	cell = offset / sim->param.cell_size;
	lane = sim_veh_ptr->lane;

	move = sim->Resolve (max_speed * sim->param.step_size);

	max_offset = offset + move;
	max_move = move / sim->param.cell_size;
	
	move = sim->Resolve (min_speed * sim->param.step_size);

	min_offset = offset + move;
	min_move = move / sim->param.cell_size;

	lane_change = false;
	distance = 0;

	for (move = 0; move < max_move; move++) {
		if (cell >= out_cell) {
		}
		c = cell++;
		offset += sim->param.cell_size;
		distance += sim->param.cell_size;
		l = lane;
		ptr = sim_dir_ptr;
		new_link = false;

		if (cell > max_cell) {
			if (next_itr->Type () == DIR_ID) {
				dir_index = next_itr->Index ();
				cell = 0;
				//offset = sim->param.half_cell;
				offset = sim->param.cell_size - 1;
				lane = next_itr->In_Best_Low ();
				sim_dir_ptr = &sim->sim_dir_array [dir_index];

				in_cell = sim_dir_ptr->In_Cell ();
				out_cell = sim_dir_ptr->Out_Cell ();
				max_cell = sim_dir_ptr->Max_Cell ();
				new_link = true;
			} else if (next_itr->Type () == PARKING_ID) {
				sim_travel_ptr->Status (-1);
				sim_veh_ptr->Location (dir_index, lane, offset);
				sim_veh_ptr->Parked (true);
				step.push_back (*sim_veh_ptr);

				ptr->Remove (l, c);
				sim_travel_ptr->sim_plan_ptr->Next_Leg ();
				sim_travel_ptr->Next_Event (sim->time_step);
				break;
			}
		}

		if (!Cell_Use (sim_dir_ptr, lane, cell, step)) {
			//if (new_link || lane_change) break;

			////---- consider a lane change ----

			//cell--;
			//offset -= sim->param.cell_size;
			//distance -= sim->param.cell_size;
			//
			//if (!Lane_Change (sim_dir_ptr, lane, cell, step)) break;
			//lane_change = true;
			break;
		}
		ptr->Set (l, c, 0);
		sim_dir_ptr->Set (lane, cell, step.Traveler ());

		sim_veh_ptr->Location (dir_index, lane, offset);

		step.push_back (*sim_veh_ptr);

		if (new_link) {
			ptr->Remove ();
			sim_dir_ptr->Add ();
			sim_travel_ptr->sim_plan_ptr->Next_Leg ();
		}
	}
	sim_travel_ptr->Speed (DTOI (distance / sim->UnRound (sim->param.step_size)));

	if (sim_travel_ptr->Speed () == 0 && speed == 0) {
		sim_travel_ptr->Add_Wait (sim->param.step_size);
	} else if (move) {
		sim_travel_ptr->Wait (0);
	}
	return (true);
}
