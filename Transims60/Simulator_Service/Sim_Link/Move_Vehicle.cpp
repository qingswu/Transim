//*********************************************************
//	Move_Vehicle.cpp - adjust the vehicle position and references
//*********************************************************

#include "Sim_Link_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Move_Vehicle
//---------------------------------------------------------

bool Sim_Link_Process::Move_Vehicle (Travel_Step &step)
{
	int in_offset, out_offset, length, in_cell, out_cell, max_cell, speed, min_speed, max_speed, new_index;
	int dir_index, move, offset, new_offset, move_size, cell, lane, max_move, distance, max_distance, l, l1, l2;
	bool new_link, lane_change, move_flag, exit_flag, next_leg_flag;
	Dtime step_size;

	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Veh_Data sim_veh;
	Sim_Veh_Ptr sim_veh_ptr;
	Sim_Dir_Ptr sim_dir_ptr, new_dir_ptr;
	Veh_Type_Data *veh_type_ptr;
	Sim_Leg_Ptr leg_ptr, next_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Sim_Stop_Ptr sim_stop_ptr;

	//---- initialize the pointers ----

	if (step.sim_travel_ptr == 0) {
		step.sim_travel_ptr = &sim->sim_travel_array [step.Traveler ()];
	}
	sim_travel_ptr = step.sim_travel_ptr;

	if (step.sim_veh_ptr == 0) {
		step.sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];
	}
	sim_veh_ptr = step.sim_veh_ptr;

	if (step.Dir_Index () < 0) {
		step.Dir_Index (sim_veh_ptr->link);
	}
	dir_index = step.Dir_Index ();
	if (dir_index != sim_veh_ptr->link) {
		sim->Warning (String ("Traveler %d Vehicle %d is on Link %d vs %d") % step.Traveler () % sim_travel_ptr->Vehicle () % sim_veh_ptr->link % dir_index);
		dir_index = sim_veh_ptr->link;
		step.Dir_Index (dir_index);
	}
	if (step.sim_dir_ptr == 0) {
		step.sim_dir_ptr = &sim->sim_dir_array [step.Dir_Index ()];
	}
	sim_dir_ptr = step.sim_dir_ptr;
	step_size = sim->method_time_step [sim_dir_ptr->Method ()];

	if (step.sim_plan_ptr == 0) {
		step.sim_plan_ptr = sim_travel_ptr->Get_Plan ();
	}
	sim_plan_ptr = step.sim_plan_ptr;

	if (step.veh_type_ptr == 0) {
		step.veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];
	}
	veh_type_ptr = step.veh_type_ptr;

//sim->debug = (sim_travel_ptr->Traveler () == 71);
//if (sim->debug) sim->Write (1, " Move step=") << sim->time_step << " link=" << dir_index;

	//---- initialize the link limits ----

	in_offset = sim_dir_ptr->In_Offset ();
	out_offset = sim_dir_ptr->Out_Offset ();
	length = sim_dir_ptr->Length ();

	in_cell = sim_dir_ptr->In_Cell ();
	out_cell = sim_dir_ptr->Out_Cell ();
	max_cell = sim_dir_ptr->Max_Cell ();

	//---- set the speed / movement range ----

	speed = sim_travel_ptr->Speed ();

	min_speed = speed - ((veh_type_ptr->Max_Decel () * step_size + sim->half_second) / sim->one_second);

	if (min_speed < 0) min_speed = 0;

	max_speed = speed + ((veh_type_ptr->Max_Accel () * step_size + sim->half_second) / sim->one_second);

	if (max_speed > veh_type_ptr->Max_Speed ()) {
		max_speed = veh_type_ptr->Max_Speed ();
	}
	if (max_speed > sim_dir_ptr->Speed ()) {
		max_speed = sim_dir_ptr->Speed ();
	}

	//---- check for parking or stop events ----

	leg_ptr = sim_plan_ptr->Get_Leg ();
	if (leg_ptr->Type () == STOP_ID) {
		sim->Warning ("STOP_ID");
		return (false);
	}
	next_ptr = sim_plan_ptr->Get_Next (leg_ptr);
