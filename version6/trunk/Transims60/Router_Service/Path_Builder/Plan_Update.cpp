//*********************************************************
//	Plan_Update - Update Path and Plan records
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Update
//---------------------------------------------------------

bool Path_Builder::Plan_Update (Plan_Data *plan_data)
{
	int i, mode, index, dir_index, nlegs, len, cost, imp, use_index, group;
	Dtime time, ttime, delay;
	bool first_park, ab_flag;
	double factor;

	Plan_Leg *leg_ptr;
	Perf_Period *perf_period_ptr = 0;
	Turn_Period *turn_period_ptr = 0;
	Turn_Data *turn_ptr;
	Link_Data *link_ptr = 0;
	Dir_Data *dir_ptr = 0;
	Connect_Data *connect_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Access_Data *access_ptr;

	if (plan_data == 0) {
		cout << "\tPlan Pointer is Zero" << endl;
		return (false);
	}
	plan_flag = true;
	plan_ptr = plan_data;
	plan_ptr->Problem (0);

	mode = plan_ptr->Mode ();
		
	if (mode == WAIT_MODE || mode == TRANSIT_MODE || mode == WALK_MODE || mode == BIKE_MODE || 
		mode == RIDE_MODE || mode == OTHER_MODE) return (true);

	//---- set the traveler parameters ----

	exe->Set_Parameters (path_param, plan_ptr->Type (), plan_ptr->Veh_Type ());

	random_flag = (path_param.random_imped > 0);
	turn_flag = (path_param.left_imped > 0 || path_param.right_imped > 0 || path_param.uturn_imped > 0);

	parking_duration = plan_ptr->Duration ();
	forward_flag = (plan_ptr->Constraint () != END_TIME);
	reroute_flag = false;

	mode = plan_ptr->Mode ();
	path_param.mode = (Mode_Type) mode;
	group = 0;

	//---- trace the path ----

	if (forward_flag) {
		time = plan_ptr->Start ();
		plan_ptr->Depart (time);
	} else {
		time = plan_ptr->End ();
		plan_ptr->Arrive (time);
	}
	nlegs = (int) plan_ptr->size ();

	plan_ptr->Zero_Totals ();
	dir_index = -1;
	first_park = true;

	for (i=0; i < nlegs; i++) {
		if (forward_flag) {
			leg_ptr = &plan_ptr->at (i);
		} else {
			leg_ptr = &plan_ptr->at (nlegs - i - 1);
		}
		mode = leg_ptr->Mode ();
		ttime = leg_ptr->Time ();
		len = leg_ptr->Length ();
		cost = leg_ptr->Cost ();
		imp = leg_ptr->Impedance ();

		if (mode == DRIVE_MODE) {
			perf_period_ptr = exe->perf_period_array.Period_Ptr (time);
			if (perf_period_ptr != 0) {

				if (leg_ptr->Link_Type ()) {
					index = leg_ptr->Link_ID ();

					link_ptr = &exe->link_array [index];

					if (leg_ptr->Link_Dir ()) {
						index = link_ptr->BA_Dir ();
					} else {
						index = link_ptr->AB_Dir ();
					}
					dir_ptr = &exe->dir_array [index];

				} else if (leg_ptr->Dir_Type ()) {
					index = leg_ptr->ID ();

					dir_ptr = &exe->dir_array [index];
					link_ptr = &exe->link_array [dir_ptr->Link ()];
				} else {
					index = -1;
				}
				if (index >= 0) {
					if (len > link_ptr->Length ()) len = link_ptr->Length ();
					factor = (double) len / link_ptr->Length ();

					if (!Best_Lane_Use (index, time, factor, ttime, delay, cost, group)) {
						//---- problem ----
					}
					cost += DTOI (path_param.op_cost_rate * len);

					if (path_param.flow_flag) {
						if (group == 1) {
							use_index = dir_ptr->Use_Index (); 
						} else {
							use_index = index;
						}
						perf_period_array_ptr->Flow_Time (use_index, time, factor, link_ptr->Length (), path_param.pce, path_param.occupancy, forward_flag);
					}
					imp = 0;

					if (path_param.grade_flag) {
						if (dir_ptr->Dir ()) {
							ab_flag = false;
						} else {
							ab_flag = true;
						}
						if (link_ptr->Grade (ab_flag) > 0) {
							ttime = ttime / path_param.veh_type_ptr->Grade (link_ptr->Grade (ab_flag));
						}
					}
					ttime += delay;

					if (dir_index >= 0) {
						if (forward_flag) {
							map2_itr = exe->connect_map.find (Int2_Key (dir_index, index));
						} else {
							map2_itr = exe->connect_map.find (Int2_Key (index, dir_index));
						}
						if (map2_itr != exe->connect_map.end ()) {
							if (path_param.turn_delay_flag) {
								turn_period_ptr = exe->turn_period_array.Period_Ptr (time);
								ttime += turn_period_ptr->Time (map2_itr->second);
								if (ttime < 1) ttime = 1;
							}
							if (path_param.turn_flow_flag) {
								turn_period_ptr = turn_period_array_ptr->Period_Ptr (time);
								turn_ptr = turn_period_ptr->Data_Ptr (map2_itr->second);
								turn_ptr->Add_Turn (path_param.pce);
							}
							if (turn_flag) {
								connect_ptr = &exe->connect_array [map2_itr->second];

								if (connect_ptr->Type () == LEFT) {
									imp += path_param.left_imped;
								} else if (connect_ptr->Type () == RIGHT) {
									imp += path_param.right_imped;
								} else if (connect_ptr->Type () == UTURN) {
									imp += path_param.uturn_imped;
								}
							}
						}
					}
					imp += Resolve (ttime * path_param.value_time + len * path_param.value_dist + cost * path_param.value_cost);

					if (link_ptr->Type () == FREEWAY) {
						imp = DTOI (imp * path_param.freeway_fac);
					} else if (link_ptr->Type () == EXPRESSWAY) {
						imp = DTOI (imp * path_param.express_fac);
					}
					dir_index = index;

					if (random_flag) {
						imp = DTOI (imp * (1.0 + path_param.random_imped * (path_param.random.Probability () - 0.5) / 100.0));
					}
				}
			}
			plan_ptr->Add_Drive (ttime);
		} else if (mode == TRANSIT_MODE) {
			plan_ptr->Add_Transit (ttime);
		} else if (mode == WALK_MODE) {
			if (leg_ptr->Access_Type ()) {
				index = leg_ptr->Access_ID ();
				access_ptr = &exe->access_array [index];

				ttime = access_ptr->Time ();
				cost = Round (access_ptr->Cost ());
				imp = Resolve (ttime * path_param.value_walk + cost * path_param.value_cost);
			} else {
				imp = Resolve (ttime * path_param.value_walk);
			}
			if (random_flag) {
				imp = DTOI (imp * (1.0 + path_param.random_imped * (path_param.random.Probability () - 0.5) / 100.0));
			}
			plan_ptr->Add_Walk (ttime);
		} else if (mode == WAIT_MODE) {
			plan_ptr->Add_Wait (ttime);
		} else {
			if (leg_ptr->Type () == PARKING_ID) {
				index = leg_ptr->ID ();

				Parking_Data *parking_ptr;
				Park_Nest_Itr park_itr;
				ttime = 0;
				cost = 0;

				//if (!first_park && veh_ptr != 0) {
				//	veh_ptr->Parking (map_itr->second);
				//}
				parking_ptr = &exe->parking_array [index];

				if (parking_ptr->size () > 0) {
					for (park_itr = parking_ptr->begin (); park_itr != parking_ptr->end (); park_itr++) {
						if (exe->Use_Permission (park_itr->Use (), path_param.use) &&
							park_itr->Start () <= time && time < park_itr->End ()) {

							if (forward_flag && first_park) {
								ttime = park_itr->Time_Out ();
							} else {
								ttime = park_itr->Time_In ();
								cost = DTOI (park_itr->Hourly () * parking_duration.Hours ());
								if (cost > park_itr->Daily ()) cost = park_itr->Daily ();
								cost = Round (cost);
							}
							break;
						}
					}
				}
				imp = Resolve (ttime * path_param.value_park + cost * path_param.value_cost);

				if (random_flag) {
					imp = DTOI (imp * (1.0 + path_param.random_imped * (path_param.random.Probability () - 0.5) / 100.0));
				}
				first_park = !first_park;
			}
			plan_ptr->Add_Other (ttime);
		}
		leg_ptr->Time (ttime);
		leg_ptr->Length (len);
		leg_ptr->Cost (cost);
		leg_ptr->Impedance (imp);

		plan_ptr->Add_Length (len);
		plan_ptr->Add_Cost (cost);
		plan_ptr->Add_Impedance (imp);

		if (forward_flag) {
			time += ttime;
		} else {
			time -= ttime;
		}
	}
	plan_ptr->Activity (plan_ptr->Duration ());

	if (forward_flag) {
		plan_ptr->Arrive (time.Round_Seconds ());

		if (plan_ptr->Arrive () > plan_ptr->End () && plan_ptr->Duration () > 0 && 
			plan_ptr->Constraint () != FIXED_TIME && plan_ptr->Constraint () != DURATION) {

			delay = DTOI (plan_ptr->Duration () * path_param.duration_factor [plan_ptr->Priority ()]);
			time = plan_ptr->End () + plan_ptr->Duration ();
			if (time > plan_ptr->Arrive () + delay.Round_Seconds ()) {
				plan_ptr->Activity (time - plan_ptr->Arrive ());
			} else {
				plan_ptr->Activity (delay.Round_Seconds ());
			}
		}
	} else {
		plan_ptr->Depart (time.Round_Seconds ());
	}
	return (true);
}
