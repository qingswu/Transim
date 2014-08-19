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
	Trip_Data *trip_ptr;

	Plan_Ptr_Array *ptr_array;
	Plan_Data *new_plan_ptr, *plan_ptr;
	Path_Parameters param;
	Trip_Index trip_index;
	//Gap_Data *gap_ptr;
	
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

	if (Master ()) {
		Show_Message ("Processing Trip Record");
		Set_Progress ();
	}
	last_hhold = 0;
	build_flag = true;

	for (map_itr = plan_trip_map.begin (); map_itr != plan_trip_map.end (); map_itr++) {
		Show_Progress ();

		plan_ptr = &plan_array [map_itr->second];

		//---- check the household id ----

		hhold = plan_ptr->Household ();
		if (hhold < 1) continue;

		if (hhold > max_hhold) break;

		plan_ptr->Get_Index (trip_index);
		trip_ptr = 0;

		//---- check the priority codes ----

		if (select_priorities) {
			if (first_iteration) {
				trip_itr = trip_map.find (trip_index);

				if (trip_itr != trip_map.end ()) {
					trip_ptr = &trip_array [trip_itr->second];
					plan_ptr->Priority (trip_ptr->Priority ());
				}
			}
			build_flag = select_priority [plan_ptr->Priority ()];
		}

		if (hhold != last_hhold) {
			if (last_hhold > 0 && ptr_array->size () > 0) {
				part_processor.Plan_Build (ptr_array, part, plan_process_ptr);
				ptr_array = new Plan_Ptr_Array ();
			}
			last_hhold = hhold;
		}

		//---- determine the processing method ----

		if (build_flag || plan_ptr->size () == 0 || plan_ptr->Problem () > 0) {
			plan_ptr->Method (BUILD_PATH);
			plan_ptr->Depart (plan_ptr->Start ());
			plan_ptr->Arrive (plan_ptr->End ());
			plan_ptr->Activity (plan_ptr->Duration ());
		} else if (reroute_flag) {
			if (plan_ptr->Depart () < reroute_time && plan_ptr->Arrive () > reroute_time) {
				plan_ptr->Method (REROUTE_PATH);

				//---- update the trip data ----

				if (first_iteration && trip_ptr == 0) {
					trip_itr = trip_map.find (trip_index);

					if (trip_itr != trip_map.end ()) {
						trip_ptr = &trip_array [trip_itr->second];

						plan_ptr->End (trip_ptr->End ());
						plan_ptr->Destination (trip_ptr->Destination ());
						plan_ptr->Type (trip_ptr->Type ());

					}
					plan_ptr->Constraint (START_TIME);
				}
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
	if (Master () && !thread_flag) End_Progress ();

	if (last_hhold > 0 && ptr_array->size () > 0) {
		part_processor.Plan_Build (ptr_array, part, plan_process_ptr);
	} else {
		delete ptr_array;
	}
	return (true);
}
