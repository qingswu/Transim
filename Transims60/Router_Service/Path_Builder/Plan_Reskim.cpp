//*********************************************************
//	Plan_Reskim - adjust times and flows
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Plan_Reskim
//---------------------------------------------------------

bool Path_Builder::Plan_Reskim (Plan_Ptr plan_ptr)
{
	int mode, index, dir_index, use_index, imped, cum_imp;
	Dtime time, ttime, tod, old_tod, drive, diff;
	double len, len_factor, time_diff;

	Plan_Leg_Itr leg_itr;
	Plan_Leg_RItr leg_ritr;
	Turn_Period *turn_period_ptr;
	Turn_Data *turn_ptr;
	Link_Data *link_ptr = 0;
	Dir_Data *dir_ptr = 0;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;

	mode = plan_ptr->Mode ();

	if (mode == WAIT_MODE || mode == TRANSIT_MODE || mode == WALK_MODE || mode == BIKE_MODE || 
		mode == RIDE_MODE || mode == OTHER_MODE) return (true);

	//---- set the traveler parameters ----
	
	exe->Set_Parameters (param, plan_ptr->Type (), plan_ptr->Veh_Type ());

	time = drive = 0;
	dir_index = -1;
	cum_imp = imped = 0;

	if (plan_ptr->Constraint () != END_TIME) {

		//---- start time constrained path ----

		tod = plan_ptr->Depart ();

		for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++, tod += time, cum_imp += imped) {
			time = leg_itr->Time ();
			imped = leg_itr->Impedance ();

			if (leg_itr->Mode () != DRIVE_MODE) continue;

			if (leg_itr->Link_Type ()) {
				index = leg_itr->Link_ID ();

				link_ptr = &exe->link_array [index];

				if (leg_itr->Link_Dir ()) {
					index = link_ptr->BA_Dir ();
				} else {
					index = link_ptr->AB_Dir ();
				}
				use_index = index; 

				if (leg_itr->Use_Type ()) {
					dir_ptr = &exe->dir_array [index];
					if (dir_ptr->Use_Index () >= 0) {
						use_index = dir_ptr->Use_Index ();
					}
				}
			} else if (leg_itr->Dir_Type ()) {
				use_index = index = leg_itr->ID ();

				dir_ptr = &exe->dir_array [index];
				link_ptr = &exe->link_array [dir_ptr->Link ()];

				if (leg_itr->Type () == USE_ID) {
					if (dir_ptr->Use_Index () >= 0) {
						use_index = dir_ptr->Use_Index ();
					}
				}
			} else {
				use_index = index = -1;
			}
			if (use_index >= 0 && leg_itr->Length () > 0) {
				len = leg_itr->Length ();
				if (len >= link_ptr->Length ()) {
					len = link_ptr->Length ();
					len_factor = 1.0;
				} else {
					len_factor = len / link_ptr->Length ();
				}
				time = perf_period_array_ptr->Flow_Time (index, tod, len_factor, link_ptr->Length (), param.pce, param.occupancy);

				diff = time - leg_itr->Time ();
				leg_itr->Time (time);
				imped += Resolve (diff * param.value_time);
				leg_itr->Impedance (imped);

				if (dir_index >= 0 && turn_flag) {
					map2_itr = exe->connect_map.find (Int2_Key (dir_index, index));
					if (map2_itr != exe->connect_map.end ()) {
						turn_period_ptr = turn_period_array_ptr->Period_Ptr (tod);

						if (turn_period_ptr != 0) {
							turn_ptr = turn_period_ptr->Data_Ptr (map2_itr->second);
							turn_ptr->Add_Turn (param.pce);
						}
					}
				}
				dir_index = index;
			}
			drive += time;
		}
		time = tod.Round_Seconds ();
		old_tod = plan_ptr->Arrive ();
		plan_ptr->Arrive (time);

	} else {

		//---- end time constrained path ----
			
		tod = plan_ptr->Arrive ();

		for (leg_ritr = plan_ptr->rbegin (); leg_ritr != plan_ptr->rend (); leg_ritr++, tod -= time, cum_imp += imped) {
			time = leg_ritr->Time ();
			imped = leg_ritr->Impedance ();

			if (leg_ritr->Mode () != DRIVE_MODE) continue;

			if (leg_ritr->Link_Type ()) {
				index = leg_ritr->Link_ID ();

				link_ptr = &exe->link_array [index];

				if (leg_ritr->Link_Dir ()) {
					index = link_ptr->BA_Dir ();
				} else {
					index = link_ptr->AB_Dir ();
				}
				use_index = index; 

				if (leg_ritr->Use_Type ()) {
					dir_ptr = &exe->dir_array [index];
					if (dir_ptr->Use_Index () >= 0) {
						use_index = dir_ptr->Use_Index ();
					}
				}
			} else if (leg_ritr->Dir_Type ()) {
				use_index = index = leg_ritr->ID ();

				dir_ptr = &exe->dir_array [index];
				link_ptr = &exe->link_array [dir_ptr->Link ()];

				if (leg_ritr->Type () == USE_ID) {
					if (dir_ptr->Use_Index () >= 0) {
						use_index = dir_ptr->Use_Index ();
					}
				}
			} else {
				use_index = index = -1;
			}
			if (use_index >= 0 && leg_ritr->Length () > 0) {
				len = leg_ritr->Length ();
				if (len >= link_ptr->Length ()) {
					len = link_ptr->Length ();
					len_factor = 1.0;
				} else {
					len_factor = len / link_ptr->Length ();
				}
				time = perf_period_array_ptr->Flow_Time (index, tod, len_factor, link_ptr->Length (), param.pce, param.occupancy, false);

				diff = time - leg_ritr->Time ();
				leg_ritr->Time (time);
				imped += Resolve (diff * param.value_time);
				leg_ritr->Impedance (imped);

				if (dir_index >= 0 && turn_flag) {
					map2_itr = exe->connect_map.find (Int2_Key (index, dir_index));
					if (map2_itr != exe->connect_map.end ()) {
						turn_period_ptr = turn_period_array_ptr->Period_Ptr (tod);

						if (turn_period_ptr != 0) {
							turn_ptr = turn_period_ptr->Data_Ptr (map2_itr->second);
							turn_ptr->Add_Turn (param.pce);
						}
					}
				}
				dir_index = index;
			}
			drive += time;
		}
		time = tod.Round_Seconds ();
		if (time < 0) time = 0;
		old_tod = plan_ptr->Depart ();
		plan_ptr->Depart (time);
	}
	time_diff = abs (old_tod - time);
	skim_gap += time_diff;
	skim_time += old_tod;

	plan_ptr->Drive (drive);
	plan_ptr->Impedance (cum_imp);

	if (exe->save_trip_gap) {
		Gap_Data *gap_ptr;

		gap_ptr = &exe->gap_data_array [plan_ptr->Index ()];
		gap_ptr->current = cum_imp;
		gap_ptr->time = time;
	}
	return (true);
}
