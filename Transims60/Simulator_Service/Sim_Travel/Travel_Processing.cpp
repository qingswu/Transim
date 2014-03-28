//*********************************************************
//	Travel_Processing.cpp - assign vehicles to links
//*********************************************************

#include "Sim_Travel_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Traveler_Processing
//---------------------------------------------------------

bool Sim_Travel_Process::Travel_Processing (Sim_Travel_Ptr sim_travel_ptr)
{
	//return true to update the link load queue ----

	Dtime next_event;
	int subarea;

	Travel_Step step;

	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Itr leg_itr, next_leg;
	Vehicle_Map_Itr veh_map_itr;
	Sim_Veh_Ptr sim_veh_ptr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Veh_Data sim_veh;

	sim_plan_ptr = sim_travel_ptr->sim_plan_ptr;

	if (sim_plan_ptr == 0) return (false);

	step.Traveler (sim_travel_ptr->Traveler ());
	step.sim_travel_ptr = sim_travel_ptr;

	subarea = 0;	//************************//

	//---- check the next scheduled event ----

	do {
		if (sim_travel_ptr->Next_Event () > sim->time_step) break;

		sim_plan_ptr = sim_travel_ptr->sim_plan_ptr;

		//---- new trip ----

		if (sim_travel_ptr->Next_Event () < 0) {

			//---- check the departure time constraint ----

			if ((sim_plan_ptr->Start () + sim->param.max_start_variance) < sim->time_step) {
				step.Problem (DEPARTURE_PROBLEM);
				step.Status (3);

				if (!Travel_Update (step)) break;
				continue;
			}
			sim_travel_ptr->Next_Event (sim_plan_ptr->Start ());

			if (sim_travel_ptr->Person () > 0) {
				stats.num_start++;
				sim_travel_ptr->Status (-1);

				//---- output event record ----

				//if (sim->event_output.In_Range (TRIP_START_EVENT, sim_plan_ptr->Mode (), subarea)) {
				//	Event_Data event_data;

				//	event_data.Household (sim_travel_ptr->Household ());
				//	event_data.Person (sim_travel_ptr->Person ());
				//	event_data.Tour (sim_plan_ptr->Tour ());
				//	event_data.Trip (sim_plan_ptr->Trip ());
				//	event_data.Mode (sim_plan_ptr->Mode ());
				//	event_data.Schedule (sim_plan_ptr->Start ());
				//	event_data.Actual (sim->time_step);
				//	event_data.Event (TRIP_START_EVENT);

				//	sim->event_output.Output_Event (event_data);
				//}
			}

		} else { 

			if (sim_travel_ptr->Status () == -2) {
				leg_itr = sim_plan_ptr->begin ();

				if (leg_itr != sim_plan_ptr->end ()) {
					if (leg_itr->Type () == STOP_ID) {
exe->Write (1, " stop");
					} else if (leg_itr->Type () == PARKING_ID) {
exe->Write (1, " parking");
					} else {
						sim_veh.Location (leg_itr->Index (), 0, 0);
						step.assign (1, sim_veh);

						step.sim_dir_ptr = &sim->sim_dir_array [leg_itr->Index ()];

						sim_veh.offset = step.sim_dir_ptr->Length ();
						step.push_back (sim_veh);

						step.Time (leg_itr->Time ());

						sim->sim_output_step.Check_Output (step);
					}
				}
			}
			sim_plan_ptr->Next_Leg ();
		}

		//---- end of the trip ----

		if (sim_plan_ptr->size () == 0) {
			if (sim_travel_ptr->Person () == 0) {
				stats.num_run_end++;
				break;
			}
			stats.num_end++;

			//---- output event record ----

			//if (sim->event_output.In_Range (TRIP_END_EVENT, sim_plan_ptr->Mode (), subarea)) {
			//	Event_Data event_data;

			//	event_data.Household (sim_travel_ptr->Household ());
			//	event_data.Person (sim_travel_ptr->Person ());
			//	event_data.Tour (sim_plan_ptr->Tour ());
			//	event_data.Trip (sim_plan_ptr->Trip ());
			//	event_data.Mode (sim_plan_ptr->Mode ());
			//	event_data.Schedule (sim_plan_ptr->End ());
			//	event_data.Actual (sim->time_step);
			//	event_data.Event (TRIP_END_EVENT);

			//	sim->event_output.Output_Event (event_data);
			//}
			if (!Travel_Update (step)) break;
			continue;
		}

		//---- check the arrival time constraint ----

		if ((sim_plan_ptr->End () + sim->param.max_end_variance) < sim->time_step) {
			step.Problem (ARRIVAL_PROBLEM);
			step.Status (3);

			if (!Travel_Update (step)) break;
			continue;
		}

		//---- get the current plan leg ----

		leg_itr = sim_plan_ptr->begin ();
		if (leg_itr == sim_plan_ptr->end ()) {
			if (!Travel_Update (step)) break;
			continue;
		}
		next_event = sim->time_step + leg_itr->Time ();

		//---- check the leg type ----

		if (leg_itr->Mode () == DRIVE_MODE) {

			if (leg_itr->Type () != DIR_ID) {
				sim->Write (1, "drive type error=") << leg_itr->Type ();
			} else {
				sim_dir_ptr = &sim->sim_dir_array [leg_itr->Index ()];

				if (sim_dir_ptr->Method () == NO_SIMULATION) {

				} else {
					sim->Write (1, "start drive simulation");
				}
			}
		} else if (leg_itr->Type () == PARKING_ID || leg_itr->Type () == STOP_ID) {
			next_leg = leg_itr + 1;

			//---- check for a network link ----

			if (next_leg != sim_plan_ptr->end () && next_leg->Mode () == DRIVE_MODE && next_leg->Type () == DIR_ID) {

				//---- find the vehicle ----

				veh_map_itr = sim->sim_veh_map.find (Vehicle_Index (sim_travel_ptr->Household (), sim_plan_ptr->Vehicle ()));

				if (veh_map_itr == sim->sim_veh_map.end ()) {
					step.Problem (VEHICLE_PROBLEM);
					step.Status (3);

					if (!Travel_Update (step)) break;
					continue;
				}

				//---- check the status of the vehicle ----

				sim_veh_ptr = &sim->sim_veh_array [veh_map_itr->second];

				if (!sim_veh_ptr->Parked ()) break;

				//---- assign the vehicle to the driver ----

				sim_travel_ptr->Vehicle (veh_map_itr->second);

				//---- check the simulation method ----

				sim_dir_ptr = &sim->sim_dir_array [next_leg->Index ()];

				if (sim_dir_ptr->Method () == NO_SIMULATION) {

					sim_travel_ptr->Status (-2);

					if (sim_travel_ptr->Person () == 0) {
						stats.num_run_start++;
					} else {
						stats.num_veh_start++;

						//---- output event record ----

						//if (sim->event_output.In_Range (VEH_START_EVENT, sim_plan_ptr->Mode (), subarea)) {
						//	Event_Data event_data;

						//	event_data.Household (sim_travel_ptr->Household ());
						//	event_data.Person (sim_travel_ptr->Person ());
						//	event_data.Tour (sim_plan_ptr->Tour ());
						//	event_data.Trip (sim_plan_ptr->Trip ());
						//	event_data.Mode (sim_plan_ptr->Mode ());
						//	event_data.Schedule (sim_travel_ptr->Next_Event ().Round_Seconds ());
						//	event_data.Actual (sim->time_step);
						//	event_data.Event (VEH_START_EVENT);
						//	event_data.Dir_Index (next_leg->Index ());
						//	//event_data.Offset (offset);
						//	//event_data.Lane (lane);
						//	event_data.Offset (0);
						//	event_data.Lane (0);
						//	event_data.Route (-1);

						//	sim->event_output.Output_Event (event_data);
						//}
					}

				} else {

					//---- add the vehicle to the link load queue ----

					sim_dir_ptr->Load_Queue (sim_travel_ptr->Traveler ());

					sim_travel_ptr->Status (2);
					break;
				}
			//} else if (leg_itr->Type () == PARKING_ID) {  ???? STOP_ID

			} else {

				//---- park the vehicle ----

				sim_travel_ptr->Status (-1);

				if (sim_travel_ptr->Person () == 0) {
					stats.num_run_end++;
				} else {
					stats.num_veh_end++;

					stats.tot_hours += sim->time_step - sim_plan_ptr->Start ();

					//---- output event record ----

					//if (sim->event_output.In_Range (VEH_END_EVENT, sim_plan_ptr->Mode (), subarea)) {
					//	Event_Data event_data;

					//	event_data.Household (sim_travel_ptr->Household ());
					//	event_data.Person (sim_travel_ptr->Person ());
					//	event_data.Tour (sim_plan_ptr->Tour ());
					//	event_data.Trip (sim_plan_ptr->Trip ());
					//	event_data.Mode (sim_plan_ptr->Mode ());
					//	event_data.Schedule (sim_travel_ptr->Schedule ());
					//	event_data.Actual (sim->time_step);
					//	event_data.Event (VEH_END_EVENT);

					//	sim->event_output.Output_Event (event_data);
					//}
				}
			}
		}
		sim_travel_ptr->Next_Event (next_event);

	} while (sim_travel_ptr->sim_plan_ptr && sim_travel_ptr->Next_Event () <= sim->time_step && sim_travel_ptr->Status () <= 0);

	return (true);
}
