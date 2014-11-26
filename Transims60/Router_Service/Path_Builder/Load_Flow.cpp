//*********************************************************
//	Load_Flow - update link/turn flows
//*********************************************************

#include "Path_Builder.hpp"

//---------------------------------------------------------
//	Load_Flow
//---------------------------------------------------------

Dtime Path_Builder::Load_Flow (Plan_Data *plan_data)
{
	int mode, index, dir_index, use_index;
	Dtime time, ttime, perf_time, tt;
	double len, len_factor;

	Plan_Leg_Itr leg_itr;
	Perf_Period *perf_period_ptr;
	Turn_Period *turn_period_ptr;
	Turn_Data *turn_ptr;
	Link_Data *link_ptr = 0;
	Dir_Data *dir_ptr = 0;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;

	if (plan_data == 0) {
		cout << "\tPlan Pointer is Zero" << endl;
		return (0);
	}
	if (!initialized) {
		cout << "\tPath Building Requires TRANSIMS Router Services" << endl;
		return (0);
	}
	plan_flag = true;
	plan_ptr = plan_data;

	mode = plan_ptr->Mode ();
		
	if (mode == WAIT_MODE || mode == TRANSIT_MODE || mode == WALK_MODE || mode == BIKE_MODE || 
		mode == RIDE_MODE || mode == OTHER_MODE) return (true);

	//---- set the traveler parameters ----

	exe->Set_Parameters (path_param, plan_ptr->Type (), plan_ptr->Veh_Type ());

	path_param.mode = (Mode_Type) mode;

	//---- trace the path ----

	perf_time = 0;
	time = plan_ptr->Depart ();
	dir_index = -1;

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++, time += ttime) {
		ttime = leg_itr->Time ();
		if (ttime < 0) plan_ptr->Problem (PATH_PROBLEM);
			
		if (leg_itr->Mode () != DRIVE_MODE) {
			perf_time += ttime;
			continue;
		}

		perf_period_ptr = perf_period_array_ptr->Period_Ptr (time);
		if (perf_period_ptr == 0) {
			perf_time += ttime;
			continue;
		}
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
		tt = ttime;

		if (use_index >= 0) {
			if (path_param.flow_flag) {
				len = leg_itr->Length ();
				if (len >= link_ptr->Length ()) {
					len = link_ptr->Length ();
					len_factor = 1.0;
				} else {
					len_factor = len / link_ptr->Length ();
				}
				tt = perf_period_array_ptr->Load_Flow (use_index, time, ttime, len_factor, link_ptr->Length (), path_param.pce, path_param.occupancy);
				if (tt < 0) {
					plan_ptr->Problem (PATH_PROBLEM);
					break;
				}
			}
			if (dir_index >= 0 && path_param.turn_flow_flag) {
				map2_itr = exe->connect_map.find (Int2_Key (dir_index, index));
				if (map2_itr != exe->connect_map.end ()) {
					turn_period_ptr = exe->turn_period_array.Period_Ptr (time);

					if (turn_period_ptr != 0) {
						turn_ptr = turn_period_ptr->Data_Ptr (map2_itr->second);
						turn_ptr->Add_Turn (path_param.pce);
						tt += turn_ptr->Time ();
					}
				}
			}
			dir_index = index;
		}
		if (tt < ttime) {
			ttime = tt;
			leg_itr->Time (ttime);
		}
		perf_time += tt;
	}
	ttime = plan_ptr->Arrive () - plan_ptr->Depart ();

	//---- adjust shorter travel times ----

	if (perf_time < ttime) {
		ttime = perf_time;
		if (plan_ptr->Constraint () != END_TIME) {
			plan_ptr->Arrive (plan_ptr->Depart () + ttime.Round_Seconds ());
			if (plan_ptr->Arrive () > exe->Model_End_Time ()) {
				plan_ptr->Arrive (exe->Model_End_Time ());
			}
		} else {
			plan_ptr->Depart (plan_ptr->Arrive () - ttime.Round_Seconds ());
			if (plan_ptr->Depart () < exe->Model_Start_Time ()) {
				plan_ptr->Depart (exe->Model_Start_Time ());
			}
		}
	}
	return (perf_time);
}
