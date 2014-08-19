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
//sim->debug = (step.sim_travel_ptr->Traveler () == 8517);
//if (sim->debug) sim->Write (1, " output problem=") << step.Problem ();

	//---- mark the traveler as processed in this time step ----

	step.sim_travel_ptr->Step_Code (sim->Step_Code ());

	if (step.sim_plan_ptr == 0) {
		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
	}
	sim_plan_ptr = step.sim_plan_ptr;

	new_plan = false;

	if (sim_plan_ptr == 0) {
		sim->Warning ("Zero Plan Pointer for Traveler=") << step.sim_travel_ptr->Traveler () << " problem=" << step.Problem () << " status=" << step.sim_travel_ptr->Status ();

		//---- try to remove the vehicle from the network ----

		if (step.sim_dir_ptr != 0) {
			for (Int_Itr itr = step.sim_dir_ptr->begin (); itr != step.sim_dir_ptr->end (); itr++) {
				if (*itr == step.Traveler ()) {
					*itr = 0;
				}
			}
		} else if (step.sim_travel_ptr->Vehicle () >= 0) {
			sim_veh_ptr = &sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];

			if (sim_veh_ptr->link >= 0) {
				sim_dir_ptr = &sim->sim_dir_array [sim_veh_ptr->link];
				int cell = sim_veh_ptr->offset / sim->param.cell_size;
				if (cell > sim_dir_ptr->Max_Cell ()) {
					cell = sim_dir_ptr->Max_Cell ();
				}
				if (sim_dir_ptr->Get (sim_veh_ptr->lane, cell) == step.Traveler ()) {
					sim_dir_ptr->Clear (sim_veh_ptr->lane, cell);
				}
			}
		}
		return (false);
	}

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
					sim->sim_output_step.Event_Check (VEH_LOST_EVENT, step);
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
		if (step.sim_travel_ptr->Vehicle () < 0) return (false);
	
		step.sim_veh_ptr = &sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];
	}
	sim_veh_ptr = step.sim_veh_ptr;

//if (sim->debug) sim->Write (0, " park=") << sim_veh_ptr->Parked () << " link=" << sim_veh_ptr->link;
//if (sim->debug) step.sim_veh_ptr = sim_veh_ptr = &sim->sim_veh_array [step.sim_travel_ptr->Vehicle ()];
//if (sim->debug) sim->Write (0, " vs ") << sim_veh_ptr->link;

	if ((new_plan || sim_veh_ptr->Parked ()) && sim_veh_ptr->link >= 0) {
		sim_dir_ptr = &sim->sim_dir_array [sim_veh_ptr->link];
		int index = sim_dir_ptr->Index (sim_veh_ptr->lane, sim->Offset_Cell (sim_veh_ptr->offset));

		if (sim_dir_ptr->Get (index) == step.Traveler ()) {
			sim_dir_ptr->Remove (index);
		}
	}

	//---- move to the next plan ----

