//*********************************************************
//	Best_Lanes.cpp - Entry/Exit Lane Options
//*********************************************************

#include "Sim_Plan_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Best_Lanes
//---------------------------------------------------------

bool Sim_Plan_Process::Best_Lanes (Sim_Trip_Ptr sim_trip_ptr, Integers &leg_list)
{
	int index, offset, low_lane, high_lane, lane, org_off, des_off, length;
	int in_low, in_high, best_low, best_high, out_low, out_high, best, total;
	int dir_index, to_index, lane_factor, in_lanes, out_lanes, period, period1, period2;
	bool first_lot, drive_flag, bound_flag, flag, first_leg;
	Dtime time;

	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Leg_Pool_Ptr leg_pool_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Sim_Leg_Data *leg_ptr, *last_leg;
	Sim_Dir_Ptr sim_dir_ptr;
	Connect_Data *connect_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Veh_Type_Data *veh_type_ptr;
	Int_RItr int_ritr, from_ritr;
	Int_Itr int_itr;

	low_lane = high_lane = -1;

	drive_flag = false;
	first_lot = true;
	org_off = des_off = dir_index = offset = 0;
	length = 1;

	sim_travel_ptr = &sim_trip_ptr->sim_travel_data;
	sim_plan_ptr = &sim_trip_ptr->sim_plan_data;

	veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];

	//---- set the time period range ----

	period1 = sim->sim_periods.Period (sim_plan_ptr->Start ());
	if (period1 < 0) period1 = 0;

	time = MIN ((3 * (sim_plan_ptr->End () - sim_plan_ptr->Start ())), sim->param.max_end_variance);

	period2 = sim->sim_periods.Period (sim_plan_ptr->End () + time);

	if (period2 < 0) period2 = sim->sim_periods.Num_Periods () - 1;
	if (period2 < period1) period2 = period1;

	//---- process each leg ----

	time = 0;
	first_leg = true;
	last_leg = 0;
	leg_pool_ptr = &sim->sim_leg_array [sim_plan_ptr->Leg_Pool ()];

	for (int_itr = leg_list.begin (); int_itr != leg_list.end (); int_itr++, last_leg = leg_ptr) {
		leg_ptr = leg_pool_ptr->Record_Pointer (*int_itr);

		if (first_leg) {
			last_leg = leg_ptr;
			first_leg = false;
		}
		index = leg_ptr->Index ();

		//---- destination parking lot ----

		if (leg_ptr->Type () == PARKING_ID && !first_lot) {
			first_lot = true;
			drive_flag = true;
			last_leg->Connect (-1);

			sim_dir_ptr = &sim->sim_dir_array [last_leg->Index ()];

			for (flag=false, period=period1; period <= period2; period++) {
				if (sim->period_subarea_method [period] [sim_dir_ptr->Subarea ()] != NO_SIMULATION) {
					flag = true;
					break;
				}
			}
			if (!flag) continue;

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
			continue;
		}
		if (leg_ptr->Mode () != DRIVE_MODE) continue;

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
			leg_ptr->In_Lane_Low (low_lane);
			leg_ptr->In_Lane_High (high_lane);
			leg_ptr->In_Best_Low (low_lane);
			leg_ptr->In_Best_High (high_lane);

			dir_index = to_index;
			des_off = length = sim_dir_ptr->Length ();
			time = dir_ptr->Time0 ();
			first_lot = false;
			continue;
		}

		//---- skip links outside the subarea ---

		for (flag=false, period=period1; period <= period2; period++) {
			if (sim->period_subarea_method [period] [sim_dir_ptr->Subarea ()] != NO_SIMULATION) {
				flag = true;
				break;
			}
		}
		if (!flag) continue;

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
MAIN_LOCK
				sim->Warning (String ("Plan %d-%d-%d-%d Connection was Not Found between Links %d and %d") % 
					sim_travel_ptr->Household () % sim_travel_ptr->Person () % sim_plan_ptr->Tour () % 
					sim_plan_ptr->Trip () % dir_index % to_index);
END_LOCK
			}	
			return (false);
		}
		connect_ptr = &sim->connect_array [map2_itr->second];

		last_leg->Out_Lane_Low (connect_ptr->Low_Lane ());
		last_leg->Out_Lane_High (connect_ptr->High_Lane ());
		last_leg->Out_Best_Low (connect_ptr->Low_Lane ());
		last_leg->Out_Best_High (connect_ptr->High_Lane ());

		leg_ptr->In_Lane_Low (connect_ptr->To_Low_Lane ());
		leg_ptr->In_Lane_High (connect_ptr->To_High_Lane ());
		leg_ptr->In_Best_Low (connect_ptr->To_Low_Lane ());
		leg_ptr->In_Best_High (connect_ptr->To_High_Lane ());

		if (connect_ptr->Speed () > 0) {
			last_leg->Max_Speed (connect_ptr->Speed ());
		} else {
			last_leg->Max_Speed (sim_dir_ptr->Speed ());
		}
		last_leg->Connect (map2_itr->second);
			
		time = MIN ((time * (des_off - org_off) / length), 1);

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

	for (int_ritr = leg_list.rbegin (); int_ritr != leg_list.rend (); int_ritr++) {
		leg_ptr = leg_pool_ptr->Record_Pointer (*int_ritr);

		if (leg_ptr->Type () != PARKING_ID && leg_ptr->Mode () != DRIVE_MODE) continue;

		if (leg_ptr->Type () == PARKING_ID && first_lot) {
			offset = des_off;
			first_lot = false;
			continue;
		}
		from_ritr = int_ritr + 1;
		if (from_ritr == leg_list.rend ()) break;

		last_leg = leg_pool_ptr->Record_Pointer (*from_ritr);

		if (last_leg->Type () == PARKING_ID) {
			offset -= org_off;
		} else if (last_leg->Type () != DIR_ID) {
			break;
		}
		in_low = best_low = leg_ptr->In_Lane_Low ();
		in_high = best_high = leg_ptr->In_Lane_High ();
		total += offset;

		if (total <= sim->param.plan_follow) {
			lane = total / lane_factor;
			best = leg_ptr->Out_Best_Low () - lane;
			if (best_low < best) {
				best_low = MIN (best_high, best);
			}
			best = leg_ptr->Out_Best_High () + lane;
			if (best_high > best) {
				best_high = MAX (best_low, best);
			}
		} else {
			total = offset;

			if (total <= sim->param.plan_follow) {
				lane = total / lane_factor;
				best = leg_ptr->Out_Lane_Low () - lane;

				if (best_low < best) {
					best_low = MIN (best_high, best);
				}
				best = leg_ptr->Out_Lane_High () + lane;

				if (best_high > best) {
					best_high = MAX (best_low, best);
				}
			}
		}
		if (last_leg->Type () != PARKING_ID) {
			if (total <= sim->param.plan_follow) {
				out_low = last_leg->Out_Best_Low ();
				out_high = last_leg->Out_Best_High ();

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
				last_leg->Out_Best_Low (out_low);
				last_leg->Out_Best_High (out_high);
			}
			dir_index = last_leg->Index ();

			sim_dir_ptr = &sim->sim_dir_array [dir_index];
			offset = sim_dir_ptr->Length ();
		}
		leg_ptr->In_Best_Low (best_low);
		leg_ptr->In_Best_High (best_high);
	}
	return (true);
}
