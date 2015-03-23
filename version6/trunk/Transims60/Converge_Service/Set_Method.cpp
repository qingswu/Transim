//*********************************************************
//	Set_Method.cpp - set the processing method
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Set_Method
//---------------------------------------------------------

void Converge_Service::Set_Method (Plan_Data &plan)
{
	double prob;
	bool build_flag, extend_flag;
	int copy;
	
	if (plan.Problem () == CONSTRAINT_PROBLEM) {
		plan.Method (EXTEND_PLAN);
		plan.Problem (0);
		return;
	} else if (plan.Problem () == FUEL_PROBLEM) {
		plan.Method (STOP_PLAN);
		plan.Problem (0);
		return;
	}
	extend_flag = false;

	if (plan.Method () == STOP_PLAN) {
		copy = EXTEND_COPY;
	} else if (plan.Method () == EXTEND_PLAN || plan.Method () == EXTEND_COPY) {
		copy = EXTEND_COPY;
		extend_flag = capacity_flag;
	} else {
		copy = COPY_PLAN;
	}

	if (plan.Priority () == SKIP) {
		plan.Method (copy);
	} else if (method == DTA_FLOWS) {
		plan.Method (BUILD_PATH);
	} else if (iteration == 1 && plan.size () == 0) {
		plan.Method (BUILD_PATH);
	} else if (!first_iteration && select_priorities) {
		build_flag = select_priority [plan.Priority ()];

		if (build_flag && max_percent_flag && percent_selected < 1.0) {
			prob = random_select.Probability (Random_Seed () + plan.Household () + iteration);
			if (plan.Priority () > 0) {
				build_flag = (prob <= (percent_selected * plan.Priority ()));
			} else {
				build_flag = (prob <= percent_selected);
			}
		}
		if (build_flag) {
			plan.Method (BUILD_PATH);
		} else if (method == DUE_PLANS) {
			plan.Method (copy);
		} else {
			plan.Method (UPDATE_PLAN);
		}
	} else if (method == DUE_PLANS) {
		plan.Method (BUILD_PATH);
	} else if (plan.size () == 0 || plan.Problem () > 0) {
		plan.Method (BUILD_PATH);
	} else if (reroute_flag) {
		if (plan.Depart () < reroute_time && plan.Arrive () > reroute_time) {
			plan.Method (REROUTE_PATH);
			plan.Reroute_Time (reroute_time);
			plan.Constraint (START_TIME);
		} else if (plan.Depart () >= reroute_time) {
			plan.Method (BUILD_PATH);
		} else if (plan.Arrive () < reroute_time) {
			plan.Method (COPY_PLAN);
		}
	} else if (update_flag) {
		plan.Method (UPDATE_PLAN);
	} else if (Link_Flows ()) {
		plan.Method (PATH_FLOWS);
	} else {
		plan.Method (COPY_PLAN);
	}
	if (extend_flag && plan.Method () == BUILD_PATH) {
		Plan_Leg_Itr leg_itr;
		Loc_Cap_Data *loc_cap_ptr;

		for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
			if (leg_itr->Type () == LOCATION_ID && leg_itr->Mode () == WAIT_MODE) {
				loc_cap_ptr = &loc_cap_array [leg_itr->ID ()];

				if (loc_cap_ptr->capacity > 0) {
					plan.Destination (leg_itr->ID ());
					break;
				}
			}
		}
	}
}
