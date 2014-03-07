//*********************************************************
//	Set_Lane_Use.cpp - set current lane use restrictions
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Set_Lane_Use
//---------------------------------------------------------

void Simulator_Service::Set_Lane_Use (Sim_Dir_Data *sim_dir_ptr, Dir_Data *dir_ptr, int use)
{
	int i, index, min_lane, max_lane, num;
	bool complex, match;

	Lane_Data lane_data, *lane_ptr;
	Lane_Array lane_array;
	Lane_Use_Data *use_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *use_index;

	sim_dir_ptr->Clear_Use (use);

	index = dir_ptr->First_Lane_Use ();

	//---- find the time period ----

	for (period_ptr = &use_period_array [index]; ; period_ptr = &use_period_array [++index]) {
		if (period_ptr->Start () > time_step) {
			if (period_ptr->Start () < use_update_time) {
				use_update_time = period_ptr->Start ();
			}
		} else if (time_step < period_ptr->End ()) {
			if (period_ptr->End () < use_update_time) {
				use_update_time = period_ptr->End ();
			}
			break;
		}
		if (period_ptr->Periods () == 0) return;
	}
	num = period_ptr->Records ();
	index = period_ptr->Index ();

	//---- initialize the default lane use ----

	lane_data.Use (use);

	lane_array.assign (dir_ptr->Lanes (), lane_data);

	min_lane = dir_ptr->Left ();
	max_lane = dir_ptr->Lanes () + min_lane - 1;

	//---- set the lane use restrictions ----

	for (i=0; i < num; i++, index++) {
		use_index = &use_period_index [index];
		use_ptr = &lane_use_array [use_index->Link ()];

		//---- must be a full length restriction ----
;
		if (use_ptr->Offset () > 0 || use_ptr->Length () > 0 || use_ptr->Type () == APPLY) {
			for (i=use_ptr->Low_Lane (); i <= use_ptr->High_Lane (); i++) {
				lane_ptr = &lane_array [i];

				if (lane_ptr->Index () < 0) {
					lane_ptr->Index (index);
				}
				lane_ptr->Complex (1);
			}
		} else if (use_ptr->Type () == REQUIRE) {
			for (i=0; i < sim_dir_ptr->Lanes (); i++) {
				if (i < use_ptr->Low_Lane () || i > use_ptr->High_Lane ()) {
					lane_ptr = &lane_array [i];
					if (lane_ptr->Index () >= 0) {
						lane_ptr->Complex (1);
						continue;
					}
					lane_ptr->Index (index);
					lane_ptr->Use (lane_ptr->Use () ^ use_ptr->Use ());
					lane_ptr->Type (LIMIT);
					if (use_ptr->Min_Veh_Type () >= 0) {
						lane_ptr->Min_Veh_Type (use_ptr->Min_Veh_Type ());
						lane_ptr->Max_Veh_Type (use_ptr->Max_Veh_Type ());
					}
					if (use_ptr->Min_Traveler () > 0) {
						lane_ptr->Min_Traveler (use_ptr->Min_Traveler ());
						lane_ptr->Max_Traveler (use_ptr->Max_Traveler ());
					}
				}
			}
		} else {
			for (i=use_ptr->Low_Lane (); i <= use_ptr->High_Lane (); i++) {
				lane_ptr = &lane_array [i];
				if (lane_ptr->Index () >= 0) {
					lane_ptr->Complex (1);
					continue;
				}
				lane_ptr->Index (index);

				if (use_ptr->Type () == PROHIBIT || use_ptr->Type () == LIMIT) {
					if (use_ptr->Type () == PROHIBIT) {
						lane_ptr->Use (lane_ptr->Use () ^ use_ptr->Use ());
					} else {
						lane_ptr->Use (use_ptr->Use ());
					}
					lane_ptr->Type (use_ptr->Type ());
					if (use_ptr->Min_Veh_Type () >= 0) {
						lane_ptr->Min_Veh_Type (use_ptr->Min_Veh_Type ());
						lane_ptr->Max_Veh_Type (use_ptr->Max_Veh_Type ());
					}
					if (use_ptr->Min_Traveler () > 0) {
						lane_ptr->Min_Traveler (use_ptr->Min_Traveler ());
						lane_ptr->Max_Traveler (use_ptr->Max_Traveler ());
					}
				}
			}
		}
	}

	//---- check the results ----

	complex = false;
	match = true;

	for (i = min_lane; i <= max_lane; i++) {
		lane_ptr = &lane_array [i];

		if (lane_ptr->Complex () == 1) complex = true;

		if (i == min_lane) {
			lane_data = *lane_ptr;
		} else {
			if (memcmp (&lane_data, lane_ptr, sizeof (lane_data))) {
				match = false;
			}
		}
		index = lane_ptr->Index ();

		if (index >= 0) {
			if (sim_dir_ptr->First_Use () < 0 || index < sim_dir_ptr->First_Use ()) {
				sim_dir_ptr->First_Use (index);
			}
		}
	}
	if (match) {
		sim_dir_ptr->Use_Type (lane_data.Type ());
		sim_dir_ptr->Use (lane_data.Use ());
		sim_dir_ptr->Min_Veh_Type (lane_data.Min_Veh_Type ());
		sim_dir_ptr->Max_Veh_Type (lane_data.Max_Veh_Type ());
		sim_dir_ptr->Min_Traveler (lane_data.Min_Traveler ());
		sim_dir_ptr->Max_Traveler (lane_data.Max_Traveler ());

		if (!complex) sim_dir_ptr->First_Use (-1);
	}
}