//if (sim->debug) sim->Write (0, " leg=") << sim_plan_ptr->First_Leg () << " next=" << leg_ptr->Next_Record ();
	if (next_ptr != 0) {
		if (next_ptr->Type () == PARKING_ID) {
			sim_park_ptr = &sim->sim_park_array [next_ptr->Index ()];

			if (sim_park_ptr->Type () != BOUNDARY) {
				if (sim_dir_ptr->Dir () == 0) {
					length = sim_park_ptr->Offset_AB ();
				} else {
					length = sim_park_ptr->Offset_BA ();
				}
				max_cell = sim->Offset_Cell (length);
			}
		} else if (next_ptr->Type () == STOP_ID) {
			sim_stop_ptr = &sim->sim_stop_array [next_ptr->Index ()];
			length = sim_stop_ptr->Offset ();
			max_cell = sim->Offset_Cell (length);
		}
	}

	//---- remove the vehicle from the current location ----

	sim_veh = *sim_veh_ptr;
	step.push_back (sim_veh);

	offset = sim_veh.offset;
	cell = sim->Offset_Cell (offset);
	lane = sim_veh.lane;

	l1 = leg_ptr->Out_Best_Low ();
	l2 = leg_ptr->Out_Best_High ();

	sim_dir_ptr->Remove (lane, cell);
