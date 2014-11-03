//*********************************************************
//	Memory_Loop.cpp - process trips and plans in memory
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Memory_Loop
//---------------------------------------------------------

bool Router::Memory_Loop (int part, Plan_Processor *plan_process_ptr)
{
	int num_car, max_hhold, hhold, mode, last_hhold;
	bool gap_flag, build_flag;

	Trip_Map_Itr trip_itr, map_itr;

	Plan_Ptr_Array *ptr_array;
	Plan_Data *new_plan_ptr, *plan_ptr;
	Path_Parameters param;
	
	Set_Parameters (param);

	num_car = 0;
	last_hhold = -1;
	max_hhold = MAX_INTEGER;

	gap_flag = ((trip_gap_map_flag || save_trip_gap) && first_iteration);

	ptr_array = new Plan_Ptr_Array ();

	if (select_households) {
		max_hhold = hhold_range.Max_Value ();
	}
	if (select_flag) {
		hhold = select_map.Max_Household ();
		if (hhold < max_hhold) max_hhold = hhold;
	}

	Show_Message ("Processing Trip Record");
	Set_Progress ();

	last_hhold = 0;
	build_flag = true;

	for (map_itr = plan_trip_map.begin (); map_itr != plan_trip_map.end (); map_itr++) {
		Show_Progress ();

		plan_ptr = &plan_array [map_itr->second];

		//---- check the household id ----

		hhold = plan_ptr->Household ();
		if (hhold < 1) continue;

		if (hhold != last_hhold) {
			if (last_hhold > 0 && ptr_array->size () > 0) {
				part_processor.Plan_Build (ptr_array, part, plan_process_ptr);
				ptr_array = new Plan_Ptr_Array ();
			}
			last_hhold = hhold;
		}
		if (hhold > max_hhold) break;

		//---- check the priority codes ----

		if (plan_ptr->Priority () == NO_PRIORITY) {
			plan_ptr->Method (UPDATE_PLAN);
		} else if (select_priorities) {
			build_flag = select_priority [plan_ptr->Priority ()];

			if (build_flag && max_percent_flag && percent_selected < 1.0) {
				double prob = random_select.Probability ();
				build_flag = (prob <= percent_selected);
			}

			if (build_flag || plan_ptr->size () == 0 || plan_ptr->Problem () > 0) {
				plan_ptr->Method (BUILD_PATH);
				plan_ptr->Depart (plan_ptr->Start ());
				plan_ptr->Arrive (plan_ptr->End ());
				plan_ptr->Activity (plan_ptr->Duration ());
			} else {
				plan_ptr->Method (UPDATE_PLAN);
			}
		} else if (plan_ptr->size () == 0 || plan_ptr->Problem () > 0) {
			plan_ptr->Method (BUILD_PATH);
			plan_ptr->Depart (plan_ptr->Start ());
			plan_ptr->Arrive (plan_ptr->End ());
			plan_ptr->Activity (plan_ptr->Duration ());
		} else if (reroute_flag) {
			if (plan_ptr->Depart () < reroute_time && plan_ptr->Arrive () > reroute_time) {
				plan_ptr->Method (REROUTE_PATH);
				plan_ptr->Constraint (START_TIME);
			} else if (plan_ptr->Depart () >= reroute_time) {
				plan_ptr->Method (BUILD_PATH);
			} else if (plan_ptr->Arrive () < reroute_time) {
				plan_ptr->Method (COPY_PLAN);
			}
		} else if (update_flag) {
			plan_ptr->Method (UPDATE_PLAN);
		} else if (Link_Flows ()) {
			plan_ptr->Method (PATH_FLOWS);
		} else {
			plan_ptr->Method (COPY_PLAN);
		}

		//---- save the plan data ----

		new_plan_ptr = new Plan_Data ();
		*new_plan_ptr = *plan_ptr;

		if (plan_ptr->Method () == REROUTE_PATH) {
			new_plan_ptr->Arrive (reroute_time);
		}
		ptr_array->push_back (new_plan_ptr);

		//---- update the link travel times ----

		if (Time_Updates () && update_rate > 0) {
			mode = plan_ptr->Mode ();
			if (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
				mode != TRANSIT_MODE && mode != RIDE_MODE && mode != OTHER_MODE) {

				if (!(++num_car % update_rate)) {
					plan_process_ptr->Save_Flows ();
					Update_Travel_Times (MPI_Size (), reroute_time);
					num_time_updates++;
				}
			}
		}
	}
	if (!thread_flag) End_Progress ();

	if (last_hhold > 0 && ptr_array->size () > 0) {
		part_processor.Plan_Build (ptr_array, part, plan_process_ptr);
	} else {
		delete ptr_array;
	}
	return (true);
}
