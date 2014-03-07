//*********************************************************
//	Travel_Update.cpp - update traveler status
//*********************************************************

#include "Sim_Travel_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Travel_Update
//---------------------------------------------------------

bool Sim_Travel_Process::Travel_Update (Travel_Step &step)
{
	int vehicle;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Veh_Ptr sim_veh_ptr;

	//---- free the vehicle ----

	vehicle = step.sim_travel_ptr->Vehicle ();

	if (vehicle >= 0) {
		sim_veh_ptr = &sim->sim_veh_array [vehicle];
		sim_veh_ptr->Parked (true);
	}

	//---- move to the next plan ----

	sim_plan_ptr = step.sim_travel_ptr->Next_Plan ();

	if (sim_plan_ptr == 0) {
		step.sim_travel_ptr->Next_Event (sim->param.end_time_step);
		step.sim_travel_ptr->Status (1);
		step.sim_travel_ptr->Next_Event (-1);
		return (false);
	} else {
		if (sim_plan_ptr->Start () > step.sim_travel_ptr->Next_Event ()) {
			step.sim_travel_ptr->Next_Event (sim_plan_ptr->Start ());
		}
		step.sim_travel_ptr->Status (0);
		return (true);
	}

	//int cell_offset, index, cell, lane, to_index, to_cell, to_lane, last_index, pce;
	//int first_cell, last_cell, to_part, from_index;
	//bool reload_flag, bound_flag, remove_flag = false;
	//bool transfer_flag = false;

	//Sim_Plan_Ptr plan_ptr;
	//Sim_Leg_Itr leg_itr;
	//Sim_Dir_Ptr sim_dir_ptr;
	//Sim_Cap_Ptr sim_cap_ptr = 0;
	//Sim_Veh_Ptr sim_veh_ptr;
	//Cell_Itr cell_itr, veh_cell_itr, end_itr;
	//Problem_Data problem_data;

	//if (step.Vehicle () >= 0 && step.sim_veh_ptr == 0) {
	//	step.sim_veh_ptr = sim->sim_veh_array [step.Vehicle ()];
	//}
	//plan_ptr = step.sim_traveler_ptr->plan_ptr;

	////---- check for lost problems ----

	//if (step.Status () == 1 || step.Problem () == DEPARTURE_PROBLEM || step.Problem () == ARRIVAL_PROBLEM) {
	//	if (plan_ptr->Person () == 0) {
	//		stats.num_run_lost++;
	//	} else {
	//		stats.num_veh_lost++;

	//		//---- check the lost vehicle event ----

	//		if (step.sim_veh_ptr != 0) {
	//			if (sim->Check_Event (VEH_LOST_EVENT, plan_ptr->Mode (), subarea)) {
	//				Event_Data event_data;

	//				event_data.Household (plan_ptr->Household ());
	//				event_data.Person (plan_ptr->Person ());
	//				event_data.Tour (plan_ptr->Tour ());
	//				event_data.Trip (plan_ptr->Trip ());
	//				event_data.Mode (plan_ptr->Mode ());

	//				if (step.Problem () == ARRIVAL_PROBLEM) {
	//					event_data.Schedule (plan_ptr->Arrive ());
	//				} else if (step.Problem () == DEPARTURE_PROBLEM) {
	//					event_data.Schedule (plan_ptr->Depart ());
	//				} else {
	//					event_data.Schedule (plan_ptr->Schedule ());
	//				}
	//				event_data.Actual (sim->step);
	//				event_data.Event (VEH_LOST_EVENT);

	//				veh_cell_itr = step.sim_veh_ptr->begin ();
	//				veh_cell_itr->Location (&to_index, &to_lane, &to_cell);

	//				event_data.Dir_Index (to_index);

	//				if (to_index >= 0 || to_cell > 0) {
	//					cell_offset = to_cell * param.cell_size + (param.cell_size >> 1);

	//					event_data.Offset (UnRound (cell_offset));
	//				} else {
	//					event_data.Offset (0);
	//				}
	//				event_data.Lane (to_lane);
	//				event_data.Route (-1);

	//				sim->Output_Event (event_data);
	//			}
	//		}
	//	}
	//}

	////---- process the problem message ----

	//if (step.Problem () > 0) {
	//	if (step.Status () == 1 || step.Problem () == DEPARTURE_PROBLEM || step.Problem () == ARRIVAL_PROBLEM || 
	//		param.count_warnings || (sim->select_problems && sim->problem_range.In_Range (step.Problem ()))) {

	//		step.sim_traveler_ptr->Problem (1);
	//		sim->Set_Problem ((Problem_Type) step.Problem ());
	//	}
	//	if (!sim->select_problems || sim->problem_range.In_Range (step.Problem ())) {
	//		if (param.problem_flag && step.sim_traveler_ptr->Active ()) {
	//			if ((int) step.size () > 0) {
	//				cell_itr = --step.end ();
	//				cell_itr->Location (&to_index, &to_lane, &to_cell);
	//			} else if (step.sim_veh_ptr != 0) {
	//				veh_cell_itr = step.sim_veh_ptr->begin ();
	//				veh_cell_itr->Location (&to_index, &to_lane, &to_cell);
	//			} else {
	//				to_index = step.Dir_Index ();
	//				to_lane = to_cell = 0;
	//			}
	//			problem_data.Household (plan_ptr->Household ());
	//			problem_data.Person (plan_ptr->Person ());
	//			problem_data.Tour (plan_ptr->Tour ());
	//			problem_data.Trip (plan_ptr->Trip ());

	//			problem_data.Start (plan_ptr->Depart ());
	//			problem_data.End (plan_ptr->Arrive ());
	//			problem_data.Duration (plan_ptr->Duration ());

	//			problem_data.Origin (plan_ptr->Origin ());
	//			problem_data.Destination (plan_ptr->Destination ());

	//			problem_data.Purpose (plan_ptr->Purpose ());
	//			problem_data.Mode (plan_ptr->Mode ());
	//			problem_data.Constraint (plan_ptr->Constraint ());
	//			problem_data.Priority (plan_ptr->Priority ());

	//			if (step.Vehicle () >= 0) {
	//				problem_data.Vehicle (sim->vehicle_array [step.Vehicle ()].Vehicle ());
	//			} else {
	//				problem_data.Vehicle (0);
	//			}
	//			problem_data.Passengers (plan_ptr->Passengers ());
	//			problem_data.Problem (step.Problem ());
	//			problem_data.Time (sim->step);

	//			problem_data.Dir_Index (to_index);

	//			if (to_index >= 0 || to_cell > 0) {
	//				cell_offset = to_cell * param.cell_size + (param.cell_size >> 1);

	//				problem_data.Offset (UnRound (cell_offset));
	//			} else {
	//				problem_data.Offset (0);
	//			}
	//			problem_data.Lane (to_lane);
	//			problem_data.Route (-1);

	//			sim->Output_Problem (problem_data);
	//		}
	//	}
	//}
	//if (!step.sim_traveler_ptr->Active ()) {
	//	sim->veh_status [step.Vehicle ()] = 3;
	//	return (false);
	//}

}
