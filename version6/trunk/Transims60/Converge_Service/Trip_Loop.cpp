//*********************************************************
//	Trip_Loop.cpp - process each trip
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Trip_Loop
//---------------------------------------------------------

void Converge_Service::Trip_Loop (bool message_flag)
{
	int last_hhold, last_period, period, num_car, mode;

	Trip_Map_Itr map_itr;
	Plan_Ptr plan_ptr;
	Plan_Ptr_Array *ptr_array;

	last_hhold = -1;
	num_car = 0;

	if (message_flag) {
		Show_Message ("Processing Trip Record");
		Set_Progress ();
	}
	ptr_array = new Plan_Ptr_Array ();

	//---- process each trip ----

	if (time_order_flag) {
		last_period = -1;

		for (time_itr = plan_time_map.begin ();;) {
			Show_Progress ();

			Time_Index index = time_itr->first;

			//---- check constraint output increments ----

			period = constraint_periods.Period (index.Start ());

			if (period >= 0) {
				if (last_period >= 0 && period != last_period) {
					if (cap_const_flag) {
						Write_Constraint (last_period);
					}
					if (fuel_const_flag) {
						Write_Fuel_Demand (last_period);
					}
				}
				last_period = period;
			}

			//---- retrieve the plan data ----

			plan_ptr = new Plan_Data ();
			ptr_array = new Plan_Ptr_Array ();

			*plan_ptr = plan_array [time_itr->second];

			//---- path processing method ----

			Set_Method (*plan_ptr);

			//---- destination choice traveler ----

			if (choice_flag && plan_ptr->Method () == BUILD_PATH && choice_range.In_Range (plan_ptr->Type ())) {
				Best_Destination (*plan_ptr);
			}

			//---- build the path ----

			ptr_array->push_back (plan_ptr);

			part_processor.Plan_Build (ptr_array);

			//---- update the link travel times ----

			if (Time_Updates () && update_rate > 0) {
				mode = plan_ptr->Mode ();
				if (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
					mode != TRANSIT_MODE && mode != RIDE_MODE && mode != OTHER_MODE) {

					if (!(++num_car % update_rate)) {
						part_processor.Save_Flows ();
						Update_Travel_Times (MPI_Size (), reroute_time);
						num_time_updates++;
					}
				}
			}
MAIN_LOCK
			if (++time_itr == plan_time_map.end ()) break;
END_LOCK
		}
	} else {

		for (map_itr = plan_trip_map.begin (); map_itr != plan_trip_map.end (); map_itr++) {
			Show_Progress ();

			plan_ptr = new Plan_Data ();

			*plan_ptr = plan_array [map_itr->second];

			if (plan_ptr->Household () < 1) continue;

			if (plan_ptr->Household () != last_hhold) {
				if (last_hhold > 0 && ptr_array->size () > 0) {
					part_processor.Plan_Build (ptr_array);
					ptr_array = new Plan_Ptr_Array ();
				}
				last_hhold = plan_ptr->Household ();
			}
			Set_Method (*plan_ptr);

			ptr_array->push_back (plan_ptr);

			//---- update the link travel times ----

			if (Time_Updates () && update_rate > 0) {
				mode = plan_ptr->Mode ();
				if (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
					mode != TRANSIT_MODE && mode != RIDE_MODE && mode != OTHER_MODE) {

					if (!(++num_car % update_rate)) {
						part_processor.Save_Flows ();
						Update_Travel_Times (MPI_Size (), reroute_time);
						num_time_updates++;
					}
				}
			}
		}

		//---- process the last household ----

		if (last_hhold > 0 && ptr_array->size () > 0) {
			part_processor.Plan_Build (ptr_array);
		} else {
			delete ptr_array;
		}
	}
	if (message_flag) {
		End_Progress (max_speed_updates == 0);
	}
}
