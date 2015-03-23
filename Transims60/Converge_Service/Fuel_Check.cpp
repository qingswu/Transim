//*********************************************************
//	Fuel_Check.cpp - check the fuel supply and build path
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Fuel_Check
//---------------------------------------------------------

void Converge_Service::Fuel_Check (Plan_Data &plan)
{
	if (plan.size () == 0) return;

	double supply = current_fuel [plan.Index ()];
	if (supply <= 0) return;

	Veh_Type_Data *veh_type_ptr = &veh_type_array [plan.Veh_Type ()];
	int seek_level = DTOI (veh_type_ptr->Fuel_Cap () * seek_fuel / 100.0);
	if (seek_level < 1) seek_level = 1;

	int function = veh_type_ptr->Fuel_Func ();
	if (function < 0) function = 0;

	Dtime tod;
	int leg, fuel_leg;
	double speed, factor, ttime;

	Loc_Fuel_Data *fuel_ptr;
	Plan_Leg_Itr itr;

	factor = (Metric_Flag ()) ? 1000.0 : 5280.0;

	//---- find the last fuel location ----

	fuel_leg = -1;

	for (leg=0, itr = plan.begin (); itr != plan.end (); itr++, leg++) {
		if (itr->Type () == LOCATION_ID && itr->Mode () == WAIT_MODE) {
			fuel_ptr = &loc_fuel_array [itr->ID ()];
			if (fuel_ptr->supply > 0) {
				fuel_leg = leg;
			}
		}
	}

	//---- monitor the fuel supply ----

	tod = plan.Depart ();

	for (leg=0, itr = plan.begin (); itr != plan.end (); itr++, leg++) {
		tod += itr->Time ();

		if (itr->Type () == LOCATION_ID && itr->Mode () == WAIT_MODE) {
			fuel_ptr = &loc_fuel_array [itr->ID ()];
			if (fuel_ptr->supply <= 0) continue;
		
			if (leg < fuel_leg) {
MAIN_LOCK
				fuel_ptr->failed++;
END_LOCK
				continue;
			}

			//---- try to fill the tank ----
MAIN_LOCK
			if ((fuel_ptr->consumed + veh_type_ptr->Fuel_Cap ()) <= fuel_ptr->supply) {
				fuel_ptr->consumed += veh_type_ptr->Fuel_Cap ();
				supply = veh_type_ptr->Fuel_Cap ();
			} else {
				fuel_ptr->failed++;
				Set_Problem (FUEL_PROBLEM);
			}
END_LOCK
			continue;
		}

		//---- consume fuel ----

		if (itr->Mode () != DRIVE_MODE) continue;

		ttime = itr->Time () / 3600.0;

		if (ttime > 0) {
			speed = itr->Length () / (factor * ttime);
		} else {
			speed = 1;
		}
		supply -= ttime * functions.Apply_Function (function, speed);

		if (supply > seek_level || leg < fuel_leg) continue;

		//---- look for fuel ----

		if (itr->Type () != DIR_ID) continue;

		int i, best;
		double x1, y1, x2, y2, x, y, dx, dy, diff1, diff, best_diff;
		Dir_Data *dir_ptr;
		Link_Data *link_ptr;
		Node_Data *node_ptr;
		Location_Itr loc_itr;
		Location_Data *loc_ptr;

		dir_ptr = &dir_array [itr->ID ()];
		link_ptr = &link_array [dir_ptr->Link ()];
		if (dir_ptr->Dir () == 0) {
			i = link_ptr->Bnode ();
		} else {
			i = link_ptr->Anode ();
		}
		node_ptr = &node_array [i];
		x1 = node_ptr->X ();
		y1 = node_ptr->Y ();

		loc_ptr = &location_array [plan.Destination ()];
		x2 = loc_ptr->X ();
		y2 = loc_ptr->Y ();

		dx = x2 - x1;
		dy = y2 - y1;

		diff1 = dx * dx + dy * dy;

		best = -1;
		best_diff = 0;

		for (i=0, loc_itr = location_array.begin (); loc_itr != location_array.end (); loc_itr++, i++) {
			fuel_ptr = &loc_fuel_array [i];

			if (fuel_ptr->supply <= 0) continue;

			if (choice_flag && plan.Method () == BUILD_PATH && choice_range.In_Range (plan.Type ())) {
				if ((fuel_ptr->consumed + veh_type_ptr->Fuel_Cap ()) > fuel_ptr->supply) continue;
			}

			x = loc_itr->X ();
			y = loc_itr->Y ();

			dx = x - x2;
			dy = y - y2;

			diff = dx * dx + dy * dy;
			if (diff > diff1) continue;

			dx = x - x1;
			dy = y - y1;

			diff += (dx * dx + dy * dy) * 2.0;

			if (best < 0 || diff < best_diff) {
				best = i;
				best_diff = diff;
			}
		}
		if (best < 0) {
			plan.Problem (FUEL_PROBLEM);
			return;
		}

		//---- stop at the gas station ----

		Time_Index new_index = plan.Get_Time_Index ();

		plan.Start (plan.Depart ());
		plan.Reroute_Time (tod);
		plan.Stop_Location (best);
		plan.Activity (fuel_duration);

		plan.Constraint (START_TIME);
		plan.Priority (CRITICAL);

		//---- save and update iterator -----
MAIN_LOCK
		Time_Index index = time_itr->first;

		if (tod <= index.Start ()) {
			new_index.Start (index.Start () + 1);
		} else {
			new_index.Start (tod);
		}
		if (!plan_time_map.insert (Time_Map_Data (new_index, plan.Index ())).second) {
			Warning (String ("Time Index Problem Plan %d-%d-%d-%d") % plan.Household () %
				plan.Person () % plan.Tour () % plan.Trip ());
		}
		time_itr = plan_time_map.find (index);
END_LOCK
		plan.Problem (FUEL_PROBLEM);
		break;
	}
}
