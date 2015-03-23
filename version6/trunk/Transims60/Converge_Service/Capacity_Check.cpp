//*********************************************************
//	Capacity_Check.cpp -  check for a capacity constraint
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Capacity_Check
//---------------------------------------------------------

void Converge_Service::Capacity_Check (Plan_Data &plan)
{
	int index;
	bool constrain_flag;
	double prob, share;

	Dbl_Itr dbl_itr;
	Loc_Cap_Data *loc_cap_ptr;

	if (plan.Method () == EXTEND_COPY) {
		Plan_Leg_Itr leg_itr;

		for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
			if (leg_itr->Type () == LOCATION_ID && leg_itr->Mode () == WAIT_MODE) {
				loc_cap_ptr = &loc_cap_array [leg_itr->ID ()];

				if (loc_cap_ptr->capacity > 0) {
MAIN_LOCK
					loc_cap_ptr->failed++;
END_LOCK							
				}
			}
		}
	}

	loc_cap_ptr = &loc_cap_array [plan.Destination ()];

	if (loc_cap_ptr->capacity == 0) return;

	constrain_flag = false;
MAIN_LOCK
	if (loc_cap_ptr->demand < loc_cap_ptr->capacity) {
		loc_cap_ptr->demand++;
	} else {
		loc_cap_ptr->failed++;
		constrain_flag = true;
		Set_Problem (CONSTRAINT_PROBLEM);
	}
END_LOCK
	if (!constrain_flag) return;

	index = (int) loc_cap_ptr->next_des.size ();

	if (index > 0) {
		Plan_Leg leg_rec;

		if (index > 1) {
			prob = random_des.Probability (Random_Seed () + plan.Household () + plan.Destination () + iteration);
			share = 0;

			for (index=0, dbl_itr = loc_cap_ptr->shares.begin (); dbl_itr != loc_cap_ptr->shares.end (); dbl_itr++, index++) {
				share += *dbl_itr;
				if (prob <= share) break;
			}
		} else {
			index = 0;
		}
		leg_rec.Type (LOCATION_ID);
		leg_rec.ID (plan.Destination ());
		leg_rec.Mode (WAIT_MODE);
		leg_rec.Impedance (Resolve (Value_Time (plan.Type ()) * cap_duration));
		leg_rec.Time (cap_duration);
		leg_rec.Length (0);
		leg_rec.Cost (0);

		plan.push_back (leg_rec);
		plan.Add_Wait (cap_duration);

		plan.Arrive (plan.Arrive () + cap_duration);
		plan.Destination (loc_cap_ptr->next_des [index]);
		plan.Constraint (START_TIME);
		plan.Priority (CRITICAL);

		//---- save and update iterator -----

		Time_Index new_index = plan.Get_Time_Index ();
MAIN_LOCK
		Time_Index index = time_itr->first;

		if (plan.Arrive () <= index.Start ()) {
			new_index.Start (index.Start () + 1);
		} else {
			new_index.Start (plan.Arrive ());
		}
		if (!plan_time_map.insert (Time_Map_Data (new_index, plan.Index ())).second) {
			Warning (String ("Time Index Problem Plan %d-%d-%d-%d") % plan.Household () %
				plan.Person () % plan.Tour () % plan.Trip ());
		}
		time_itr = plan_time_map.find (index);
END_LOCK
	}
	plan.Problem (CONSTRAINT_PROBLEM);
}
