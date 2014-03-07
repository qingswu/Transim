//*********************************************************
//	Plan_ReRoute - Build a Path after a Specified Time
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_ReRoute
//---------------------------------------------------------

bool Path_Builder::Plan_ReRoute (Plan_Data *plan_data)
{
	int veh_id, lot, stat, len, cost, imp, link, mode;
	Dtime tod, time, reroute_time;
	double factor;
	bool start_flag;
	
	Trip_End trip_end;
	Path_End path_end;
	Path_Data path_data;
	Int_Map_Itr map_itr;
	Location_Data *loc_ptr;
	Link_Data *link_ptr;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;
	Plan_Leg_Itr leg_itr;

	if (plan_data == 0) {
		cout << "\tPlan Pointer is Zero" << endl;
		return (false);
	}
	plan_flag = true;
	plan_ptr = plan_data;

	//---- set the traveler parameters ----

	exe->Set_Parameters (param, plan_ptr->Type ());

	param.mode = (Mode_Type) plan_ptr->Mode (),
	parking_duration = plan_ptr->Duration ();
	forward_flag = true;
	reroute_flag = false;

	//---- initialize the plan ----

	from_array.clear ();	
	trip_org.clear ();
	trip_des.clear ();

	reroute_time = plan_ptr->Arrive ();
	start_flag = false;
	tod = plan_ptr->Depart ();

	//---- find the path mode at reroute time ---

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		tod += leg_itr->Time ();
		if (leg_itr->Mode () == DRIVE_MODE) start_flag = true;

		if (tod >= reroute_time) {
			if (leg_itr->Mode () == DRIVE_MODE) break;

			if (start_flag) {
				if (param.flow_flag) {
					return (Plan_Flow (plan_data));
				} else {
					return (true);
				}
			} else {
				return (Plan_Build (plan_data));
			}
		}
	}

	//---- trace the path up to reroute_time ----

	tod = plan_ptr->Depart ();
	plan_ptr->Zero_Totals ();
	factor = 1.0;
	start_flag = false;

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		mode = leg_itr->Mode ();
		time = leg_itr->Time ();
		len = leg_itr->Length ();
		cost = leg_itr->Cost ();
		imp = Round (leg_itr->Impedance ());

		tod += time;
		if (tod > reroute_time && time > 0) {
			factor = (double) (reroute_time - tod + time) / time;

			time = DTOI (time * factor);
			len = DTOI (len * factor);
			cost = DTOI (cost * factor);
			imp = DTOI (imp * factor);

			leg_itr->Time (time);
			leg_itr->Length (len);
			leg_itr->Cost (cost);
			leg_itr->Impedance (Resolve (imp));
		}
		if (mode == DRIVE_MODE) {
			plan_ptr->Add_Drive (time);
			start_flag = true;
		} else if (mode == TRANSIT_MODE) {
			plan_ptr->Add_Transit (time);
		} else if (mode == WALK_MODE) {
			plan_ptr->Add_Walk (time);
		} else if (mode == WAIT_MODE) {
			plan_ptr->Add_Wait (time);
		} else {
			plan_ptr->Add_Other (time);
		}
		plan_ptr->Add_Length (len);
		plan_ptr->Add_Cost (cost);
		plan_ptr->Add_Impedance (Resolve (imp));

		//---- set the origin ----

		if (tod >= reroute_time) {
			path_end.Clear ();
			path_end.Trip_End (0);
			path_end.End_Type (FROM_ID);
			path_end.Type (LINK_ID);

			link = leg_itr->Link_ID ();

			map_itr = exe->link_map.find (link);
			if (map_itr != exe->link_map.end ()) {
				path_end.Index (map_itr->second);

				link_ptr = &exe->link_array [map_itr->second];

				path_end.Offset (len);

				if (leg_itr->Link_Dir () == 0) {
					path_data.From (link_ptr->AB_Dir ());
				} else {
					path_data.From (link_ptr->BA_Dir ());
				}
				path_data.Type (DIR_ID);
			}
			path_data.Time (reroute_time);
			path_data.Imped (Round ((int) plan_ptr->Impedance ()));
			path_data.Length (plan_ptr->Length ());
			path_data.Cost (plan_ptr->Cost ());
			path_data.Walk (plan_ptr->Walk ());
			path_data.Path (-1);

			path_end.push_back (path_data);

			from_array.push_back (path_end);

			trip_end.Type (DIR_ID);
			trip_end.Time (reroute_time);
			trip_end.Index (path_data.From ());

			trip_org.push_back (trip_end);
			break;
		}
	}
	time_limit = MAX_INTEGER;
	reroute_flag = true;

	//---- remove the remaining legs ----

	if (++leg_itr != plan_ptr->end ()) {
		plan_ptr->erase (leg_itr, plan_ptr->end ());
	}
	if (param.flow_flag) {
		Plan_Flow (plan_data);
	}

	//---- set the destination ----

	map_itr = exe->location_map.find (plan_ptr->Destination ());

	if (map_itr == exe->location_map.end ()) {
		plan_ptr->Problem (LOCATION_PROBLEM);
		return (true);
	}
	loc_ptr = &exe->location_array [map_itr->second];

	trip_end.Type (LOCATION_ID);
	trip_end.Index (map_itr->second);
	trip_end.Time (plan_ptr->End ());

	trip_des.push_back (trip_end);

	//---- get the vehicle record ----

	veh_id = plan_ptr->Vehicle ();
	lot = -1;
	pce = 1.0;

	if (veh_id <= 0 || !veh_type_flag) {
		grade_flag = false;
		op_cost_rate = 0.0;
		param.use = CAR;
		param.veh_type = -1;
	} else {
		map_itr = exe->veh_type_map.find (plan_ptr->Veh_Type ());

		if (map_itr != exe->veh_type_map.end ()) {
			param.veh_type = map_itr->second;
			veh_type_ptr = &exe->veh_type_array [param.veh_type];

			param.use = veh_type_ptr->Use ();
			op_cost_rate = UnRound (veh_type_ptr->Op_Cost ());

			if (Metric_Flag ()) {
				op_cost_rate /= 1000.0;
			} else {
				op_cost_rate /= MILETOFEET;
			}
			grade_flag = param.grade_flag && veh_type_ptr->Grade_Flag ();
			pce = UnRound (veh_type_ptr->PCE ());
		} else {
			param.veh_type = -1;
			grade_flag = false;
			op_cost_rate = 0.0;
			param.use = CAR;
			exe->Warning (String ("Vehicle Type %d was Not Found") % plan_ptr->Veh_Type ());
		}
	}
	stat = Build_Path (lot);
	
	if (stat < 0) return (false);
	if (stat > 0) {
		if (!param.ignore_errors) {
			//skip = true;
		}
		plan_ptr->Problem (stat);
	}
	return (true);
}
