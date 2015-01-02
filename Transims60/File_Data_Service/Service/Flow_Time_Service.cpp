//*********************************************************
//	Flow_Time_Service.cpp - flow time service keys and data
//*********************************************************

#include "Flow_Time_Service.hpp"

#include <math.h>

//---------------------------------------------------------
//	Flow_Time_Service constructor
//---------------------------------------------------------

Flow_Time_Service::Flow_Time_Service (void)
{
<<<<<<< .working
	flow_updates = turn_updates = time_updates = false;
=======
	flow_updates = turn_updates = time_updates = average_times = false;
>>>>>>> .merge-right.r1529
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
		{ AVERAGE_TRAVEL_TIMES, "AVERAGE_TRAVEL_TIMES", LEVEL0, OPT_KEY, BOOL_KEY, "FALSE", BOOL_RANGE, NO_HELP },
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

		if (dat->Control_Key_Status (AVERAGE_TRAVEL_TIMES)) {
			dat->Print (1);
			Average_Times (dat->Get_Control_Flag (AVERAGE_TRAVEL_TIMES));
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
	int i, num, nrec, type, lanes, lanes0, lanes1, cap, capacity, tod_cap, len, index, rec, use_index;
	Dtime time0, time, tim, tim1, tod1, tod, period, prv, prv1, prev, prev1;
	double volume, volume1, vol_fac, length, speed, avg_tt;

	Dir_Itr dir_itr;
	Link_Data *link_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *index_ptr;
	Lane_Use_Data *use_ptr;
	Perf_Period_Itr per_itr;
	Perf_Data *perf_ptr, *perf1_ptr, *prev_ptr, *prev1_ptr;

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
		prev_ptr = prev1_ptr = 0;
		prv = prev = prv1 = prev1 = time0;
		perf1_ptr = 0;

		for (per_itr = dat->perf_period_array.begin (); per_itr != dat->perf_period_array.end (); per_itr++, tod += period) {
			if (tod < first_time) continue;

			//---- check for time of day restrictions ----

			time = time0;
			cap = capacity;
			lanes0 = lanes;
			lanes1 = 0;
			volume1 = 0;

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
							if (time < time0) time = time0;
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

			volume = perf_ptr->Enter () * vol_fac;

			if (lanes0 < 1) {
				tim = (int) (len / 0.1 + 0.5);
				if (tim < time0) tim = time0;
			} else if (volume == 0.0) {
				tim = time;
			} else {
				if (lanes0 != lanes) {
					tod_cap = (cap * lanes0 + lanes0 / 2) / lanes;
				} else {
					tod_cap = cap;
				}
				tim = equation.Apply_Equation (type, time, volume, tod_cap, len);
				if (tim < time0) tim = time0;
			}

			//---- managed lanes ----

			use_index = dir_itr->Use_Index ();

			if (use_index >= 0) {
				perf1_ptr = per_itr->Data_Ptr (use_index);

				volume1 = perf1_ptr->Enter () * vol_fac;

				if (lanes1 == 0) {
					tim1 = (int) (len / 0.1 + 0.5);
					if (tim1 < time0) tim1 = time0;
				} else if (volume1 == 0.0) {
					tim1 = time;
				} else {
					tod_cap = (cap * lanes1 + lanes1 / 2) / lanes;

					tim1 = equation.Apply_Equation (type, time, volume1, tod_cap, len);
					if (tim1 < time0) tim1 = time0;
				}
			}

			//---- save the results ----
			
			if (dat->Lane_Use_Flows () && use_index >= 0) {
				if (avg_times) {
					tim = (MAX (perf_ptr->Time (), time0) + tim) / 2;
					perf_ptr->Time (tim);

					if (prev_ptr > 0) {
						avg_tt = (prv + 2.0 * prev + tim) / 4.0;
						if (avg_tt >= MAX_INTEGER) avg_tt = MAX_INTEGER - 1;
						prev_ptr->Time ((int) avg_tt);
					}
					prev_ptr = perf_ptr;
					prv = prev;
					prev = tim;

					tim1 = (MAX (perf1_ptr->Time (), time0) + tim1) / 2;
					perf1_ptr->Time (tim1);

					if (prev1_ptr > 0) {
						avg_tt = (prv1 + 2.0 * prev1 + tim1) / 4.0;
						if (avg_tt >= MAX_INTEGER) avg_tt = MAX_INTEGER - 1;
						prev1_ptr->Time ((int) avg_tt);
					}
					prev1_ptr = perf1_ptr;
					prv1 = prev1;
					prev1 = tim1;

				} else {
					perf_ptr->Time (tim);
					perf1_ptr->Time (tim1);
				}
				if (zero_flows) {
					perf_ptr->Clear_Flows ();
					perf1_ptr->Clear_Flows ();
				} else {
					tim = perf_ptr->Time ();
					if (tim < time0) tim = time0;

					speed = length / tim.Seconds ();
					if (speed < 0.1) speed = 0.1;

					perf_ptr->Veh_Time (perf_ptr->Veh_Dist () / speed);

					tim1 = perf1_ptr->Time ();
					if (tim1 < time0) tim1 = time0;

					speed = length / tim1.Seconds ();
					if (speed < 0.1) speed = 0.1;

					perf1_ptr->Veh_Time (perf1_ptr->Veh_Dist () / speed);
				}
			} else {
				if (use_index >= 0 && volume1 > 0) {
					perf_ptr->Add_Flows (perf1_ptr);
					tim = (tim * volume + tim1 * volume1) / (volume + volume1);
				}
				if (avg_times) {
					tim = (MAX (perf_ptr->Time (), time0) + tim) / 2;
					perf_ptr->Time (tim);

					if (prev_ptr > 0) {
						avg_tt = (prv + 2.0 * prev + tim) / 4.0;
						if (avg_tt >= MAX_INTEGER) avg_tt = MAX_INTEGER - 1;
						prev_ptr->Time ((int) avg_tt);
					}
					prev_ptr = perf_ptr;
					prv = prev;
					prev = tim;

				} else {
					perf_ptr->Time (tim);
				}
				if (zero_flows) {
					perf_ptr->Clear_Flows ();
				} else {
					tim = perf_ptr->Time ();
					if (tim < time0) tim = time0;

					speed = length / tim.Seconds ();
					if (speed < 0.1) speed = 0.1;

					perf_ptr->Veh_Time (perf_ptr->Veh_Dist () / speed);
				}
			}
		}
	}

<<<<<<< .working
	if (turn_updates && dat->System_File_Flag (SIGNAL) && dat->System_File_Flag (CONNECTION)) {
		int index, node, lanes, cycle, green, min_green;
		double red, sf, gc, vc, cap, max_flow, max_cap, flow, delay;
=======
	if (turn_updates && dat->System_File_Flag (SIGNAL) && dat->System_File_Flag (CONNECTION)) {
		int index, node, lanes, cycle, green, min_green, type;
		double red, sf, gc, vc, cap, max_flow, max_cap, flow, delay, max_delay;
>>>>>>> .merge-right.r1529

		Turn_Period_Itr per_itr;
		Turn_Itr turn_itr;
		Connect_Data *connect_ptr;
		Dir_Data *dir_ptr;
		Link_Data *link_ptr;
		Node_Data *node_ptr;
		Signal_Data *signal_ptr;
		Signal_Time_Itr signal_time_itr;
		Timing_Itr timing_itr;
		Timing_Phase_Itr phase_itr;
		Phasing_Itr phasing_itr;
		Movement_Itr move_itr;

<<<<<<< .working
		tod = tod1;

		for (per_itr = dat->turn_period_array.begin (); per_itr != dat->turn_period_array.end (); per_itr++, tod += period) {
			if (tod < first_time) continue;

			for (index=0, turn_itr = per_itr->begin (); turn_itr != per_itr->end (); turn_itr++, index++) {
				turn_itr->Time (0);
				if (turn_itr->Turn () == 0) continue;

				connect_ptr = &dat->connect_array [index];

				dir_ptr = &dat->dir_array [connect_ptr->Dir_Index ()];

				if (dir_ptr->Sign () == STOP_SIGN) {
					turn_itr->Time  (200);
				} else if (dir_ptr->Sign () == ALL_STOP) {
					turn_itr->Time  (100);
				} else if (dir_ptr->Sign () == YIELD_SIGN) {
					turn_itr->Time  (50);
				} else {
					link_ptr = &dat->link_array [dir_ptr->Link ()];

					node = (dir_ptr->Dir () == 1) ? link_ptr->Anode () : link_ptr->Bnode ();
					node_ptr = &dat->node_array [node];
					if (node_ptr->Control () < 0) continue;

					signal_ptr = &dat->signal_array [node_ptr->Control ()];
					green = min_green = cycle = 0;

					for (signal_time_itr = signal_ptr->begin (); signal_time_itr != signal_ptr->end (); signal_time_itr++) {
						if (signal_time_itr->Start () <= tod && signal_time_itr->End () >= tod) {
							for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
								if (phasing_itr->Phasing () == signal_time_itr->Phasing ()) break;
							}
							for (timing_itr = signal_ptr->timing_plan.begin (); timing_itr != signal_ptr->timing_plan.end (); timing_itr++) {
								if (timing_itr->Timing () == signal_time_itr->Timing ()) break;
							}
							cycle = timing_itr->Cycle ();

							for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
								if (move_itr->Connection () == index) {
									for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
										if (phase_itr->Phase () == phasing_itr->Phase ()) {
											green += phase_itr->Max_Green ();
											min_green += phase_itr->Min_Green ();
										}
									}
								}
							}
						}
					}
					if (green == 0 || cycle == 0) continue;

					flow = turn_itr->Turn () * vol_fac;
					lanes = (connect_ptr->High_Lane () - connect_ptr->Low_Lane () + 1);
					if (green > cycle * 9 / 10) {
						green = (green + min_green) / 2;
					}
					red = cycle - green;
					sf = 1800 * lanes;
					gc = (double) green / cycle;
					max_flow = sf * gc;
					vc = flow / max_flow;
					cap = sf * gc / vol_fac;
					max_cap = 900 / vol_fac;

					delay = (red * red * sf / ((sf - flow) * 2 * cycle)) + 
						(max_cap * ((vc - 1) + sqrt (pow ((vc - 1), 2) + (4 * vc / cap))));

					turn_itr->Time (Dtime (delay));
=======
		tod = tod1;
		max_delay = Dtime (1.0, HOURS).Seconds ();

		for (per_itr = dat->turn_period_array.begin (); per_itr != dat->turn_period_array.end (); per_itr++, tod += period) {
			if (tod < first_time) continue;

			for (index=0, turn_itr = per_itr->begin (); turn_itr != per_itr->end (); turn_itr++, index++) {
				turn_itr->Time (0);
				if (turn_itr->Turn () == 0) continue;

				connect_ptr = &dat->connect_array [index];

				//---- assume an uncontrolled yield sign -----

				if (connect_ptr->Type () == RIGHT) {
					dir_ptr = &dat->dir_array [connect_ptr->To_Index ()];
					link_ptr = &dat->link_array [dir_ptr->Link ()];
					type = link_ptr->Type ();

					if (type == RAMP || type == FREEWAY || type == EXPRESSWAY) {
						turn_itr->Time (50);
						continue;
					}
>>>>>>> .merge-right.r1529
				}

				//---- apply the approach controls ----

				dir_ptr = &dat->dir_array [connect_ptr->Dir_Index ()];

				if (dir_ptr->Sign () == STOP_SIGN) {
					turn_itr->Time (200);
				} else if (dir_ptr->Sign () == ALL_STOP) {
					turn_itr->Time (100);
				} else if (dir_ptr->Sign () == YIELD_SIGN) {
					turn_itr->Time (50);
				} else if (connect_ptr->Type () == LEFT || connect_ptr->Type () == RIGHT && connect_ptr->Type () == UTURN) {
					link_ptr = &dat->link_array [dir_ptr->Link ()];

					//---- assume an uncontrolled yield sign ----

					if (link_ptr->Type () == RAMP && connect_ptr->Type () == RIGHT) {
						turn_itr->Time (50);
						continue;
					}

					//---- get the green time for the signal -----

					node = (dir_ptr->Dir () == 1) ? link_ptr->Anode () : link_ptr->Bnode ();
					node_ptr = &dat->node_array [node];
					if (node_ptr->Control () < 0) continue;

					signal_ptr = &dat->signal_array [node_ptr->Control ()];
					green = min_green = cycle = 0;

					for (signal_time_itr = signal_ptr->begin (); signal_time_itr != signal_ptr->end (); signal_time_itr++) {
						if (signal_time_itr->Start () <= tod && signal_time_itr->End () >= tod) {
							for (phasing_itr = signal_ptr->phasing_plan.begin (); phasing_itr != signal_ptr->phasing_plan.end (); phasing_itr++) {
								if (phasing_itr->Phasing () == signal_time_itr->Phasing ()) break;
							}
							for (timing_itr = signal_ptr->timing_plan.begin (); timing_itr != signal_ptr->timing_plan.end (); timing_itr++) {
								if (timing_itr->Timing () == signal_time_itr->Timing ()) break;
							}
							cycle = timing_itr->Cycle ();

							for (move_itr = phasing_itr->begin (); move_itr != phasing_itr->end (); move_itr++) {
								if (move_itr->Connection () == index) {
									for (phase_itr = timing_itr->begin (); phase_itr != timing_itr->end (); phase_itr++) {
										if (phase_itr->Phase () == phasing_itr->Phase ()) {
											green += phase_itr->Max_Green ();
											min_green += phase_itr->Min_Green ();
										}
									}
								}
							}
						}
					}
					if (green == 0 || cycle == 0) continue;

					//---- estimate the signal delay ----

					flow = turn_itr->Turn () * vol_fac;
					lanes = (connect_ptr->High_Lane () - connect_ptr->Low_Lane () + 1);
					if (green > cycle * 9 / 10) {
						green = (green + min_green) / 2;
					}
					red = cycle - green;
					sf = 1800 * lanes;
					gc = (double) green / cycle;
					max_flow = sf * gc;
					vc = flow / max_flow;
					cap = sf * gc / vol_fac;
					max_cap = 900 / vol_fac;

					delay = (red * red * sf / ((sf - flow) * 2 * cycle)) + 
						(max_cap * ((vc - 1) + sqrt (pow ((vc - 1), 2) + (4 * vc / cap))));

					if (avg_times) {
						delay = (delay + turn_itr->Time ().Seconds ()) / 2.0;
					}
					if (delay > max_delay) delay = max_delay;
					turn_itr->Time (Dtime (delay));
				}
			}
		}
	}
}

