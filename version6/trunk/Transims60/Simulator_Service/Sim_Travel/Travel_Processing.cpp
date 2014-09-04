//*********************************************************
//	Travel_Processing.cpp - assign vehicles to links
//*********************************************************

#include "Sim_Travel_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Traveler_Processing
//---------------------------------------------------------

void Sim_Travel_Process::Travel_Processing (Sim_Travel_Ptr sim_travel_ptr)
{
	Dtime next_event;
	Travel_Step step;

	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Ptr sim_leg_ptr, next_leg_ptr;
	Vehicle_Map_Itr veh_map_itr;
	Sim_Veh_Ptr sim_veh_ptr;
	Sim_Veh_Data sim_veh;

	if (sim_travel_ptr == 0) return;

	sim_plan_ptr = sim_travel_ptr->Get_Plan ();

	if (sim_plan_ptr == 0) {
		sim_travel_ptr->Status (NOT_ACTIVE);
		sim_travel_ptr->Next_Event (sim->param.end_time_step);
		return;
	}

	step.Process_ID (ID ());
	step.Traveler (sim_travel_ptr->Traveler ());
	step.sim_travel_ptr = sim_travel_ptr;

	//---- check the next scheduled event ----

	do {
#ifdef CHECK
		if (sim_travel_ptr == 0) sim->Error ("Sim_Travel_Process::Travel_Processing: sim_travel_ptr");
#endif
		if (sim_travel_ptr->Next_Event () > sim->time_step) break;

		sim_plan_ptr = sim_travel_ptr->Get_Plan ();
#ifdef CHECK
		if (sim_plan_ptr == 0) sim->Error ("Sim_Travel_Process::Travel_Processing: sim_plan_ptr");
#endif
		//---- new trip ----

		if (sim_travel_ptr->Status () == NOT_ACTIVE) {

			//---- check the departure time constraint ----

			if ((sim_plan_ptr->Start () + sim->param.max_start_variance) < sim->time_step) {
				step.Problem (DEPARTURE_PROBLEM);
				sim->Output_Step (step);
				continue;
			}

			//---- activate the trip ----

			sim_travel_ptr->Next_Event (sim_plan_ptr->Start ());
			sim_travel_ptr->Status (OFF_NET_START);

			if (sim_travel_ptr->Person () > 0) {
				stats.num_start++;

				//---- output start event ----

				step.Event_Type (TRIP_START_EVENT);
				sim->sim_output_step.Event_Check (step);
			}

		} else {
			sim_plan_ptr->Next_Leg ();
		}

		//---- end of the trip ----

		if (sim_plan_ptr->First_Leg () < 0) {
			sim_travel_ptr->Status (OFF_NET_END);

			if (sim_travel_ptr->Person () == 0) {
				stats.num_run_end++;
			} else {
				stats.num_end++;

				stats.tot_hours += sim->time_step - sim_plan_ptr->Start ();

				//---- output end event ----

				step.Event_Type (TRIP_END_EVENT);
				sim->sim_output_step.Event_Check (step);
			}
			sim->Output_Step (step);
			continue;
		}

		//---- check the arrival time constraint ----

		if ((sim_plan_ptr->End () + sim->param.max_end_variance) < sim->time_step) {
			step.Problem (ARRIVAL_PROBLEM);
			sim->Output_Step (step);
			continue;
		}

		//---- get the current plan leg ----

		sim_leg_ptr = sim_plan_ptr->Get_Leg ();

		if (sim_leg_ptr == 0) {
			sim->Warning ("End of Plan Problem");
			break;
		}
		next_event = sim->time_step + sim_leg_ptr->Time ();

		//---- check the leg type ----

		if (sim_leg_ptr->Mode () == DRIVE_MODE) {

			if (sim_leg_ptr->Type () != DIR_ID) {
				sim->Warning ("Drive Type Error=") << sim_leg_ptr->Type ();
				break;
			} else {
				step.Dir_Index (sim_leg_ptr->Index ());
#ifdef CHECK
				if (sim_leg_ptr->Type () != DIR_ID) sim->Error (String ("Sim_Travel_Process::Travel_Processing: Leg Type=%d") % sim_leg_ptr->Type ());
				if (step.Dir_Index () < 0 || (int) sim->sim_dir_array.size () <= step.Dir_Index ()) sim->Error ("Sim_Travel_Process::Travel_Processing: Dir_Index");
#endif
				step.sim_dir_ptr = &sim->sim_dir_array [sim_leg_ptr->Index ()];

				if (step.sim_dir_ptr->Method () == NO_SIMULATION) {
					sim_travel_ptr->Status (OFF_NET_DRIVE);
				} else {

					//---- add the vehicle to the link load queue ----

					sim_travel_ptr->Status (OFF_ON_DRIVE);
					step.sim_dir_ptr->Load_Queue (sim_travel_ptr->Traveler ());
					break;
				}
			}
		} else if (sim_leg_ptr->Type () == PARKING_ID || sim_leg_ptr->Type () == STOP_ID) {
			next_leg_ptr = sim_plan_ptr->Get_Next (sim_leg_ptr);

			//---- check for a network link ----

			if (next_leg_ptr != 0 && next_leg_ptr->Mode () == DRIVE_MODE && next_leg_ptr->Type () == DIR_ID) {

				//---- find the vehicle ----

				veh_map_itr = sim->sim_veh_map.find (Vehicle_Index (sim_travel_ptr->Household (), sim_plan_ptr->Vehicle ()));

				if (veh_map_itr == sim->sim_veh_map.end ()) {
					step.Problem (VEHICLE_PROBLEM);
					sim->Output_Step (step);
					continue;
				}

				//---- check the status of the vehicle ----

				sim_veh_ptr = &sim->sim_veh_array [veh_map_itr->second];

				if (!sim_veh_ptr->Parked ()) break;

				//---- assign the vehicle to the driver ----

				sim_travel_ptr->Vehicle (veh_map_itr->second);

				//---- check the simulation method ----
						
				step.Dir_Index (next_leg_ptr->Index ());
#ifdef CHECK
				if (next_leg_ptr->Type () != DIR_ID) sim->Error (String ("Sim_Travel_Process::Travel_Processing: Next Type=%d") % next_leg_ptr->Type ());
				if (step.Dir_Index () < 0 || (int) sim->sim_dir_array.size () <= step.Dir_Index ()) sim->Error ("Sim_Travel_Process::Travel_Processing: Dir_Index");
#endif
				step.sim_dir_ptr = &sim->sim_dir_array [next_leg_ptr->Index ()];

				if (step.sim_dir_ptr->Method () == NO_SIMULATION) {
					if (sim_travel_ptr->Person () == 0) {
						if (sim_travel_ptr->Status () == OFF_NET_START)	stats.num_run_start++;
					} else {
						stats.num_veh_start++;
						step.Event_Type (VEH_START_EVENT);
						sim->sim_output_step.Event_Check (step);
					}
					sim_travel_ptr->Status (OFF_NET_LOAD);

				} else if (sim_leg_ptr->Type () == STOP_ID) {

					//---- add the firt stop to the link load queue ----

					if (sim_travel_ptr->Status () == OFF_NET_START) {
						sim_travel_ptr->Status (OFF_ON_LOAD);
						step.sim_dir_ptr->Load_Queue (sim_travel_ptr->Traveler ());
						break;
					}

				} else {

					//---- add the vehicle to the link load queue ----

					sim_travel_ptr->Status (OFF_ON_LOAD);
					step.sim_dir_ptr->Load_Queue (sim_travel_ptr->Traveler ());
					break;
				}

			} else {

				//---- park the vehicle ----

				if (sim_travel_ptr->Person () > 0) {
					stats.num_veh_end++;
					sim_travel_ptr->Status (OFF_NET_MOVE);

					//---- output vehicle end event ----

					step.Event_Type (VEH_END_EVENT);
					sim->sim_output_step.Event_Check (step);
					sim_plan_ptr->Next_Leg ();
				} else {
					sim_travel_ptr->Next_Plan ();
					stats.num_run_end++;
					sim_travel_ptr->Status (NOT_ACTIVE);
				}
			}
		}
		sim_travel_ptr->Next_Event (next_event);

	} while (sim_travel_ptr->Plan_Index () >= 0 && sim_travel_ptr->Next_Event () <= sim->time_step && sim_travel_ptr->Status () <= OFF_NET_END);
}
