//*********************************************************
//	Sim_Network_Update.cpp - Network Use Update Class
//*********************************************************

#include "Sim_Network_Update.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Network_Update constructor
//---------------------------------------------------------

Sim_Network_Update::Sim_Network_Update () : Sim_Update_Data ()
{
	Type (SIM_NETWORK_UPDATE);
	
	use_update_time = turn_update_time = 0;
}

//---------------------------------------------------------
//	Update_Check
//---------------------------------------------------------

bool Sim_Network_Update::Update_Check (void)
{
	if (use_update_time > sim->time_step && turn_update_time > sim->time_step) return (false);

	int i, dir, index, first_index, min_lane, max_lane, num, lanes;
	bool complex, match, use_update_flag, turn_update_flag;
	double capacity, cap_factor, pockets;

	Lane_Data lane_data, *lane_ptr;
	Lane_Array lane_array;
	Lane_Use_Data *use_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *use_index;
	Sim_Dir_Itr sim_dir_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Turn_Pen_Data *turn_ptr;

	use_update_flag = (use_update_time <= sim->time_step);
	if (use_update_flag) use_update_time = MAX_INTEGER;

	turn_update_flag = (turn_update_time <= sim->time_step);
	if (turn_update_flag) turn_update_time = MAX_INTEGER;
		
	cap_factor = sim->method_time_step [MACROSCOPIC];
	if (cap_factor <= 0.0) cap_factor = Dtime (6, SECONDS);
	cap_factor = sim->param.cap_factor * cap_factor / Dtime (1, HOURS);

	//---- initialize link dir data ----

	for (dir=0, sim_dir_itr = sim->sim_dir_array.begin (); sim_dir_itr != sim->sim_dir_array.end (); sim_dir_itr++, dir++) {
		dir_ptr = &sim->dir_array [dir];

		lanes = dir_ptr->Lanes ();

		if (!use_update_flag || dir_ptr->First_Lane_Use () < 0) goto exit_use;

		link_ptr = &sim->link_array [dir_ptr->Link ()];

		sim_dir_itr->Clear_Use (link_ptr->Use ());
		first_index = dir_ptr->First_Lane_Use ();

		//---- find the time period ----

		for (period_ptr = &sim->use_period_array [first_index]; ; period_ptr = &sim->use_period_array [++first_index]) {
			if (period_ptr->Start () > sim->time_step) {
				if (period_ptr->Start () < use_update_time) {
					use_update_time = period_ptr->Start ();
				}
			} else if (sim->time_step < period_ptr->End ()) {
				if (period_ptr->End () < use_update_time) {
					use_update_time = period_ptr->End ();
				}
				break;
			}
			if (period_ptr->Periods () == 0) goto exit_use;
		}
		num = period_ptr->Records ();
		index = period_ptr->Index ();

		//---- initialize the default lane use ----

		lane_data.Clear (link_ptr->Use ());

		lane_array.assign (sim_dir_itr->Lanes (), lane_data);

		min_lane = dir_ptr->Left ();
		max_lane = dir_ptr->Lanes () + min_lane - 1;

		//---- set the lane use restrictions ----

		for (i=0; i < num; i++, index++) {
			use_index = &sim->use_period_index [index];
			use_ptr = &sim->lane_use_array [use_index->Link ()];

			//---- must be a full length restriction ----

			if (use_ptr->Offset () > 0 || use_ptr->Length () > 0 || use_ptr->Type () == APPLY) {
				for (i=use_ptr->Low_Lane (); i <= use_ptr->High_Lane (); i++) {
					lane_ptr = &lane_array [i];

					if (lane_ptr->Index () < 0) {
						lane_ptr->Index (first_index);
					}
					lane_ptr->Complex (1);
				}
			} else if (use_ptr->Type () == REQUIRE) {
				for (i=0; i < sim_dir_itr->Lanes (); i++) {
					if (i < use_ptr->Low_Lane () || i > use_ptr->High_Lane ()) {
						lane_ptr = &lane_array [i];
						if (lane_ptr->Index () >= 0) {
							lane_ptr->Complex (1);
							continue;
						}
						lane_ptr->Index (first_index);
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
					lane_ptr->Index (first_index);

					if (use_ptr->Type () == PROHIBIT || use_ptr->Type () == LIMIT) {
						if (use_ptr->Type () == PROHIBIT) {
							if (use_ptr->Use () == ANY_USE_CODE) {
								lane_ptr->Use (0);
							} else {
								lane_ptr->Use (lane_ptr->Use () ^ use_ptr->Use ());
							}
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
		lanes = 0;

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
				if (sim_dir_itr->First_Use () < 0 || index < sim_dir_itr->First_Use ()) {
					sim_dir_itr->First_Use (index);
				}
			}
			if (lane_ptr->Use ()) {
				lanes++;
			}
		}
		if (match) {
			sim_dir_itr->Use_Type (lane_data.Type ());
			sim_dir_itr->Use (lane_data.Use ());
			sim_dir_itr->Min_Veh_Type (lane_data.Min_Veh_Type ());
			sim_dir_itr->Max_Veh_Type (lane_data.Max_Veh_Type ());
			sim_dir_itr->Min_Traveler (lane_data.Min_Traveler ());
			sim_dir_itr->Max_Traveler (lane_data.Max_Traveler ());

			if (!complex) sim_dir_itr->First_Use (-1);
		}

exit_use:

		//---- macroscopic flow rate -----

		if (lanes < dir_ptr->Lanes ()) {
			pockets = (dir_ptr->Left () + dir_ptr->Right ()) * 0.5;
			capacity = (double) dir_ptr->Capacity () * (lanes + pockets) / (dir_ptr->Lanes () + pockets);
		} else {
			capacity = dir_ptr->Capacity ();
		}
		sim_dir_itr->Max_Flow (Round (capacity * cap_factor));

		if (turn_update_flag) {

			//---- set the turn prohibition flag ----

			sim_dir_itr->Turn (false);

			for (index = dir_ptr->First_Turn (); index >= 0; index = turn_ptr->Next_Index ()) {
				turn_ptr = &sim->turn_pen_array [index];

				if (turn_ptr->Penalty () != 0) continue;

				if (turn_ptr->Start () > sim->time_step) {
					if (turn_ptr->Start () < turn_update_time) {
						turn_update_time = turn_ptr->Start ();
					}
				} else if (sim->time_step < turn_ptr->End ()) {
					if (turn_ptr->End () < turn_update_time) {
						turn_update_time = turn_ptr->End ();
					}
					sim_dir_itr->Turn (true);
					break;
				}
			}
		}
	}
	return (true);
}

