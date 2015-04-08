//*********************************************************
//	Array_Processing.cpp - process an array of plans
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Array_Processing
//---------------------------------------------------------

bool Path_Builder::Array_Processing (Plan_Ptr_Array *array_ptr)
{
	Plan_Ptr prev_ptr;
	Plan_Ptr_Itr itr;
	int last_person, last_tour, last_time;
	bool duration_flag, problem_flag, first;
	Dtime minute;

	if (!initialized) {
		cout << "\tPath Building Requires TRANSIMS Router Services" << endl;
		return (0);
	}
	duration_flag = problem_flag = false;
	last_person = last_tour = last_time = parking_lot = -1;
	minute.Minutes (1.0);

	//---- process each trip in the household ----

	for (itr = array_ptr->begin (); itr != array_ptr->end (); itr++) {
		plan_ptr = *itr;

		if (plan_ptr == 0) {
			cout << "\tPlan Pointer is Zero" << endl;
			return (false);
		}
		min_time_limit = 0;

		if (plan_ptr->Person () == last_person) {
			first = false;

			if (last_time >= plan_ptr->Depart () ||
				(last_tour == plan_ptr->Tour () && duration_flag && 
				plan_ptr->Constraint () != FIXED_TIME &&
				plan_ptr->Constraint () != START_TIME)) {

				if (plan_ptr->Method () == COPY_PLAN || plan_ptr->Method () == PATH_FLOWS || plan_ptr->Method () == EXTEND_COPY) {
					plan_ptr->Arrive (plan_ptr->Arrive () - plan_ptr->Depart () + last_time);
				}
				if (!exe->path_param.ignore_time) {
					if (plan_ptr->Constraint () == START_TIME || plan_ptr->Constraint () == FIXED_TIME) {
						if ((last_time - plan_ptr->Depart ()) > path_param.end_time) {
							plan_ptr->Problem (TIME_PROBLEM);
							goto skip;
						}
					} else if (plan_ptr->Constraint () == END_TIME && last_time > 0) {
						min_time_limit = last_time;
					}
				}
				plan_ptr->Depart (last_time);
			}
			last_tour = plan_ptr->Tour ();
		} else {
			last_person = plan_ptr->Person ();
			last_tour = plan_ptr->Tour ();
			parking_lot = -1;
			first = true;
		}
		reroute_flag = false;

		//---- process the plan ----

		switch (plan_ptr->Method ()) {
			case BUILD_PATH:
				if (!Plan_Build ()) return (false);
				break;
			case UPDATE_PLAN:
				if (!Plan_Update ()) return (false);
				break;
			case REROUTE_PATH:
				if (!Plan_ReRoute ()) return (false);
				break;
			case PATH_FLOWS:
				if (!Plan_Flow ()) return (false);
				break;
			case RESKIM_PLAN:
				if (!Plan_Reskim ()) return (false);
				break;
			case COPY_PLAN:
			case EXTEND_COPY:
				if (exe->Link_Flows ()) {
					plan_ptr->Activity (Load_Flow ());
				}
				break;
			case EXTEND_PLAN:
				if (!Plan_Extend ()) return (false);
				break;
			case STOP_PLAN:
				if (plan_ptr->Mode () == TRANSIT_MODE) {
					if (!Stop_Access ()) return (false);
				} else {
					if (!Plan_Stop ()) return (false);
				}
				break;
			default:
				break;
		}

		//---- check for a problem ----

skip:
		if (plan_ptr->Problem () > 0 && exe->path_param.ignore_errors) {
			last_time = -1;
			duration_flag = false;
			problem_flag = true;
		} else {

			//---- gather the end time ----

			last_time = plan_ptr->Arrive ();
			duration_flag = (plan_ptr->Activity () > 0);
			problem_flag = false;

			//---- adjust the duration of this activity ----

			if (duration_flag) {
				if (last_time < plan_ptr->End ()) {

					//---- earlier than expected ----

					if (!path_param.adjust_schedule || 
						(!path_param.ignore_duration && plan_ptr->Constraint () == FIXED_TIME)) {
						last_time = plan_ptr->End ();
					}
				} else if (last_time > plan_ptr->End ()) {

					//---- later than expected ----

					if (path_param.ignore_duration) {
						plan_ptr->Activity (plan_ptr->End () + plan_ptr->Duration () - last_time);
						if (plan_ptr->Activity () < minute) plan_ptr->Activity (minute);
					} 
				}
			}
			last_time += plan_ptr->Activity ();

			//---- adjust duration of previous trip ----

			if (!first && plan_ptr->Constraint () == END_TIME) {
				prev_ptr = *(itr - 1);
				if (prev_ptr->Constraint () != DURATION || path_param.ignore_duration) {
					time_limit = prev_ptr->Arrive () + prev_ptr->Activity ();
					if (time_limit > plan_ptr->Depart ()) {
						time_limit = prev_ptr->Activity () + plan_ptr->Depart () - time_limit;
						if (time_limit < 0) {
							exe->Set_Problem (DURATION_PROBLEM);
							time_limit = 0;
						}
						prev_ptr->Activity (time_limit);
					}
				}
			}
		}
	}
	return (true);
}
