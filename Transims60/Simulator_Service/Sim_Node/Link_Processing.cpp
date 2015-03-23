//*********************************************************
//	Link_Processing.cpp - simulate directional links 
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Link_Processing
//---------------------------------------------------------

bool Sim_Node_Process::Link_Processing (int link)
{
	int i, max_exit, exit_count, lane0, lane, cell, traveler, step_size, next_load;
	double max_flow;
	Dtime time;

	Int_Itr load_itr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Travel_Ptr sim_travel_ptr, last_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Ptr sim_leg_ptr;
	Travel_Step step;
	Random random_flow;

	max_exit = exit_count = 0;

	step.Process_ID (ID ());
	step.Dir_Index (link);
	step.sim_dir_ptr = sim_dir_ptr = &sim->sim_dir_array [link];

	//---- check the simulation method ----	

	step_size = sim->method_time_step [sim_dir_ptr->Method ()];
	if (step_size <= 0) return (false);

	lane0 = (sim->time_step / step_size) % sim_dir_ptr->Lanes ();

	//---- check for loaded vehicles ----

	sim->sim_dir_array.Lock (sim_dir_ptr, ID ());
//sim->Warning (String ("Sim_Node_Process::Link_Process: Lock %d") % sim_dir_ptr->Lock ());
	if (sim_dir_ptr->Count () <= 0) goto load_check;

	//---- calculate maximum macroscopic throughput ----

	if (sim_dir_ptr->Method () == MACROSCOPIC) {
		exit_count = 0;
		max_flow = sim->UnRound (sim_dir_ptr->Max_Flow ());
		max_exit = (int) max_flow;
		max_flow -= max_exit;
		if (max_flow > 0) {
			if (max_flow > random_flow.Probability (link + sim->time_step)) {
				max_exit++;
			}
		}
		step.Exit_Flag (max_exit > 0);
	}

	//---- scan the link cells for vehicles ----

	for (cell = sim_dir_ptr->Max_Cell (); cell >= 0; cell--) {

		//---- check each lane ----

		for (i=0, lane=lane0; i < sim_dir_ptr->Lanes (); i++, lane++) {
			if (lane >= sim_dir_ptr->Lanes ()) lane = 0;

			traveler = sim_dir_ptr->Get (lane, cell);
			if (traveler <= 0) continue;

			step.Clear_Travel ();
			step.Traveler (traveler);
			step.sim_travel_ptr = sim_travel_ptr = &sim->sim_travel_array [traveler];

			//---- check if the traveler has already been processed during this time step ----

			if (sim_travel_ptr->Step_Code () == sim->Step_Code ()) continue;

			//---- data problem ----

			if (sim_travel_ptr->Vehicle () < 1) {
				step.Problem (PATH_PROBLEM);
				sim->Output_Step (step);
				continue;
			}

			//---- stop and wait for the event time ----

			if (sim_travel_ptr->Next_Event () > sim->time_step) {
				step.sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];
				step.push_back (*step.sim_veh_ptr);

				sim_travel_ptr->Speed (0);
				sim->Output_Step (step);
				if (sim_travel_ptr->Person () > 0 || sim_travel_ptr->Passengers () > 0) num_vehicles++;
				num_pce++;
				continue;
			}

			//---- check the wait time constraint ----

			if (sim_travel_ptr->Wait () > sim->param.max_wait_time) {
				step.Problem (WAIT_PROBLEM);
				sim->Output_Step (step);
				continue;
			}

			//---- data problem ----

			step.sim_plan_ptr = sim_plan_ptr =  sim_travel_ptr->Get_Plan ();

			if (sim_plan_ptr == 0) {
				step.Problem (PATH_PROBLEM);
				sim->Output_Step (step);
				continue;
			}

			//---- check the vehicle position ----

			step.sim_veh_ptr = &sim->sim_veh_array [sim_travel_ptr->Vehicle ()];

			if (step.sim_veh_ptr->link != link) {
				step.Problem (PATH_PROBLEM);
				sim->Output_Step (step);
				continue;
			}

			//---- check the plan legs ----

			sim_leg_ptr = sim_plan_ptr->Get_Leg ();

			if (sim_leg_ptr == 0) {
				sim_travel_ptr->Next_Plan ();
				continue;
			}

			if (sim_leg_ptr->Type () == STOP_ID) {
				if (!sim_plan_ptr->Next_Leg ()) {
					step.sim_veh_ptr->Parked (true);
					sim->Output_Step (step);
					stats.num_run_end++;
					continue;
				}
			}

			//---- check the arrival time constraint ----

			if ((sim_plan_ptr->End () + sim->param.max_end_variance) < sim->time_step) {
				step.Problem (ARRIVAL_PROBLEM);
				sim->Output_Step (step);
				continue;
			}

			//---- try to move the vehicle ----

			if (Move_Vehicle (step)) {
				if (sim_dir_ptr->Method () == MACROSCOPIC) {
					if (++exit_count >= max_exit) {
						step.Exit_Flag (false);
					}
				}
			}
			if (sim_travel_ptr->Status () == ON_NET_DRIVE) {
				if (sim_travel_ptr->Person () > 0 || sim_travel_ptr->Passengers () > 0) num_vehicles++;
				num_pce++;
			}
		}

		//---- priority load processing ----

		if (cell == sim_dir_ptr->Load_Cell ()) {
			for (traveler = sim_dir_ptr->First_Load (); traveler >= 0; traveler = sim_travel_ptr->Next_Load ()) {
				sim_travel_ptr = &sim->sim_travel_array [traveler];

				if (sim_travel_ptr->Status () == OFF_ON_LOAD || sim_travel_ptr->Status () == OFF_ON_DRIVE) {
					if (sim_travel_ptr->Wait () > sim->param.min_load_time) {
						if (Load_Vehicle (traveler)) {
							sim_dir_ptr->Load_Cell (-1);
							if (sim_travel_ptr->Status () == ON_NET_DRIVE) {
								if (sim_travel_ptr->Person () > 0 || sim_travel_ptr->Passengers () > 0) num_vehicles++;
								num_pce++;
							}
							break;
						}
					}
				}
			}
		}
	}

	//---- update the load list ----

