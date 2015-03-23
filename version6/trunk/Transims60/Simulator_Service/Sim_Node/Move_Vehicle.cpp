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
	int i, in_offset, out_offset, in_cell, out_cell, max_cell, speed, min_speed, max_speed, spd1, spd2, spd0;
	int dir_index, new_index, index, move, offset, new_offset, move_size, cell, lane, max_move, lanes;
	int distance, min_distance, max_distance, l, low, high, control, from_dir, num_cells, c, num_sec;
	bool new_link, keep_flag, move_flag, exit_flag, next_leg_flag, slow_down, change_flag;
	double speed_change;
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
	Connect_Data *connect_ptr, *conflict_ptr;
	Sim_Signal_Data *control_ptr;
	Sim_Connection *sim_con_ptr = 0;

	//---- initialize the pointers ----

	if (step.sim_travel_ptr == 0) {
		step.sim_travel_ptr = &sim->sim_travel_array [step.Traveler ()];
	}
	sim_travel_ptr = step.sim_travel_ptr;
#ifdef CHECK
	if (sim_travel_ptr == 0) sim->Error ("Sim_Node_Process::Move_Vehicle: sim_travel_ptr");
	if (sim_travel_ptr->Vehicle () < 1) sim->Error (String ("Sim_Node_Process::Move_Vehicle: Vehicle=%d, Traveler=%d") % sim_travel_ptr->Vehicle () % sim_travel_ptr->Traveler ());
#endif

	if (step.sim_veh_ptr == 0) {
		step.sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];
	}
	sim_veh_ptr = step.sim_veh_ptr;
#ifdef CHECK
	if (sim_veh_ptr == 0) sim->Error ("Sim_Node_Process::Move_Vehicle: sim_veh_ptr");
#endif

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

#ifdef CHECK
	if (sim_dir_ptr == 0) sim->Error ("Sim_Node_Process::Move_Vehicle: sim_dir_ptr");
#endif
	step_size = sim->method_time_step [sim_dir_ptr->Method ()];

	if (step.sim_plan_ptr == 0) {
		step.sim_plan_ptr = sim_travel_ptr->Get_Plan ();
	}
	sim_plan_ptr = step.sim_plan_ptr;
#ifdef CHECK
	if (sim_plan_ptr == 0) sim->Error ("Sim_Node_Process::Move_Vehicle: sim_plan_ptr");
	if (sim_plan_ptr->Veh_Type () < 0) sim->Error ("Sim_Node_Process::Move_Vehicle: Veh_Type");
#endif

	if (step.veh_type_ptr == 0) {
		step.veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];
	}
	veh_type_ptr = step.veh_type_ptr;
#ifdef CHECK
	if (veh_type_ptr == 0 || veh_type_ptr != &sim->veh_type_array [sim_plan_ptr->Veh_Type ()]) {
		sim->Error ("Sim_Node_Process::Move_Vehicle: veh_type_ptr");
	}
#endif

	//---- initialize the link limits ----

	in_offset = sim_dir_ptr->In_Offset ();
	out_offset = sim_dir_ptr->Out_Offset ();

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

	step.sim_leg_ptr = leg_ptr = sim_plan_ptr->Get_Leg ();

#ifdef CHECK
	if (leg_ptr->Type () == STOP_ID) sim->Error ("Sim_Node_Process::Move_Vehicle: Leg Type");
