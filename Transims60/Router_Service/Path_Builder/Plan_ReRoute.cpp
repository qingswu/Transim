//*********************************************************
//	Plan_ReRoute - Build a Path after a Specified Time
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_ReRoute
//---------------------------------------------------------

bool Path_Builder::Plan_ReRoute (void)
{
	int stat, len, cost, imp, link, mode;
	Dtime tod, time, reroute_time;
	double factor;
	bool start_flag;
	
	Trip_End trip_end;
	Path_End path_end;
	Path_Data path_data;
	Link_Data *link_ptr;
	Plan_Leg_Itr leg_itr;

	plan_flag = true;

	//---- set the traveler parameters ----

	exe->Set_Parameters (path_param, plan_ptr->Type (), plan_ptr->Veh_Type ());

	path_param.mode = (Mode_Type) plan_ptr->Mode (),
	parking_duration = plan_ptr->Duration ();
	forward_flag = true;

	//---- initialize the plan ----

	from_array.clear ();	
	trip_org.clear ();
	trip_des.clear ();

	reroute_time = plan_ptr->Reroute_Time ();
	start_flag = false;
	tod = plan_ptr->Depart ();

	//---- find the path mode at reroute time ---

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		tod += leg_itr->Time ();
		if (leg_itr->Mode () == DRIVE_MODE) start_flag = true;

		if (tod >= reroute_time) {
			if (leg_itr->Mode () == DRIVE_MODE) break;

			if (start_flag) {
				//if (path_param.flow_flag) {
				//	return (Plan_Flow (plan_data));
				//} else {
					return (true);
				//}
			} else {
				return (Plan_Build ());
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
			if (!leg_itr->Link_Type ()) return (true);

			path_end.Clear ();
			path_end.Trip_End (0);
			path_end.End_Type (FROM_ID);
			path_end.Type (LINK_ID);

			link = leg_itr->Link_ID ();

			path_end.Index (link);
			link_ptr = &exe->link_array [link];

			path_end.Offset (len);

			if (leg_itr->Link_Dir () == 0) {
				path_data.From (link_ptr->AB_Dir ());
			} else {
				path_data.From (link_ptr->BA_Dir ());
			}
			path_data.Type (DIR_ID);

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
	//if (path_param.flow_flag) {
	//	Plan_Flow (plan_data);
	//}

	//---- set the destination ----

	trip_end.Index (plan_ptr->Destination ());
	trip_end.Type (LOCATION_ID);
	trip_end.Time (plan_ptr->End ());

	trip_des.push_back (trip_end);

	stat = Build_Path (-1);

	if (stat < 0) return (false);
	if (stat > 0) {
		if (!path_param.ignore_errors) {
			//skip = true;
		}
		plan_ptr->Problem (stat);
	}
	return (true);
}