load_check:
	sim_dir_ptr->Load_Cell (-1);

	traveler = sim_dir_ptr->First_Load ();
	sim_dir_ptr->First_Load (-1);
	sim_dir_ptr->Last_Load (-1);
	last_travel_ptr = 0;

	for (; traveler > 0; traveler = next_load) {
		sim_travel_ptr = &sim->sim_travel_array [traveler];
		next_load = sim_travel_ptr->Next_Load ();
		sim_travel_ptr->Next_Load (-1);

		if (sim_travel_ptr->Status () != OFF_ON_LOAD && sim_travel_ptr->Status () != OFF_ON_DRIVE) continue;
			
		//---- try to load the vehicle onto the link ----

		if (Load_Vehicle (traveler)) {
			if (sim_travel_ptr->Status () == ON_NET_DRIVE) {
				if (sim_travel_ptr->Person () > 0 || sim_travel_ptr->Passengers () > 0) num_vehicles++;
				num_pce++;
			}
		} else {
			sim_travel_ptr->Add_Wait (step_size);

			//---- check the load time constraint ----

			if (sim_travel_ptr->Wait () < sim->param.max_load_time) {
				if (last_travel_ptr == 0) {
					sim_dir_ptr->First_Load (traveler);
				} else {
					last_travel_ptr->Next_Load (traveler);
				}
				sim_dir_ptr->Last_Load (traveler);
				last_travel_ptr = sim_travel_ptr;

				//---- number of non-transit vehicles still in the loading queue ----

				if (sim_travel_ptr->Person () > 0 || sim_travel_ptr->Passengers () > 0) num_waiting++;

			} else {

				//---- process a load problem ----

				step.clear ();
				step.Traveler (traveler);
				step.sim_travel_ptr = sim_travel_ptr;
				step.sim_plan_ptr = 0;
				step.sim_veh_ptr = 0;

				step.Problem (LOAD_PROBLEM);

				sim->Output_Step (step);
			}
		}
	}
#ifdef CHECK
	if (ID () > 0 && sim_dir_ptr->Lock () != ID ()) sim->Error (String ("Sim_Node_Process::Link_Processing UnLock (% vs %d)") % sim_dir_ptr->Lock () % ID ());
#endif
	sim->sim_dir_array.UnLock (sim_dir_ptr, ID ());
	return (true);
}