#endif

	next_ptr = sim_plan_ptr->Get_Next (leg_ptr);

	if (next_ptr != 0) {
		if (next_ptr->Type () == PARKING_ID) {
			sim_park_ptr = &sim->sim_park_array [next_ptr->Index ()];

			if (sim_park_ptr->Type () != BOUNDARY) {
				if (sim_dir_ptr->Dir () == 0) {
					max_cell = sim->Offset_Cell (sim_park_ptr->Offset_AB ());
				} else {
					max_cell = sim->Offset_Cell (sim_park_ptr->Offset_BA ());
				}
			}
		} else if (next_ptr->Type () == STOP_ID) {
			sim_stop_ptr = &sim->sim_stop_array [next_ptr->Index ()];
			max_cell = sim->Offset_Cell (sim_stop_ptr->Offset ());
		}
	}
	
	//---- remove the vehicle from the current location ----

	sim_veh = *sim_veh_ptr;

	offset = sim_veh.offset;
	cell = sim->Offset_Cell (offset);
	lane = sim_veh.lane;

	step.push_back (sim_veh);

	sim_dir_ptr->Remove (lane, cell);

	connect_ptr = 0;
	control = UNCONTROLLED;
	change_flag = keep_flag = false;

	//---- meso / micro setup ----

	if (sim_dir_ptr->Method () > MACROSCOPIC) {

		//---- look ahead lane change ----

		if (Look_Ahead (sim_dir_ptr, lane, cell, step)) {
			keep_flag = true;
			sim_veh.lane = lane;
			step.push_back (sim_veh);
		}

		//---- get the connection control ----

		if (leg_ptr->Connect () >= 0) {
			connect_ptr = &sim->connect_array [leg_ptr->Connect ()];
			control = connect_ptr->Control ();
		}

		//---- set plan following / lane change flags ----

		num_cells = max_cell - cell;

		low = leg_ptr->Out_Best_Low ();
		high = leg_ptr->Out_Best_High ();

		if (num_cells < sim->param.plan_follow && (lane < low || lane > high)) {
			change_flag = true;

			if (lane < low) {
				l = 1;
				lanes = low - lane;
				if (lanes > 1 && leg_ptr->Out_Lane_Low () < low) lanes--;
			} else {
				l = -1;
				lanes = lane - high;
				if (lanes > 1 && leg_ptr->Out_Lane_High () > high) lanes--;
			}
			lanes = (sim->param.lane_change_levels * num_cells / lanes + sim->param.plan_follow / 2) / sim->param.plan_follow; 

			if (lanes <= sim->param.change_priority && max_speed > 0 && max_speed > min_speed) {
				if (lanes > 0) {
					if (Check_Cell (sim_dir_ptr, lane + l, cell, step)) {
						spd0 = sim_dir_ptr->Speed () * lanes / sim->param.lane_change_levels + 1;
						if (max_speed > spd0) {
							max_speed -= veh_type_ptr->Max_Decel ();
						}
					}
				} else {
					max_speed -= veh_type_ptr->Max_Decel ();
				}
				if (max_speed < min_speed) max_speed = min_speed;

				lanes = sim->param.change_priority - lanes + 1;
				if (lanes > sim_travel_ptr->Priority ()) sim_travel_ptr->Priority (lanes);
			}

			//---- consider a lane change ----

			if (change_flag && sim_travel_ptr->Priority () > 0) {
				if (Check_Cell (sim_dir_ptr, lane + l, cell, step)) {
					sim_veh.lane = lane = lane + l;
					step.push_back (sim_veh);
					change_flag = false;
				}
			}
		}

		//---- apply random slow down ----

		if (sim->param.slow_down_flag && max_speed > min_speed) {
			speed_change = sim->param.slow_down_prob [sim_dir_ptr->Type ()];
			if (sim->param.traveler_flag) speed_change *= sim->param.traveler_fac.Best (step.sim_travel_ptr->Type ());

			if (speed_change > step.sim_travel_ptr->random.Probability ()) {
				speed_change = sim->param.slow_down_percent [sim_dir_ptr->Type ()];
				if (sim->param.traveler_flag) speed_change *= sim->param.traveler_fac.Best (step.sim_travel_ptr->Type ());

				if (speed_change > 0.0) {
					max_speed = DTOI (max_speed * (1.0 - speed_change));
				} else {
					max_speed -= veh_type_ptr->Max_Decel ();
				}
				if (max_speed < min_speed) max_speed = min_speed;
			}
		}

		//---- maximum comfortable speed ----

		if (sim->param.comfort_flag && max_speed > min_speed) {
			spd0 = sim->param.comfort_speed.Best (step.sim_travel_ptr->Type ());
			if (spd0 > 0 && max_speed > spd0) {
				max_speed = spd0;
				if (max_speed < min_speed) max_speed = min_speed;
			}
		}
	}

	//---- calculate the move limits ----

	num_sec = (step_size + sim->half_second) / sim->one_second;

	if (num_sec > 0) {
		spd1 = spd2 = speed;
		min_distance = max_distance = 1;

		for (i=0; i < num_sec; i++) {
			spd0 = spd1;
			spd1 -= veh_type_ptr->Max_Decel ();
			if (spd1 < min_speed) spd1 = min_speed;

			min_distance += (spd0 + spd1) / 2;

			spd0 = spd2;
			spd2 += veh_type_ptr->Max_Accel ();
			if (spd2 > max_speed) spd2 = max_speed;

			max_distance += (spd0 + spd2) / 2;
		}
	} else {
		min_distance = sim->Resolve (min_speed * step_size) + 1;
		max_distance = sim->Resolve (max_speed * step_size) + 1;
	}
	max_move = max_distance / sim->param.cell_size;

	exit_flag = slow_down = false;
	distance = 0;
	move_size = sim->param.cell_size;

	//---- process each movement increment ----

	for (move = 0; move <= max_move; move++) {
		new_link = next_leg_flag = slow_down = false;

		//---- meso / micro logic ----

		if (sim_dir_ptr->Method () > MACROSCOPIC) {

			//---- check the intersection approach lane ----

			if (cell >= out_cell) {

				//---- check the exit lanes ----

				if (lane < leg_ptr->Out_Best_Low () || lane > leg_ptr->Out_Best_High ()) { 
					if (Lane_Change (sim_dir_ptr, lane, cell, step)) {
						sim_veh.Location (dir_index, lane, offset);
						step.push_back (sim_veh);
						keep_flag = true;
					}
					if (lane < leg_ptr->Out_Lane_Low () || lane > leg_ptr->Out_Lane_High ()) continue;
				} 

				//---- check the traffic control ----

				if (control != UNCONTROLLED && cell == out_cell) {

					//---- check stop controls ----

					if (control == RED_LIGHT || (speed > 0 && (control == STOP_SIGN || control == STOP_GREEN))) {
						if (move > 1 && move == max_move && !keep_flag) {
							slow_down = true;
						} else {
							if (control == RED_LIGHT) {
								control_ptr = &sim->sim_signal_array [sim_dir_ptr->Control ()];
								sim_travel_ptr->Next_Event (control_ptr->Check_Time ()); 
							}
							step.Speed (0);		//---- stop the vehicle ----
						}
						break;
					} 

					//---- yellow decision ----

					if (control == YELLOW_LIGHT && distance > min_distance) {	
						if (move > 1 && move == max_move && !keep_flag) {
							slow_down = true;
						} else {
							control_ptr = &sim->sim_signal_array [sim_dir_ptr->Control ()];
							sim_travel_ptr->Next_Event (control_ptr->Check_Time ()); 
							step.Speed (0);		//---- stop the vehicle ----
						}
						break;
					}

					//---- check conflicts ----

					if (control != PROTECTED_GREEN && connect_ptr > 0) {
#ifdef CHECK
						if (leg_ptr->Connect () < 0 || leg_ptr->Connect () >= (int) sim->sim_connection.size ()) 
							sim->Error ("Sim_Node_Process::Move_Vehicle: leg connect");
#endif				
						sim_con_ptr = &sim->sim_connection [leg_ptr->Connect ()];

						for (i=0; i < sim_con_ptr->Max_Conflicts (); i++) {
							index = sim_con_ptr->Conflict (i);
							if (index < 0) continue;
#ifdef CHECK
							if (index >= (int) sim->connect_array.size ()) 
								sim->Error ("Sim_Node_Process::Move_Vehicle: connect index");
#endif	
							conflict_ptr = &sim->connect_array [index];

							if (i == 0) {
								from_dir = conflict_ptr->Dir_Index ();
								low = conflict_ptr->Low_Lane ();
								high = conflict_ptr->High_Lane ();
#ifdef CHECK
								if (from_dir < 0 || from_dir >= (int) sim->sim_dir_array.size ()) 
									sim->Error ("Sim_Node_Process::Move_Vehicle: from_dir");
#endif	
								c = sim->sim_dir_array [from_dir].Max_Cell ();
							} else {
								from_dir = conflict_ptr->To_Index ();
								low = conflict_ptr->To_Low_Lane ();
								high = conflict_ptr->To_High_Lane ();
								c = 0;
							}
							num_cells = max_cell - cell + (high - low + 1) / 2;
#ifdef CHECK
							if (num_cells < 0) 
								sim->Error (String ("Sim_Node_Process::Move_Vehicle: num_cells=%d") % num_cells);
							if (from_dir < 0 || from_dir >= (int) sim->sim_dir_array.size ()) 
								sim->Error ("Sim_Node_Process::Move_Vehicle: from_dir");

							if (low < 0 || high >= sim->sim_dir_array [from_dir].Lanes ()) 
								sim->Error ("Sim_Node_Process::Move_Vehicle: lanes");
#endif
							for (l=low; l <= high; l++) {
								if (!Check_Behind (from_dir, l, c, step, num_cells)) {
									if (move > 1 && !keep_flag) {
										slow_down = true;
									} else {
										step.Speed (0);
									}
									break;
								}
							}
							if (l <= high) break;
						}
					}
				}

			} else {

				//---- try changing lanes ----

				if (change_flag) {
					if (Lane_Change (sim_dir_ptr, lane, cell, step)) {
						keep_flag = true;
						change_flag = (lane < leg_ptr->Out_Best_Low () || lane > leg_ptr->Out_Best_High ());
						goto make_move;
					}
				}
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

		low = high = lane;

		//---- end of leg ----

		if (cell > max_cell || new_offset > sim_dir_ptr->Length ()) {

			if (next_ptr->Type () == PARKING_ID) {

				//---- park the vehicle ----

				sim_veh.Parked (true);
				step.push_back (sim_veh);
				sim_travel_ptr->Status (ON_OFF_PARK);
				sim_travel_ptr->Next_Event (sim->time_step);
				break;
			} else if (next_ptr->Type () == STOP_ID) {

				//---- add dwell time ----

				if (!sim_plan_ptr->Next_Leg ()) {
					step.sim_veh_ptr->Parked (true);
					sim_travel_ptr->Status (OFF_NET_END);
					sim_travel_ptr->Next_Event (sim->param.end_time_step);
				} else {
					sim_travel_ptr->Next_Event (sim->time_step + Dtime (5, SECONDS));
				}
				break;
			} else if (next_ptr->Type () == DIR_ID) {

				//---- check macro exit count ----

				step.Speed (0);
				if (!step.Exit_Flag ()) break;

				//---- set link entry attributes ----

				new_offset -= sim_dir_ptr->Length ();

				new_index = next_ptr->Index ();
				new_dir_ptr = &sim->sim_dir_array [new_index];

				if (new_dir_ptr->Method () == NO_SIMULATION) {

					//---- leave the simulation network ----

					sim_plan_ptr->Next_Leg ();
					sim_travel_ptr->Next_Event (sim->time_step);
					sim_travel_ptr->Status (ON_OFF_DRIVE);
					sim_travel_ptr->Wait (0);
					sim_travel_ptr->Priority (0);
					exit_flag = true;
					sim->Remove_Vehicle (step);
					goto output;
				}
				sim->sim_dir_array.Lock (new_dir_ptr, ID ());
//sim->Warning (String ("Sim_Node_Process::Mode_Vehicle: Lock %d") % new_dir_ptr->Lock ());
				in_cell = new_dir_ptr->In_Cell ();
				out_cell = new_dir_ptr->Out_Cell ();
				max_cell = new_dir_ptr->Max_Cell ();

				cell = sim->Offset_Cell (new_offset);
				lane = low = next_ptr->In_Best_Low ();
				high = next_ptr->In_Best_High ();
				new_link = true;
			}

		} else {

			//---- check for a pocket lane ----

			if (sim_dir_ptr->Get (lane, cell) == -1) {
				if (lane > 0) {
					l = lane - 1;

					if (Check_Cell (sim_dir_ptr, l, cell, step)) {
						lane = l;
						goto make_move;
					}
				}
				l = lane + 1;
				if (l < sim_dir_ptr->Lanes ()) {
					if (Check_Cell (sim_dir_ptr, l, cell, step)) {
						lane = l;
						goto make_move;
					}
				}
				if (cell > 1) {
					cell--;
					if (lane > 0) {
						l = lane - 1;

						if (Check_Cell (sim_dir_ptr, l, cell, step)) {
							lane = l;
							goto make_move;
						}
					}
					l = lane + 1;
					if (l < sim_dir_ptr->Lanes ()) {
						if (Check_Cell (sim_dir_ptr, l, cell, step)) {
							lane = l;
							goto make_move;
						}
					}
				}
				break;
			}
		}

		//---- check the new cell options ----

		move_flag = false;

		for (l = low; l <= high; l++) {
			if (Check_Cell (new_dir_ptr, l, cell, step)) {
				lane = l;
				move_flag = true;
				break;
			}
		}
		if (!move_flag) {
			if (!new_link) {

				//---- consider changing lanes ----

				move_flag = Lane_Change (new_dir_ptr, lane, cell, step);
				if (move_flag) keep_flag = true;
			}
			if (!move_flag) {
				if (new_link) {
#ifdef CHECK
					if (ID () > 0 && new_dir_ptr->Lock () != ID ()) sim->Error (String ("Move_Vehicle::Unlock (% vs %d)") % new_dir_ptr->Lock () % ID ());
#endif
					sim->sim_dir_array.UnLock (new_dir_ptr, ID ());
				}
				break;
			}
		} else if (!new_link) {
			keep_flag = false;
		}

		//---- move forward ----

		dir_index = new_index;
		sim_dir_ptr = new_dir_ptr;

		distance += move_size;
		offset = new_offset;
		next_leg_flag = new_link;
		if (new_link) exit_flag = true;

make_move:
		sim_veh.Location (dir_index, lane, offset);

		step.push_back (sim_veh);

		//---- move to the next leg ----

		if (next_leg_flag) {
			keep_flag = true;

			if (!sim_plan_ptr->Next_Leg ()) break;

			step.sim_leg_ptr = leg_ptr = sim_plan_ptr->Get_Leg ();
			next_ptr = sim_plan_ptr->Get_Next (leg_ptr);

			if (next_ptr != 0) {
				if (next_ptr->Type () == PARKING_ID) {
					sim_park_ptr = &sim->sim_park_array [next_ptr->Index ()];

					if (sim_park_ptr->Type () != BOUNDARY) {
						if (sim_dir_ptr->Dir () == 0) {
							max_cell = sim->Offset_Cell (sim_park_ptr->Offset_AB ());
						} else {
							max_cell = sim->Offset_Cell (sim_park_ptr->Offset_BA ());
						}
					}
				} else if (next_ptr->Type () == STOP_ID) {
					sim_stop_ptr = &sim->sim_stop_array [next_ptr->Index ()];
					max_cell = sim->Offset_Cell (sim_stop_ptr->Offset ());
				}
			}

			//---- meso / micro setup ----

			if (sim_dir_ptr->Method () > MACROSCOPIC) {

				//---- get the traffic control ----

				if (leg_ptr->Connect () >= 0) {
					connect_ptr = &sim->connect_array [leg_ptr->Connect ()];
					control = connect_ptr->Control ();
				}

				//---- set plan following / lane change flags ----

				num_cells = max_cell - cell;
		
				low = leg_ptr->Out_Best_Low ();
				high = leg_ptr->Out_Best_High ();

				if (num_cells < sim->param.plan_follow && (lane < low || lane > high)) {
					change_flag = true;

					lanes = (sim->param.lane_change_levels * num_cells + sim->param.plan_follow / 2) / sim->param.plan_follow; 

					if (lanes <= sim->param.change_priority) {
						lanes = sim->param.change_priority - lanes + 1;
						if (lanes > sim_travel_ptr->Priority ()) sim_travel_ptr->Priority (lanes);
					}
				}
			}
		}
	}

	// ---- slow_down ----

	if (slow_down) {
		sim_veh = step.back ();
		step.pop_back ();
		distance -= move_size;
		if (distance < 1) distance = 1;
	}

	//---- place the vehicle at the new location ----

	*sim_veh_ptr = sim_veh;

	if (!sim_veh.Parked ()) {
		lane = sim_veh.lane;
		cell = sim->Offset_Cell (sim_veh.offset);

		sim_dir_ptr->Add (lane, cell, step.Traveler ());

		//---- move multi-cell vehicles ----

		if (veh_type_ptr->Cells () > 1 && ((int) step.size () > 1 || sim->Offset_Cell (step [0].offset) != cell)) {
			Sim_Veh_Array to_step;
			Sim_Veh_Itr sim_veh_itr;

			new_index = (int) step.size () - 2;
			index = sim_travel_ptr->Vehicle ();
#ifdef CHECK
			if (index < 0) sim->Error (String ("Sim_Node_Process::Move_Vehicle: vehicle index, index=%d") % index);
#endif
			for (cell=1; cell < veh_type_ptr->Cells (); cell++, new_index--) {
				if (new_index >= 0) {
					to_step.push_back (step [new_index]);
				} else {
#ifdef CHECK
					if ((index - new_index) < 0) sim->Error (String ("Sim_Node_Process::Move_Vehicle: sim_veh_ptr, index=%d, new_index=%d") % index % new_index);
#endif
					to_step.push_back (sim->sim_veh_array [index - new_index]);
				}
			}
			new_index = -(step.Traveler ());

			for (++index, sim_veh_itr = to_step.begin (); sim_veh_itr != to_step.end (); sim_veh_itr++, index++) {
				sim_veh_ptr = &sim->sim_veh_array [index];

				//---- remove the vehicle from the original location ----

				if (!sim_veh_ptr->Parked ()) {
					if (sim_veh_ptr->link != dir_index) {
						dir_index = sim_veh_ptr->link;
						sim_dir_ptr = &sim->sim_dir_array [dir_index];
					}
					lane = sim_veh_ptr->lane;
					cell = sim->Offset_Cell (sim_veh_ptr->offset);

					if (sim_dir_ptr->Get (lane, cell) != step.Traveler ()) {
						sim_dir_ptr->Remove (lane, cell);
					} else {
						sim_dir_ptr->Remove ();
					}
				}

				//---- add the vehicle at the new location ----

				*sim_veh_ptr = *sim_veh_itr;

				if (!sim_veh_ptr->Parked ()) {
					if (sim_veh_ptr->link != dir_index) {
						dir_index = sim_veh_ptr->link;
						sim_dir_ptr = &sim->sim_dir_array [dir_index];
					}
					lane = sim_veh_ptr->lane;
					cell = sim->Offset_Cell (sim_veh_ptr->offset);

					if (sim_dir_ptr->Get (lane, cell) != step.Traveler ()) {
						sim_dir_ptr->Add (lane, cell, new_index);
					} else {
						sim_dir_ptr->Add ();
					}
				}
			}
		}
	}
	sim_travel_ptr->Speed (DTOI (distance / sim->UnRound (step_size)));

output:

	//---- process the cell movements ----

	sim->Output_Step (step);

	if (sim_travel_ptr->Speed () == 0 && speed == 0) {
		if (sim_travel_ptr->Next_Event () <= sim->time_step) {
			sim_travel_ptr->Add_Wait (step_size);
		}
	} else if (move > 0 || distance > 1) {
		sim_travel_ptr->Wait (0);
		sim_travel_ptr->Priority (0);
	}
	return (exit_flag);
}


