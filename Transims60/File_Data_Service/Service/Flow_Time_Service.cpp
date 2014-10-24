//*********************************************************
//	Flow_Time_Service.cpp - flow time service keys and data
//*********************************************************

#include "Flow_Time_Service.hpp"

//---------------------------------------------------------
//	Flow_Time_Service constructor
//---------------------------------------------------------

Flow_Time_Service::Flow_Time_Service (void)
{
	flow_updates = turn_updates = time_updates = false;
	first_update = true;
	update_rate = 0;
	flow_factor = 1.0;
}

//---------------------------------------------------------
//	Flow_Time_Service_Keys
//---------------------------------------------------------

void Flow_Time_Service::Flow_Time_Service_Keys (int *keys)
{
	Control_Key control_keys [] = { //--- code, key, level, status, type, default, range, help ----
		{ UPDATE_FLOW_RATES, "UPDATE_FLOW_RATES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_TURNING_MOVEMENTS, "UPDATE_TURNING_MOVEMENTS", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ CLEAR_INPUT_FLOW_RATES, "CLEAR_INPUT_FLOW_RATES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ UPDATE_TRAVEL_TIMES, "UPDATE_TRAVEL_TIMES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
		{ TIME_UPDATE_RATE, "TIME_UPDATE_RATE", LEVEL0, OPT_KEY, INT_KEY, "0", "-1..5000", NO_HELP },
		{ LINK_FLOW_FACTOR, "LINK_FLOW_FACTOR", LEVEL0, OPT_KEY, FLOAT_KEY, "1.0", "1..100000", NO_HELP },
		{ EQUATION_PARAMETERS, "EQUATION_PARAMETERS", LEVEL1, OPT_KEY, LIST_KEY, "BPR, 0.15, 4.0, 0.75" , EQUATION_RANGE, NO_HELP},
		END_CONTROL
	};

	if (keys == 0) {
		dat->Key_List (control_keys);
	} else {
		int i, j;

		for (i=0; keys [i] != 0; i++) {
			for (j=0; control_keys [j].code != 0; j++) {
				if (control_keys [j].code == keys [i]) {
					dat->Add_Control_Key (control_keys [j]);
					break;
				}
			}
			if (control_keys [j].code == 0) {
				dat->Error (String ("Flow-Time Service Key %d was Not Found") % keys [i]);
			}
		}
	}
}

//---------------------------------------------------------
//	Read_Flow_Time_Keys
//---------------------------------------------------------

void Flow_Time_Service::Read_Flow_Time_Keys (void)
{
	String key;
	
	dat->Print (2, "Flow-Time Service Controls:");

	//---- update flow rates ----

	if (dat->Control_Key_Status (UPDATE_FLOW_RATES)) {
		Flow_Updates (dat->Get_Control_Flag (UPDATE_FLOW_RATES));
	}

	if (Flow_Updates ()) {

		//---- update turning movements ----

		if (dat->Control_Key_Status (UPDATE_TURNING_MOVEMENTS)) {
			if (dat->System_File_Flag (CONNECTION)) {
				Turn_Updates (dat->Get_Control_Flag (UPDATE_TURNING_MOVEMENTS));
			}
		}

		//---- clear input link flows ----

		if (dat->Control_Key_Status (CLEAR_INPUT_FLOW_RATES) && dat->System_File_Flag (PERFORMANCE)) {
			dat->Clear_Flow_Flag (dat->Get_Control_Flag (CLEAR_INPUT_FLOW_RATES));
		}
	}

	//---- update travel times ----

	if (dat->Control_Key_Status (UPDATE_TRAVEL_TIMES)) {
		dat->Print (1);
		Time_Updates (dat->Get_Control_Flag (UPDATE_TRAVEL_TIMES));
	}

	if (Time_Updates ()) {

		//---- time updates and volume factor ----

		if (dat->Control_Key_Status (TIME_UPDATE_RATE)) {
			update_rate = dat->Get_Control_Integer (TIME_UPDATE_RATE);

			if (update_rate == 0) {
				update_rate = -1;
				Flow_Updates (true);
			}
		}
		
		//---- link flow factor ----

		if (dat->Control_Key_Status (LINK_FLOW_FACTOR)) {
			flow_factor = dat->Get_Control_Double (LINK_FLOW_FACTOR);
		}

		if (dat->Control_Key_Status (EQUATION_PARAMETERS)) {
			dat->Print (1);

			//---- equation parameters ----

			for (int i=1; i <= EXTERNAL; i++) {
				key = dat->Get_Control_String (EQUATION_PARAMETERS, i);

				equation.Add_Equation (i, key);
			}
		}
	}
}

//---------------------------------------------------------
//	Build_Perf_Arrays
//---------------------------------------------------------

void Flow_Time_Service::Build_Perf_Arrays (Perf_Period_Array &perf_array)
{
	if (perf_array.size () != 0) return;

	perf_array.Initialize (&dat->time_periods);

	perf_array.Set_Time0 ();
}

//---------------------------------------------------------
//	Build_Turn_Arrays
//---------------------------------------------------------

void Flow_Time_Service::Build_Turn_Arrays (Turn_Period_Array &turn_delay)
{
	if (turn_delay.size () != 0 || !Turn_Updates ()) return;

	turn_delay.Initialize (&dat->time_periods, (int) dat->connect_array.size ());
}

//---------------------------------------------------------
//	Update_Travel_Times
//---------------------------------------------------------

void Flow_Time_Service::Update_Travel_Times (int mpi_size, Dtime first_time, bool avg_times, bool zero_flows)
{
	int i, num, nrec, type, lanes, lanes0, lanes1, cap, capacity, tod_cap, max_cap, len, index, rec, use_index;
	Dtime time0, time, time1, tod1, tod, period;
	double volume, vol_fac, length, speed;

	Dir_Itr dir_itr;
	Link_Data *link_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *index_ptr;
	Lane_Use_Data *use_ptr;
	Perf_Period_Itr per_itr;
	Perf_Data *perf_ptr, *perf1_ptr;

	period = dat->time_periods.Increment ();
	if (period < 1) period = 1;

	tod1 = dat->Model_Start_Time () + period / 2;
	vol_fac = flow_factor * Dtime (60, MINUTES) / period;
	if (mpi_size > 1) vol_fac *= mpi_size;

	for (index=0, dir_itr = dat->dir_array.begin (); dir_itr != dat->dir_array.end (); dir_itr++, index++) {
		link_ptr = &dat->link_array [dir_itr->Link ()];

		len = link_ptr->Length ();
		length = exe->UnRound (len);

		type = link_ptr->Type ();

		time0 = dir_itr->Time0 ();
		capacity = dir_itr->Capacity ();
		lanes = dir_itr->Lanes ();
		if (lanes < 1) lanes = 1;
		tod = tod1;

		for (per_itr = dat->perf_period_array.begin (); per_itr != dat->perf_period_array.end (); per_itr++, tod += period) {
			if (tod < first_time) continue;

			//---- check for time of day restrictions ----

			time = time0;
			cap = capacity;
			lanes0 = lanes;
			lanes1 = 0;
			rec = dir_itr->First_Lane_Use ();

			if (rec >= 0) {
				for (;; rec++) {
					period_ptr = &dat->use_period_array [rec];

					if (period_ptr->Start () <= tod && tod < period_ptr->End ()) {
						lanes0 = period_ptr->Lanes0 ();
						lanes1 = period_ptr->Lanes1 ();

						nrec = period_ptr->Records ();
						num = period_ptr->Index ();

						for (i=0; i < nrec; i++, num++) {
							index_ptr = &dat->use_period_index [num];
							use_ptr = &dat->lane_use_array [index_ptr->Link ()];

							if (use_ptr->Speed () > 0) {
								time.Seconds ((double) len / use_ptr->Speed () + 0.09);
							} else if (use_ptr->Spd_Fac () != 100 && use_ptr->Spd_Fac () != 0) {
								time = time * 100.0 / use_ptr->Spd_Fac ();
							}
							if (use_ptr->Capacity () > 0) {
								cap = use_ptr->Capacity ();
							} else if (use_ptr->Cap_Fac () != 100 && use_ptr->Cap_Fac () != 0) {
								cap = dat->DTOI (cap * use_ptr->Cap_Fac () / 100.0);
							}
						}
						break;
					}
					if (period_ptr->Periods () == 0) break;
				}
			}

			//---- general purpose lanes ----

			perf_ptr = per_itr->Data_Ptr (index);

			volume = perf_ptr->Volume () * vol_fac;

			use_index = dir_itr->Use_Index ();

			if (use_index >= 0) {
				perf1_ptr = per_itr->Data_Ptr (use_index);

				if (!dat->Lane_Use_Flows ()) {
					volume += perf1_ptr->Volume () * vol_fac;
					use_index = -1;
				}
			} else {
				perf1_ptr = 0;
			}
			if (lanes0 < 1) {
				time1 = (int) (len / 0.1 + 0.5);
				if (time1 < 1) time1 = 1;
			} else if (volume == 0.0) {
				time1 = time;
			} else {
				if (lanes0 != lanes) {
					tod_cap = (cap * lanes0 + lanes0 / 2) / lanes;
				} else {
					tod_cap = cap;
				}
				if (avg_times) {
					max_cap = 2 * tod_cap;
					if (volume > max_cap) {
						volume = max_cap;
					}
				}
				time1 = equation.Apply_Equation (type, time, volume, tod_cap, len);
				if (time1 < 1) time1 = 1;
			}
			if (avg_times) {
				perf_ptr->Average_Time (time1);
			} else {
				perf_ptr->Time (time1);
			}
			if (zero_flows) {
				perf_ptr->Clear_Flows ();
			} else {
				time1 = perf_ptr->Time ();
				if (time1 < 1) time1 = 1;

				speed = length / time1.Seconds ();
				if (speed < 0.1) speed = 0.1;

				perf_ptr->Veh_Time (perf_ptr->Veh_Dist () / speed);
			}

			//---- managed lanes ----

			if (perf1_ptr > 0) {
				volume = perf1_ptr->Volume () * vol_fac;

				if (lanes1 == 0) {
					time1 = (int) (len / 0.1 + 0.5);
					if (time1 < 1) time1 = 1;
				} else if (volume == 0.0) {
					time1 = time;
				} else {
					tod_cap = (cap * lanes1 + lanes1 / 2) / lanes;

					if (avg_times) {
						max_cap = 2 * tod_cap;
						if (volume > max_cap) {
							volume = max_cap;
						}
					}
					time1 = equation.Apply_Equation (type, time, volume, tod_cap, len);
					if (time1 < 1) time1 = 1;
				}
			} else {
				time1 = perf_ptr->Time ();
			}
			if (perf1_ptr > 0) {
				if (avg_times) {
					perf1_ptr->Average_Time (time1);
				} else {
					perf1_ptr->Time (time1);
				}
				if (zero_flows) {
					perf1_ptr->Clear_Flows ();
				} else {
					time1 = perf1_ptr->Time ();
					if (time1 < 1) time1 = 1;

					speed = length / time1.Seconds ();
					if (speed < 0.1) speed = 0.1;

					perf1_ptr->Veh_Time (perf1_ptr->Veh_Dist () / speed);
				}
			}
		}
	}
	if (first_update) {
		first_update = false;

		if (turn_updates) {
			Turn_Period_Itr per_itr;
			Turn_Itr turn_itr;
			tod = tod1;

			for (per_itr = dat->turn_period_array.begin (); per_itr != dat->turn_period_array.end (); per_itr++, tod += period) {
				if (tod < first_time) continue;

				for (turn_itr = per_itr->begin (); turn_itr != per_itr->end (); turn_itr++) {
					turn_itr->Time (0);
				}
			}
		}
	}
}

