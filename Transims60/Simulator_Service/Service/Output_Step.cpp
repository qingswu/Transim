//*********************************************************
//	Output_Step.cpp - output the travel step directives
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Output_Step
//---------------------------------------------------------

bool Simulator_Service::Output_Step (Travel_Step &step)
{
	bool new_plan, problem_flag;

	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Itr leg_itr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Veh_Data sim_veh;
	Sim_Veh_Ptr sim_veh_ptr;
	Problem_Data problem_data;
	
	//---- check the traveler data ----

	if (step.sim_travel_ptr == 0) {
		if (step.Traveler () < 0) return (false);

		step.sim_travel_ptr = &sim->sim_travel_array [step.Traveler ()];
	}

	//---- mark the traveler as processed in this time step ----

	step.sim_travel_ptr->Step_Code (sim->Step_Code ());

	if (step.sim_plan_ptr == 0) {
		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
	}
	sim_plan_ptr = step.sim_plan_ptr;
#ifdef CHECK
	if (sim_plan_ptr == 0) sim->Error ("Simulator_Service::Output_Step: sim_plan_ptr");
#endif

	new_plan = false;

	//---- process the problem message ----

	if (step.Problem () > 0) {
		problem_flag = (!sim->select_problems || sim->problem_range.In_Range (step.Problem ()));

		//---- check for lost problems ----

		if (step.Problem () == DEPARTURE_PROBLEM || step.Problem () == ARRIVAL_PROBLEM || step.Problem () == WAIT_PROBLEM || 
			step.Problem () == LOAD_PROBLEM || step.Problem () == PATH_PROBLEM) {

			if (step.sim_travel_ptr->Person () == 0) {
				stats.num_run_lost++;
			} else {
				stats.num_veh_lost++;

				//---- check the lost vehicle event ----

				if (step.sim_travel_ptr->Vehicle () >= 0) {
					step.Event_Type (VEH_LOST_EVENT);
					sim->sim_output_step.Event_Check (step);
				}
			}
			new_plan = true;
		}

		//---- count problems ----

		if (new_plan || problem_flag || sim->param.count_warnings) {
			step.sim_travel_ptr->Problem (1);
			sim->Set_Problem ((Problem_Type) step.Problem ());
		}

		//---- write selected problems -----

		if (problem_flag && sim->param.problem_flag && sim_plan_ptr != 0) {
			if ((int) step.size () > 0) {
				sim_veh = step.back ();
			} else {
#ifdef CHECK
				if (step.sim_travel_ptr->Vehicle () < 0) sim->Error ("Simulator_Service::Output_Step: Vehicle");
#endif
				sim_veh = sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];
			}
			problem_data.Household (step.sim_travel_ptr->Household ());
			problem_data.Person (step.sim_travel_ptr->Person ());
			problem_data.Tour (sim_plan_ptr->Tour ());
			problem_data.Trip (sim_plan_ptr->Trip ());

			problem_data.Start (sim_plan_ptr->Start ());
			problem_data.End (sim_plan_ptr->End ());
			problem_data.Duration (sim_plan_ptr->Activity ());

			problem_data.Origin (sim_plan_ptr->Origin ());
			problem_data.Destination (sim_plan_ptr->Destination ());

			problem_data.Purpose (sim_plan_ptr->Purpose ());
			problem_data.Mode (sim_plan_ptr->Mode ());
			problem_data.Constraint (sim_plan_ptr->Constraint ());
			problem_data.Priority (sim_plan_ptr->Priority ());
			problem_data.Vehicle (sim_plan_ptr->Vehicle ());

			problem_data.Type (sim_plan_ptr->Type ());
			problem_data.Problem (step.Problem ());
			problem_data.Time (sim->time_step);

			problem_data.Dir_Index (sim_veh.link);
			problem_data.Lane (sim_veh.lane);
			problem_data.Offset (sim_veh.offset);
			problem_data.Route (-1);

			if (step.sim_travel_ptr->Person () == 0) {
				int route = sim->line_array.Route (step.sim_travel_ptr->Household ());

				Int_Map_Itr map_itr = sim->line_map.find (route);
				if (map_itr != sim->line_map.end ()) {
					problem_data.Route (map_itr->second);
				}
			}
			sim->sim_output_step.Output_Problem (problem_data);
		}
	}
	step.Problem (0);

	//---- process vehicle movement ----

	if (step.size () > 0) {
		sim->sim_output_step.Output_Check (step);
	}
	if (sim_plan_ptr->First_Leg () < 0) {
		new_plan = true;
	}

	//---- check the vehicle data ----

	if (step.sim_veh_ptr == 0) {
#ifdef CHECK
		if (step.sim_travel_ptr->Vehicle () < 0) sim->Error ("Simulator_Service::Output_Step: Vehicle");
#endif
		step.sim_veh_ptr = &sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];
#ifdef CHECK
	} else {
		sim_veh_ptr = &sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];

		if (sim_veh_ptr != step.sim_veh_ptr) {
			sim->Error ("Simulator_Service::Output_Step: Vehicle Pointer");
		}
#endif
	}
	sim_veh_ptr = step.sim_veh_ptr;
#ifdef CHECK
	if (sim_veh_ptr == 0) sim->Error ("Simulator_Service::Output_Step: sim_veh_ptr");
#endif

	if ((new_plan || sim_veh_ptr->Parked ()) && sim_veh_ptr->link >= 0) {
		sim_dir_ptr = &sim->sim_dir_array [sim_veh_ptr->link];
		int index = sim_dir_ptr->Index (sim_veh_ptr->lane, sim->Offset_Cell (sim_veh_ptr->offset));
		int traveler = sim_dir_ptr->Get (index);

		if (traveler == step.Traveler ()) {
			sim_dir_ptr->Remove (index);
#ifdef CHECK
		} else if (!sim_veh_ptr->Parked ()) {
			sim->Error (String ("Simulator_Service::Output_Step: Traveler %d vs %d") % traveler % step.Traveler ());
#endif
		}
	}

#ifdef THREADS
	if (Num_Threads () > 0 && (int) step.size () > 1) {
		int index = step.Dir_Index ();
		Sim_Veh_Itr sim_veh_itr;

		//---- release link locks ----

		for (sim_veh_itr = step.begin (); sim_veh_itr != step.end (); sim_veh_itr++) {
			if (sim_veh_itr->link != index) {
				index = sim_veh_itr->link;
				sim_dir_ptr = &sim->sim_dir_array [index];
				sim->sim_dir_array.UnLock (sim_dir_ptr, step.Process_ID ());
			}
		}
	}
#endif

	//---- move to the next plan ----

	if (new_plan) {
		sim_veh_ptr->Parked (true);
		step.sim_travel_ptr->Vehicle (0);

		step.sim_travel_ptr->Status (NOT_ACTIVE);
		step.sim_travel_ptr->Next_Event (sim_plan_ptr->Activity ());

		if (!step.sim_travel_ptr->Next_Plan ()) return (false);

		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
		
		if (step.sim_plan_ptr->Start () > step.sim_travel_ptr->Next_Event ()) {
			step.sim_travel_ptr->Next_Event (step.sim_plan_ptr->Start ());
		}
		return (true);
	}
	if (sim_veh_ptr->Parked ()) {
		step.sim_travel_ptr->Status (OFF_NET_MOVE);
	}
	return (!sim_veh_ptr->Parked ());
}
