//*********************************************************
//	Sum_Path.cpp - sum the lane occupancy and lane changes
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sum_Path
//---------------------------------------------------------

int Sim_Node_Process::Sum_Path (Sim_Dir_Ptr sim_dir_ptr, int lane, int cell, Travel_Step &step)
{
	int cells, num_cells, sum_speed, num_lanes, num_veh, best_weight, weight;
	int speed, max_cell, dir_index, traveler, from_lane, from_index, l, high, low;

	Dir_Data *dir_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Ptr leg_ptr, next_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Sim_Travel_Ptr sim_travel_ptr;

	//---- initial conditions ----

	num_cells = sim->Offset_Cell (sim->param.look_ahead);
	sum_speed = num_lanes = num_veh = 0;

	speed = step.sim_travel_ptr->Speed ();
	sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();

	//---- get the plan data ----

	leg_ptr = step.sim_leg_ptr;
	next_ptr = sim_plan_ptr->Get_Next (leg_ptr);

	dir_index = leg_ptr->Index ();
	max_cell = sim_dir_ptr->Max_Cell ();

	if (next_ptr != 0 && next_ptr->Type () == PARKING_ID) {
		sim_park_ptr = &sim->sim_park_array [next_ptr->Index ()];

		if (sim_park_ptr->Type () != BOUNDARY) {
			if (sim_dir_ptr->Dir () == 0) {
				max_cell = sim->Offset_Cell (sim_park_ptr->Offset_AB ());
			} else {
				max_cell = sim->Offset_Cell (sim_park_ptr->Offset_BA ());
			}
		}
	}

	for (cells=1; cells <= num_cells; cells++) {
		cell++;

		//---- move forward ----

		if (cell > max_cell) {

			//---- set the exit speed ----

			if (leg_ptr->Max_Speed () < speed) {
				speed = leg_ptr->Max_Speed ();
			}
		
			//---- check the exit lane ----

			if (lane < leg_ptr->Out_Lane_Low ()) {
				num_lanes += leg_ptr->Out_Lane_Low () - lane;
				lane = leg_ptr->Out_Lane_Low ();
				speed = 0;
			} else if (lane > leg_ptr->Out_Lane_High ()) {
				num_lanes += lane - leg_ptr->Out_Lane_High ();
				lane = leg_ptr->Out_Lane_High ();
				speed = 0;
			}
			from_lane = lane;
			from_index = dir_index;

			if (next_ptr == 0 || next_ptr->Type () != DIR_ID) break;

			dir_index = next_ptr->Index ();

			sim_dir_ptr = &sim->sim_dir_array [dir_index];

			if (sim_dir_ptr->Method () < MESOSCOPIC) break;

			leg_ptr = next_ptr;

			//---- find the best connection ----

			lane = -1;
			best_weight = 0;

			low = leg_ptr->In_Lane_Low ();
			high = leg_ptr->In_Lane_High ();

			for (l=low; l < high; l++) {
				weight = 0;
				if (sim_dir_ptr->Thru_Link (l) == from_index && sim_dir_ptr->Thru_Lane (l) == from_lane) {
					weight += sim->param.connect_lane_weight;
				}
				if (Cell_Use (sim_dir_ptr, l, 0, step)) {
					weight += sim->param.lane_use_weight;
				}
				if (l >= leg_ptr->In_Best_Low () && l <= leg_ptr->In_Best_High ()) {
					weight += sim->param.connect_lane_weight;
				}

				if (weight > best_weight) {
					lane = l;
					best_weight = weight;
				}
			}
			if (lane < 0) break;
			cell = 0;

			next_ptr = sim_plan_ptr->Get_Next (leg_ptr);

			max_cell = sim_dir_ptr->Max_Cell ();

			if (next_ptr != 0 && next_ptr->Type () == PARKING_ID) {
				sim_park_ptr = &sim->sim_park_array [next_ptr->Index ()];

				if (sim_park_ptr->Type () != BOUNDARY) {
					if (sim_dir_ptr->Dir () == 0) {
						max_cell = sim->Offset_Cell (sim_park_ptr->Offset_AB ());
					} else {
						max_cell = sim->Offset_Cell (sim_park_ptr->Offset_BA ());
					}
				}
			}
		}

		//---- check the use restrictions ----

		if (!Cell_Use (sim_dir_ptr, lane, cell, step)) {
			if (lane > 0) {
				while (lane > 0) {
					num_lanes++;
					if (Cell_Use (sim_dir_ptr, --lane, cell, step)) break;
				}
			} else {
				while (lane < (sim_dir_ptr->Lanes () - 1)) {
					num_lanes++;
					if (Cell_Use (sim_dir_ptr, ++lane, cell, step)) break;
				}
			}
			speed = 0;

		} else {

			//---- check the cell availability -----

			traveler = sim_dir_ptr->Get (lane, cell);

			if (traveler == -1) {

				//---- pocket lane ----

				dir_ptr = &sim->dir_array [dir_index];

				if (lane <= dir_ptr->Left ()) {
					while (lane <= dir_ptr->Left ()) {
						num_lanes++;
						if (sim_dir_ptr->Get (++lane, cell) >= 0) break;
					}
				} else {
					while (lane >= (dir_ptr->Left () + dir_ptr->Lanes ())) {
						num_lanes++;
						if (sim_dir_ptr->Get (--lane, cell) >= 0) break;
					}
				}
				speed = 0;

			} else {

				//---- accelerate ----

				speed += step.veh_type_ptr->Max_Accel ();

				if (speed > step.veh_type_ptr->Max_Speed ()) {
					speed = step.veh_type_ptr->Max_Speed ();
				}
				if (speed > sim_dir_ptr->Speed ()) {
					speed = sim_dir_ptr->Speed ();
				}

				//---- check the vehicle speed ----

				traveler = abs (traveler);

				if (traveler > 1) {
					sim_travel_ptr = &sim->sim_travel_array [traveler];

					if (sim_travel_ptr->Speed () < speed) {
						speed = sim_travel_ptr->Speed ();
					}
					num_veh++;
				}
			}
		}
		sum_speed += speed - step.Delay ();
	}
	sim->Offset_Cell (sum_speed);
	sum_speed = DTOI (sum_speed * sim->param.time_factor - num_lanes * sim->param.lane_factor - num_veh * sim->param.veh_factor);

	return (sum_speed);	
}
