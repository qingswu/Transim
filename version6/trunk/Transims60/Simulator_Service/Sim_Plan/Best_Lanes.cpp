//*********************************************************
//	Best_Lanes.cpp - Entry/Exit Lane Options
//*********************************************************

#include "Sim_Plan_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Best_Lanes
//---------------------------------------------------------

bool Sim_Plan_Process::Best_Lanes (Sim_Travel_Ptr sim_travel_ptr)
{
	int index, offset, low_lane, high_lane, lane, org_off, des_off, length;
	int in_low, in_high, best_low, best_high, out_low, out_high, best, total;
	int dir_index, to_index, lane_factor, in_lanes, out_lanes;
	bool first_lot, drive_flag, bound_flag;
	Dtime time;

	Sim_Plan_Ptr sim_plan_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Sim_Leg_Itr leg_itr, last_leg;
	Sim_Leg_RItr leg_ritr, from_leg;
	Sim_Dir_Ptr sim_dir_ptr;
	Connect_Data *connect_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Veh_Type_Data *veh_type_ptr;

	low_lane = high_lane = -1;

	drive_flag = false;
	first_lot = true;
	org_off = des_off = dir_index = offset = 0;
	length = 1;
	time = 0;

	sim_plan_ptr = sim_travel_ptr->sim_plan_ptr;

	veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];

	for (leg_itr = last_leg = sim_plan_ptr->begin (); leg_itr != sim_plan_ptr->end (); last_leg = leg_itr++) {
		index = leg_itr->Index ();

		//---- destination parking lot ----

		if (leg_itr->Type () == PARKING_ID && !first_lot) {
			first_lot = true;
			drive_flag = true;
			last_leg->Connect (-1);

			sim_dir_ptr = &sim->sim_dir_array [last_leg->Index ()];
			if (sim_dir_ptr->Method () == NO_SIMULATION) continue;

			dir_ptr = &sim->dir_array [last_leg->Index ()];

			sim_park_ptr = &sim->sim_park_array [index];
			bound_flag = (sim_park_ptr->Type () == BOUNDARY);

			//---- get the link offset and lane options ----

			if (sim_dir_ptr->Dir () == 0) {
				des_off = sim_park_ptr->Offset_AB ();
				low_lane = sim_park_ptr->Min_Lane_AB ();
				high_lane = sim_park_ptr->Max_Lane_AB ();
			} else {
				des_off = sim_park_ptr->Offset_BA ();
				low_lane = sim_park_ptr->Min_Lane_BA ();
				high_lane = sim_park_ptr->Max_Lane_BA ();
			}

			//---- set speed and lanes ----

			last_leg->Max_Speed (sim_dir_ptr->Speed ());

			if (!bound_flag && last_leg->Max_Speed () > veh_type_ptr->Max_Decel ()) {
				last_leg->Max_Speed (veh_type_ptr->Max_Decel ());
			}
			if (last_leg->Max_Speed () > veh_type_ptr->Max_Speed ()) {
				last_leg->Max_Speed (veh_type_ptr->Max_Speed ());
			}
			last_leg->Out_Lane_Low (low_lane);
			last_leg->Out_Lane_High (high_lane);
			last_leg->Out_Best_Low (low_lane);
			last_leg->Out_Best_High (high_lane);

			time = MIN ((time * (des_off - org_off) / length), 1);
			last_leg->Time (time);
			continue;
		}
		if (leg_itr->Mode () != DRIVE_MODE) continue;

		sim_dir_ptr = &sim->sim_dir_array [index];
		to_index = index;

		dir_ptr = &sim->dir_array [index];

		//---- origin parking lot or link ----

		if (first_lot) {

			if (last_leg->Type () == PARKING_ID) {
				sim_park_ptr = &sim->sim_park_array [last_leg->Index ()];
				bound_flag = (sim_park_ptr->Type () == BOUNDARY);

				if (sim_dir_ptr->Dir () == 0) {
					org_off = sim_park_ptr->Offset_AB ();
					low_lane = sim_park_ptr->Min_Lane_AB ();
					high_lane = sim_park_ptr->Max_Lane_AB ();
				} else {
					org_off = sim_park_ptr->Offset_BA ();
					low_lane = sim_park_ptr->Min_Lane_BA ();
					high_lane = sim_park_ptr->Max_Lane_BA ();
				}
			} else {
				dir_ptr = &sim->dir_array [index];

				bound_flag = true;
				org_off = 0;
				low_lane = dir_ptr->Left ();
				high_lane = dir_ptr->Lanes () + dir_ptr->Left () - 1;
			}

			//---- set speed and lanes ----

			last_leg->Max_Speed (sim_dir_ptr->Speed ());

			if (!bound_flag && last_leg->Max_Speed () > veh_type_ptr->Max_Accel ()) {
				last_leg->Max_Speed (veh_type_ptr->Max_Accel ());
			}
			if (last_leg->Max_Speed () > veh_type_ptr->Max_Speed ()) {
				last_leg->Max_Speed (veh_type_ptr->Max_Speed ());
			}
			leg_itr->In_Lane_Low (low_lane);
			leg_itr->In_Lane_High (high_lane);
			leg_itr->In_Best_Low (low_lane);
			leg_itr->In_Best_High (high_lane);

			dir_index = to_index;
			des_off = length = sim_dir_ptr->Length ();
			time = dir_ptr->Time0 ();
			first_lot = false;
			continue;
		}

		//---- skip links outside the subarea ---

		if (sim_dir_ptr->Method () == NO_SIMULATION) {
			dir_index = to_index;
			continue;
		}

		//---- get the connection to the next link ----

		map2_itr = sim->connect_map.find (Int2_Key (dir_index, to_index));

		if (map2_itr == sim->connect_map.end ()) {
			if (sim->param.print_problems) {
				dir_ptr = &sim->dir_array [to_index];
				link_ptr = &sim->link_array [dir_ptr->Link ()];
				to_index = link_ptr->Link ();

				dir_ptr = &sim->dir_array [dir_index];
				link_ptr = &sim->link_array [dir_ptr->Link ()];
				dir_index = link_ptr->Link ();

				sim->Warning (String ("Plan %d-%d-%d-%d Connection was Not Found between Links %d and %d") % 
					sim_travel_ptr->Household () % sim_travel_ptr->Person () % sim_plan_ptr->Tour () % 
					sim_plan_ptr->Trip () % dir_index % to_index);
			}	
			return (false);
		}
		connect_ptr = &sim->connect_array [map2_itr->second];

		last_leg->Out_Lane_Low (connect_ptr->Low_Lane ());
		last_leg->Out_Lane_High (connect_ptr->High_Lane ());
		last_leg->Out_Best_Low (connect_ptr->Low_Lane ());
		last_leg->Out_Best_High (connect_ptr->High_Lane ());

		leg_itr->In_Lane_Low (connect_ptr->To_Low_Lane ());
		leg_itr->In_Lane_High (connect_ptr->To_High_Lane ());
		leg_itr->In_Best_Low (connect_ptr->To_Low_Lane ());
		leg_itr->In_Best_High (connect_ptr->To_High_Lane ());

		if (connect_ptr->Speed () > 0) {
			last_leg->Max_Speed (connect_ptr->Speed ());
		} else {
			last_leg->Max_Speed (sim_dir_ptr->Speed ());
		}
		last_leg->Connect (map2_itr->second);
			
		time = MIN ((time * (des_off - org_off) / length), 1);
		last_leg->Time (time);

		dir_index = to_index;
		des_off = length = sim_dir_ptr->Length ();
		time = dir_ptr->Time0 ();
		org_off = 0;
	}
	if (!drive_flag) return (true);

	//---- set the best lane alignment ----

	first_lot = true;
	total = offset = 0;
	lane_factor = sim->param.plan_follow * 2 / sim->param.lane_change_levels;
	if (lane_factor < 1) lane_factor = 1;

	for (leg_ritr = sim_plan_ptr->rbegin (); leg_ritr != sim_plan_ptr->rend (); leg_ritr++) {
		if (leg_ritr->Type () != PARKING_ID && leg_ritr->Mode () != DRIVE_MODE) continue;

		if (leg_ritr->Type () == PARKING_ID && first_lot) {
			offset = des_off;
			first_lot = false;
			continue;
		} else {
			from_leg = leg_ritr + 1;
			if (from_leg == sim_plan_ptr->rend ()) break;

			if (from_leg->Type () == PARKING_ID) {
				offset -= org_off;
			} else if (from_leg->Type () != DIR_ID) {
				break;
			}
		}
		in_low = best_low = leg_ritr->In_Lane_Low ();
		in_high = best_high = leg_ritr->In_Lane_High ();
		total += offset;

		if (total <= sim->param.plan_follow) {
			lane = total / lane_factor;
			best = leg_ritr->Out_Best_Low () - lane;
			if (best_low < best) {
				best_low = MIN (best_high, best);
			}
			best = leg_ritr->Out_Best_High () + lane;
			if (best_high > best) {
				best_high = MAX (best_low, best);
			}
		} else {
			total = offset;

			if (total <= sim->param.plan_follow) {
				lane = total / lane_factor;
				best = leg_ritr->Out_Lane_Low () - lane;

				if (best_low < best) {
					best_low = MIN (best_high, best);
				}
				best = leg_ritr->Out_Lane_High () + lane;

				if (best_high > best) {
					best_high = MAX (best_low, best);
				}
			}
		}
		if (from_leg->Type () != PARKING_ID) {
			if (total <= sim->param.plan_follow) {
				out_low = from_leg->Out_Best_Low ();
				out_high = from_leg->Out_Best_High ();

				if (best_low > in_low || best_high < in_high) {
					in_lanes = best_high - best_low + 1;
					out_lanes = out_high - out_low + 1;

					while (out_lanes > in_lanes) {
						if (best_low > in_low) {
							in_low++;
							out_low++;
							out_lanes--;
						}
						if (out_lanes > in_lanes && best_high < in_high) {
							in_high--;
							out_high--;
							out_lanes--;
						}
						if (best_low == in_low && best_high == in_high) break;
					}
				}
				from_leg->Out_Best_Low (out_low);
				from_leg->Out_Best_High (out_high);
			}
			dir_index = from_leg->Index ();

			sim_dir_ptr = &sim->sim_dir_array [dir_index];
			offset = sim_dir_ptr->Length ();
		}
		leg_ritr->In_Best_Low (best_low);
		leg_ritr->In_Best_High (best_high);
	}
	return (true);
}