//if (sim->debug) sim->Write (0, " new_plan=") << new_plan;
	if (new_plan) {
		sim_veh_ptr->Parked (true);
		step.sim_travel_ptr->Vehicle (0);

		step.sim_travel_ptr->Status (NOT_ACTIVE);

		if (!step.sim_travel_ptr->Next_Plan ()) {
			step.sim_travel_ptr->Next_Event (sim->param.end_time_step);
			return (false);
		}
		step.sim_plan_ptr = step.sim_travel_ptr->Get_Plan ();
		
		if (step.sim_plan_ptr->Start () > step.sim_travel_ptr->Next_Event ()) {
			step.sim_travel_ptr->Next_Event (step.sim_plan_ptr->Start ());
		}
		return (true);
	}
	//if (step.Status () > 0) return (false);

	if (sim_veh_ptr->Parked ()) {
		step.sim_travel_ptr->Status (OFF_NET_MOVE);
	}

	return (!sim_veh_ptr->Parked ());
}

		////---- update the vehicle status ----

		//step.sim_travel_ptr->Speed (step.Veh_Speed ());
		//				
		//from_index = last_index = step.veh_cell_ptr->Link ();

		//sim_dir_ptr = &sim->sim_dir_array [last_index];

		//for (cell_itr = step.begin (); cell_itr != step.end (); cell_itr++) {
		//	cell_itr->Location (&to_index, &to_lane, &to_offset);

		//	//---- check for a link change ----

		//	if (to_index != last_index) {
		//		sim_plan_ptr->Next_Leg ();
		//		last_index = to_index;
		//	}

		//	//---- move each vehicle cell ----

		//	for (veh_cell_itr = step.sim_veh_ptr->begin (); veh_cell_itr != end_itr; veh_cell_itr++) {
		//		veh_cell_itr->Location (&index, &lane, &cell);

		//		if (index >= 0 && lane >= 0) {
		//			if (to_index == index && to_lane == lane && to_offset == cell) break;
		//			sim->sim_dir_array.Cell (index, lane, cell, -1);
		//		}
		//		veh_cell_itr->Location (to_index, to_lane, to_offset);

		//		if (to_index >= 0 && to_lane >= 0) {

		//			sim->sim_dir_array.Cell (to_index, to_lane, to_cell, step.Vehicle ());
		//		}
		//		to_index = index;
		//		to_lane = lane;
		//		to_offset = cell;
		//	}
		//}

		////---- check the partition status ----

		//sim_dir_ptr = &sim->sim_dir_array [last_index];
		//front = step.sim_veh_ptr->front ();

		//to_part = sim_dir_ptr->Partition (front.Cell ());

		////---- output traveler record ----

		//if (sim->traveler_output.Output_Flag ()) {
		//	Traveler_Data traveler_data;

		//	traveler_data.Household (step.sim_travel_ptr->Household ());
		//	traveler_data.Person (step.sim_travel_ptr->Person ());
		//	traveler_data.Time (sim->time_step);
		//	traveler_data.Mode (sim_plan_ptr->Mode ());

		//	if (sim->traveler_output.In_Range (traveler_data)) {
		//		traveler_data.Tour (sim_plan_ptr->Tour ());
		//		traveler_data.Trip (sim_plan_ptr->Trip ());
		//		traveler_data.Distance ((int) step.size () * sim->param.cell_size);
		//		traveler_data.Speed (step.Veh_Speed () * sim->param.cell_size);
		//		traveler_data.Dir_Index (front.Index ());
		//		traveler_data.Lane (front.Lane ());
		//		traveler_data.Offset ((front.Cell () + 1) * sim->param.cell_size);

		//		sim->traveler_output.Output_Traveler (traveler_data);
		//	}
		//}

		//if (bound_flag && to_part != partition) {
		//	transfer_flag = true;
		//}
		//if (sim_dir_ptr->Boundary ()) {

		//	last_cell = sim_dir_ptr->Split_Cell ();
		//	first_cell = last_cell - sim_dir_ptr->Speed ();

		//	cell = front.Cell ();

		//	if (cell >= first_cell && cell < last_cell) {
		//		boundary.push_back (step.Vehicle ());
		//	} else if (cell >= last_cell) {
		//		if (to_part != partition) {
		//			transfer_flag = true;
		//		} else if ((cell - last_cell) < step.sim_veh_ptr->Num_Cells ()) {
		//			first_list.push_back (step.Vehicle ());
		//		}
		//	}
		//}

		////---- process a transit stop ----

		//if (step.Stop () >= 0) {
		//	leg_itr = sim_plan_ptr->begin ();
		//	step.sim_veh_ptr->Next_Event (sim->time_step + leg_itr->Time ());
		//	sim_plan_ptr->Next_Leg ();
		//	if (sim_plan_ptr->size () == 0) {
		//		stats.num_run_end++;
		//		step.Status (1);
		//		remove_flag = true;
		//	}
		//}

		////---- park the vehicle -----

		//if (step.Parking () >= 0 || step.Status () > 0) {

		//	//---- remove the vehicle from the network ----

		//	sim_dir_ptr = &sim->sim_dir_array [last_index];

		//	if (step.sim_travel_ptr->Status () >= 0) {
		//		for (veh_cell_itr = step.sim_veh_ptr->begin (); veh_cell_itr != end_itr; veh_cell_itr++) {
		//			veh_cell_itr->Location (&index, &lane, &cell);

		//			if (index >= 0 && lane >= 0) {
		//				sim->sim_dir_array.Cell (index, lane, cell, -1);
		//			}
		//		}
		//		if (step.Status () > 0 && sim_dir_ptr->Method () == MACROSCOPIC) {
		//			sim_cap_ptr = sim_dir_ptr->Sim_Cap ();

		//			//---- remove the vehicle from the current link ----

		//			pce = (int) step.sim_veh_ptr->size ();

		//			if (step.sim_veh_ptr->Restricted ()) {
		//				sim_cap_ptr->Add_High (-pce);
		//			} else {
		//				sim_cap_ptr->Add_Low (-pce);
		//			}
		//			step.Exit_Link (pce);

		//			//---- update the vehicle list ----

		//			if (step.sim_veh_ptr->Follower () >= 0) {
		//				sim_veh_ptr = sim->sim_veh_array [step.sim_veh_ptr->Follower ()];
		//				if (sim_veh_ptr != 0) sim_veh_ptr->Leader (step.sim_veh_ptr->Leader ());
		//			} else {
		//				sim_cap_ptr->Last_Veh (step.sim_veh_ptr->Leader ());
		//			}
		//			if (step.sim_veh_ptr->Leader () >= 0) {
		//				sim_veh_ptr = sim->sim_veh_array [step.sim_veh_ptr->Leader ()];
		//				if (sim_veh_ptr != 0) sim_veh_ptr->Follower (step.sim_veh_ptr->Follower ());
		//			} else {
		//				sim_cap_ptr->First_Veh (step.sim_veh_ptr->Follower ());
		//			}
		//		}
		//	}
		//	if (sim_dir_ptr->Method () == MESOSCOPIC) {
		//		if (step.sim_veh_ptr->Follower () >= 0) {
		//			sim_veh_ptr = sim->sim_veh_array [step.sim_veh_ptr->Follower ()];
		//			if (sim_veh_ptr != 0) {
		//				sim_veh_ptr->Leader (-1);
		//				sim_veh_ptr->Next_Event (0);
		//			}
		//		}
		//		if (step.sim_veh_ptr->Leader () >= 0) {
		//			sim_veh_ptr = sim->sim_veh_array [step.sim_veh_ptr->Leader ()];
		//			if (sim_veh_ptr != 0) sim_veh_ptr->Follower (-1);
		//		}
		//	}
		//	
		//	//---- move the vehicle to the destination parking lot ----

		//	reload_flag = false;

		//	if (step.Parking () >= 0) {
		//		step.sim_travel_ptr->Status (2);

		//		stats.num_veh_end++;
		//		stats.tot_hours += sim->time_step - sim_plan_ptr->Start ();

		//		sim->vehicle_array [step.Vehicle ()].Parking (step.Parking ());

		//		//---- output event record ----

		//		if (sim->event_output.In_Range (VEH_END_EVENT, sim_plan_ptr->Mode (), subarea)) {
		//			Event_Data event_data;

		//			event_data.Household (step.sim_travel_ptr->Household ());
		//			event_data.Person (step.sim_travel_ptr->Person ());
		//			event_data.Tour (sim_plan_ptr->Tour ());
		//			event_data.Trip (sim_plan_ptr->Trip ());
		//			event_data.Mode (sim_plan_ptr->Mode ());
		//			event_data.Schedule (sim_plan_ptr->End ());
		//			event_data.Actual (sim->time_step);
		//			event_data.Event (VEH_END_EVENT);

		//			event_data.Dir_Index (front.Index ());
		//			event_data.Lane (front.Lane ());
		//			event_data.Offset (UnRound (front.Cell () * sim->param.cell_size + (sim->param.cell_size >> 1)));

		//			event_data.Route (-1);

		//			sim->event_output.Output_Event (event_data);
		//		}
		//		leg_itr = sim_plan_ptr->begin ();
		//		sim_plan_ptr->Next_Event (sim->time_step + leg_itr->Time ());
		//		sim_plan_ptr->Next_Leg ();
		//		remove_flag = true;

		//	} else if (step.sim_travel_ptr->Active ()) {

		//		//---- check the reload option ----

		//		if (step.Status () == 1 && sim->param.reload_problems) {
		//			sim_plan_ptr->Next_Leg ();
		//			leg_itr = sim_plan_ptr->begin ();

		//			if (leg_itr != sim_plan_ptr->end () && leg_itr->Type () == DIR_ID) {
		//				reload_flag = true;
		//				step.Status (0);
		//				step.sim_travel_ptr->Status (-4);

		//				to_index = leg_itr->Index ();

		//				for (veh_cell_itr = step.sim_veh_ptr->begin (); veh_cell_itr != end_itr; veh_cell_itr++) {
		//					veh_cell_itr->Location (to_index, -1, 0);
		//				}
		//				last_index = to_index;
		//				sim_dir_ptr = &sim->sim_dir_array [last_index];
		//				if (sim_dir_ptr->From_Part () != partition) {
		//					remove_flag = true;
		//					to_part = sim_dir_ptr->From_Part ();
		//				}
		//			}
		//		}

		//		//---- move the vehicle to the destination parking lot ----

		//		if (!reload_flag) {
		//			for (leg_itr = sim_plan_ptr->begin (); leg_itr != sim_plan_ptr->end (); leg_itr++) {
		//				if (leg_itr->Type () == PARKING_ID) {
		//					if (sim_plan_ptr->Next_Event () < 0) {
		//						sim_plan_ptr->Next_Event (0);
		//					} else {
		//						sim->vehicle_array [step.Vehicle ()].Parking (leg_itr->Index ());
		//						break;
		//					}
		//				}
		//			}
		//			step.sim_travel_ptr->Delete ();
		//		}
		//	}

		//	//---- delete the vehicle ----

		//	if (!reload_flag) {
		//		delete step.sim_veh_ptr;
		//		sim->sim_veh_array [step.Vehicle ()] = step.sim_veh_ptr = 0;

		//		sim->veh_status [step.Vehicle ()] = 3;
		//	} else {

		//		step.sim_veh_ptr->Leader (-1);
		//		step.sim_veh_ptr->Follower (-1);
		//		step.sim_veh_ptr->Wait (0);

		//		sim->veh_status [step.Vehicle ()] = (remove_flag) ? 3 : ((step.size () > 0) ? 2 : 1);
		//	}
		//} else {
		//	sim->veh_status [step.Vehicle ()] = (transfer_flag) ? 3 : ((step.size () > 0) ? 2 : 1);
		//}
		//if (remove_flag || transfer_flag) {
		//	sim_dir_ptr = &sim->sim_dir_array [last_index];

		//	sim_plan_ptr->Partition (to_part);
		//	sim_plan_ptr->Next_Event (0);

		//	transfers->push_back (Int2_Key (step.Traveler (), plan_ptr->Partition ()));

		//	//---- remove links between subareas ----

		//	if (step.sim_veh_ptr != 0 && sim_dir_ptr->Method () == MESOSCOPIC) {

		//		if (step.sim_veh_ptr->Follower () >= 0) {
		//			sim_veh_ptr = sim->sim_veh_array [step.sim_veh_ptr->Follower ()];
		//			if (sim_veh_ptr != 0) sim_veh_ptr->Leader (-1);
		//			step.sim_veh_ptr->Follower (-1);
		//		}
		//		if (step.sim_veh_ptr->Leader () >= 0) {
		//			sim_veh_ptr = sim->sim_veh_array [step.sim_veh_ptr->Leader ()];
		//			if (sim_veh_ptr != 0) sim_veh_ptr->Follower (-1);
		//			step.sim_veh_ptr->Leader (-1);
		//		}
		//	}
		//}
