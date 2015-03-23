//*********************************************************
//	Output_Step.cpp - output the travel step directives
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Output_Step
//---------------------------------------------------------

bool Simulator_Service::Output_Step (Travel_Step &step)
{
	bool new_plan, problem_flag, no_veh_flag;

	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Veh_Data sim_veh;
	Sim_Veh_Ptr sim_veh_ptr = 0;
	Problem_Data problem_data;
	Sim_Leg_Ptr sim_leg_ptr;

	//---- check the traveler data ----

	if (step.sim_travel_ptr == 0) {
		if (step.Traveler () < 2) return (false);

		step.sim_travel_ptr = &sim->sim_travel_array [step.Traveler ()];
	}

	//---- mark the traveler as processed in this time step ----

	step.sim_travel_ptr->Step_Code (sim->Step_Code ());

	if (step.sim_plan_ptr == 0) {
		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
	}
	sim_plan_ptr = step.sim_plan_ptr;
#ifdef CHECK
	if (sim_plan_ptr == 0) sim->Error (String ("Simulator_Service::Output_Step: sim_plan_ptr, Household=%d, Problem=%d") % step.sim_travel_ptr->Household () % step.Problem ());
#endif

	if (step.sim_leg_ptr == 0) {
		step.sim_leg_ptr = sim_plan_ptr->Get_Leg ();
	}
	sim_leg_ptr = step.sim_leg_ptr;
	no_veh_flag = ((sim_leg_ptr == 0 && step.sim_travel_ptr->Vehicle () == 0) || 
		(sim_leg_ptr != 0 && sim_leg_ptr->Mode () != DRIVE_MODE && sim_leg_ptr->Type () != STOP_ID));

	new_plan = false;

	//---- process the problem message ----

	if (step.Problem () > 0) {
		problem_flag = (!sim->select_problems || sim->problem_range.In_Range (step.Problem ()));

		//---- check for lost problems ----

		if (step.Problem () == DEPARTURE_PROBLEM || step.Problem () == ARRIVAL_PROBLEM || step.Problem () == WAIT_PROBLEM || 
			step.Problem () == LOAD_PROBLEM || step.Problem () == PATH_PROBLEM) {

			if (step.sim_travel_ptr->Person () == 0) {
				stats.num_run_lost++;
			} else if (!no_veh_flag) {
				stats.num_veh_lost++;

				//---- check the lost vehicle event ----

				if (step.sim_travel_ptr->Vehicle () > 0) {
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
			} else if (!no_veh_flag) {
#ifdef CHECK
				if (step.sim_travel_ptr->Vehicle () < 1) sim->Error ("Simulator_Service::Output_Step: Vehicle");
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
		
			if (sim_veh.link >= 0) {
				sim_dir_ptr = &sim->sim_dir_array [sim_veh.link];
				if (sim_veh.offset > sim_dir_ptr->Length ()) sim_veh.offset = sim_dir_ptr->Length ();
			}
			problem_data.Offset (sim_veh.offset);
			problem_data.Route (-1);

			if (step.sim_travel_ptr->Person () == 0) {
				int route = sim->line_array.Route (step.sim_travel_ptr->Household ());

				Int_Map_Itr map_itr = sim->line_map.find (route);
				if (map_itr != sim->line_map.end ()) {
					problem_data.Route (map_itr->second);
				}
			} else if (no_veh_flag && sim_leg_ptr != 0 && sim_leg_ptr->Type () == ROUTE_ID) {
				Int_Map_Itr map_itr = sim->line_map.find (sim_leg_ptr->Index ());
				if (map_itr != sim->line_map.end ()) {
					problem_data.Route (map_itr->second);
				}
			}
			sim->sim_output_step.Output_Problem (problem_data);
		}
	}
	step.Problem (0);

	//---- process vehicle movement ----

	if (step.size () > 0 && !no_veh_flag) {
		sim->sim_output_step.Output_Check (step);
	}
	if (sim_plan_ptr->First_Leg () < 0) {
		new_plan = true;
	}

	//---- check the vehicle data ----

	if (!no_veh_flag) {
		if (step.sim_veh_ptr == 0) {
#ifdef CHECK
			if (step.sim_travel_ptr->Vehicle () < 1) sim->Error (String ("Simulator_Service::Output_Step: Vehicle %d, Household=%d") % step.sim_travel_ptr->Vehicle () % step.sim_travel_ptr->Household ());
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

		//---- park the vehicle ----

		if (new_plan || sim_veh_ptr->Parked ()) {
			step.veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];
			Remove_Vehicle (step);
		}

#ifdef THREADS

		if (Num_Threads () > 0 && (int) step.size () > 1) {
			int index = step.Dir_Index ();
			Sim_Veh_Itr sim_veh_itr;
#ifdef CHECK
			if (index < 0 || index > (int) sim->sim_dir_array.size ()) sim->Error (String ("Output_Step::Unlock Index=%d vs %d") % index % sim->sim_dir_array.size ());
#endif
			//---- release link locks ----

			for (sim_veh_itr = step.begin (); sim_veh_itr != step.end (); sim_veh_itr++) {
				if (sim_veh_itr->link != index && sim_veh_itr->link >= 0 && sim_veh_itr->lane > -1) {
					index = sim_veh_itr->link;
#ifdef CHECK
					if (index < 0 || index > (int) sim->sim_dir_array.size ()) sim->Error (String ("Output_Step::Unlock Index=%d vs %d") % index % sim->sim_dir_array.size ());
#endif
					sim_dir_ptr = &sim->sim_dir_array [index];
#ifdef CHECK
					if (step.Process_ID () > 0 && sim_dir_ptr->Lock () != step.Process_ID ()) sim->Error (String ("Output_Step::Unlock (% vs %d), Index=%d, dir=%d") % sim_dir_ptr->Lock () % step.Process_ID () % index % sim_dir_ptr->Dir ());
#endif
					sim->sim_dir_array.UnLock (sim_dir_ptr, step.Process_ID ());
				}
			}
		}
#endif
	}

	//---- move to the next plan ----

	if (new_plan) {
		if (!no_veh_flag) sim_veh_ptr->Parked (true);


		step.sim_travel_ptr->Vehicle (0);
		step.sim_travel_ptr->Next_Load (-1);

		if (step.sim_travel_ptr->Person () > 0) {
			step.sim_travel_ptr->Status (NOT_ACTIVE);
			step.sim_travel_ptr->Next_Event (sim->time_step + sim_plan_ptr->Activity ());
		} else {
			step.sim_travel_ptr->Status (OFF_NET_END);
			step.sim_travel_ptr->Next_Event (sim->param.end_time_step);
		}
		if (!step.sim_travel_ptr->Next_Plan ()) return (false);

		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
		
		if (step.sim_plan_ptr->Start () > step.sim_travel_ptr->Next_Event ()) {
			step.sim_travel_ptr->Next_Event (step.sim_plan_ptr->Start ());
		}
		return (true);
	}
	if (no_veh_flag) {
		step.sim_travel_ptr->Status (OFF_NET_MOVE);
		return (true);
	} else {
		if (sim_veh_ptr->Parked ()) {
			step.sim_travel_ptr->Status (OFF_NET_MOVE);
		} else if (step.sim_travel_ptr->Status () == ON_OFF_DRIVE) {
			step.sim_travel_ptr->Status (OFF_NET_DRIVE);
		}
		return (!sim_veh_ptr->Parked ());
	}
}