//if (sim->debug) sim->Write (0, " remove=") << dir_index << "-" << lane << "-" << cell;

	//---- calculate the move limits ----

	max_distance = sim->Resolve (max_speed * step_size);
	max_move = max_distance / sim->param.cell_size;

	exit_flag = lane_change = false;
	distance = 0;

	//---- process each movement increment ----

	for (move = 0; move <= max_move; move++) {
		new_link = next_leg_flag = false;
			
		//---- check the intersection approach lane ----

		if (cell >= out_cell && sim_dir_ptr->Method () > MACROSCOPIC) {

			//---- check the exit lanes ----

			if (lane < leg_ptr->Out_Lane_Low ()) {
				if (Cell_Use (sim_dir_ptr, lane + 1, cell, step)) {
					lane++;
					goto make_move;
				}
				break;
			} else if (lane > leg_ptr->Out_Lane_High ()) {
				if (Cell_Use (sim_dir_ptr, lane - 1, cell, step)) {
					lane--;
					goto make_move;
				}
				break;
			}
		}

		//---- try to move forward ----

		move_size = sim->param.cell_size;

		if (distance + move_size > max_distance) {
			move_size = max_distance - distance;
		}
		new_offset = offset + move_size;

		cell = sim->Offset_Cell (new_offset);

		new_index = dir_index;		
		new_dir_ptr = sim_dir_ptr;

		l1 = l2 = lane;

		//---- end of leg ----

		if (cell > max_cell) {
			if (next_ptr->Type () == PARKING_ID) {
				sim_veh.Parked (true);
				step.push_back (sim_veh);

				sim_travel_ptr->Status (ON_OFF_PARK);
				sim_travel_ptr->Next_Event (sim->time_step);
				//stats.num_veh_end++;
				break;
			} else if (next_ptr->Type () == STOP_ID) {
//if (sim->debug) sim->Write (0, " STOP");
				//---- add dwell time ----

				if (!sim_plan_ptr->Next_Leg ()) {
//if (sim->debug) sim->Write (0, " PARK");
					step.sim_veh_ptr->Parked (true);
					sim_travel_ptr->Status (OFF_NET_END);
					sim_travel_ptr->Next_Event (sim->param.end_time_step);
				} else {
					sim_travel_ptr->Next_Event (sim->time_step + Dtime (5, SECONDS));
				}
				break;
			} else if (next_ptr->Type () == DIR_ID) {
				step.Speed (0);
				if (!step.Exit_Flag ()) break;

				new_offset -= sim_dir_ptr->Length ();

				new_index = next_ptr->Index ();
				new_dir_ptr = &sim->sim_dir_array [new_index];

				if (new_dir_ptr->Method () == NO_SIMULATION) {
					sim_plan_ptr->Next_Leg ();
					sim_travel_ptr->Next_Event (sim->time_step);
					sim_travel_ptr->Status (ON_OFF_DRIVE);
					sim_travel_ptr->Wait (0);
					exit_flag = true;
					goto output;
				}


//int lock=new_dir_ptr->Lock ();
//sim->Write (1, String ("new lock=%d, lock=%d, id=%d ") % new_index % lock % id);
				sim->sim_dir_array.Lock (new_dir_ptr, ID ());
//sim->Write (1, String ("link=%d locked=%d ") % new_index % new_dir_ptr->Lock ());
				in_cell = new_dir_ptr->In_Cell ();
				out_cell = new_dir_ptr->Out_Cell ();
				max_cell = new_dir_ptr->Max_Cell ();

				cell = sim->Offset_Cell (new_offset);
				lane = l1 = next_ptr->In_Best_Low ();
				l2 = next_ptr->In_Best_High ();
				new_link = true;
			}
		}

		//---- check the new cell options ----

		move_flag = false;

		for (l = l1; l <= l2; l++) {
			if (Cell_Use (new_dir_ptr, l, cell, step)) {
				lane = l;
				move_flag = true;
				break;
			}
		}
		if (!move_flag) {
			if (!new_link) {

				//---- consider changing lanes ----

				if (leg_ptr->Out_Best_Low () < lane) {
					if (Cell_Use (new_dir_ptr, lane - 1, cell, step)) {
						lane = lane - 1;
						move_flag = true;
					}
				}
				if (!move_flag) {
					if (leg_ptr->Out_Best_High () > lane) {
						if (Cell_Use (new_dir_ptr, lane + 1, cell, step)) {
							lane = lane + 1;
							move_flag = true;
						}
					}
				}
			}
			if (!move_flag) {
//sim->Write (1, String ("unlock=%d lock=%d id=%d") % new_index % new_dir_ptr->Lock () % id);
				sim->sim_dir_array.UnLock (new_dir_ptr, ID ());
//sim->Write (1, String ("unlock=%d") % new_index);
				break;
			}
		}
		dir_index = new_index;
		sim_dir_ptr = new_dir_ptr;

		distance += move_size;
		offset = new_offset;
		next_leg_flag = new_link;
		if (new_link) {
//sim->Write (1, String ("unlock=%d lock=%d id=%d") % new_index % new_dir_ptr->Lock () % id);
			sim->sim_dir_array.UnLock (new_dir_ptr, ID ());
//sim->Write (1, String ("unlock=%d") % new_index);
			exit_flag = true;
		}
make_move:
		sim_veh.Location (dir_index, lane, offset);
		step.push_back (sim_veh);
//if (sim->debug) sim->Write (0, " to=") << offset << " flag=" << next_leg_flag;
		//---- move to the next leg ----

		if (next_leg_flag) {

			if (!sim_plan_ptr->Next_Leg ()) break;
//if (sim->debug) sim->Write (0, " NEXT_LEG");
			leg_ptr = sim_plan_ptr->Get_Leg ();
			next_ptr = sim_plan_ptr->Get_Next (leg_ptr);

			if (next_ptr != 0) {
				if (next_ptr->Type () == PARKING_ID) {
					sim_park_ptr = &sim->sim_park_array [next_ptr->Index ()];

					if (sim_park_ptr->Type () != BOUNDARY) {
						if (sim_dir_ptr->Dir () == 0) {
							length = sim_park_ptr->Offset_AB ();
						} else {
							length = sim_park_ptr->Offset_BA ();
						}
						max_cell = sim->Offset_Cell (length);
					}
				} else if (next_ptr->Type () == STOP_ID) {
					sim_stop_ptr = &sim->sim_stop_array [next_ptr->Index ()];

					length = sim_stop_ptr->Offset ();
					max_cell = sim->Offset_Cell (length);
				}
			}
		}
	}

	//---- place the vehicle at the new location ----

	*sim_veh_ptr = sim_veh;

	if (!sim_veh.Parked ()) {
		lane = sim_veh.lane;
		cell = sim->Offset_Cell (sim_veh.offset);
//if (sim->debug) sim->Write (0, " put=") << dir_index << "-" << lane << "-" << cell;
		sim_dir_ptr->Add (lane, cell, step.Traveler ());
//if (sim->debug) sim->Write (0, " result=") << sim_dir_ptr->Get (lane, cell);
	}
	sim_travel_ptr->Speed (DTOI (distance / sim->UnRound (step_size)));
//if (sim->debug) sim->Write (0, " dist=") << distance << " speed=" << sim_travel_ptr->Speed ();
output:
	sim->Output_Step (step);
//if (sim->debug) sim->Write (0, " speed=") << sim_travel_ptr->Speed () << " vs " << speed << " move=" << move;
	if (sim_travel_ptr->Speed () == 0 && speed == 0) {
		sim_travel_ptr->Add_Wait (step_size);
	} else if (move) {
		sim_travel_ptr->Wait (0);
	}
	return (exit_flag);
}


