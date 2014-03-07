//*********************************************************
//	Load_Vehicle.cpp - attempt to load a vehicle to the link
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Load_Vehicle
//---------------------------------------------------------

bool Sim_Node_Process::Load_Vehicle (int traveler)
{
	int lane, length, subarea, load_cell, lead_cell, dir_index, cells, start_offset, end_offset;
	bool loaded, type_flag, bound_flag, side_flag;
	Dtime time;
	double flow, speed;
	
	Travel_Step step;

	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Itr leg_itr, next_leg, check_leg;
	Veh_Cell_Ptr load_cell_ptr, lead_cell_ptr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Park_Ptr sim_park_ptr;
	Sim_Stop_Ptr sim_stop_ptr;
	Veh_Type_Data *veh_type_ptr;
	Cell_Data cell_rec;
	Sim_Lane_Ptr sim_lane_ptr;

	subarea = 0;

	//---- build the travel step ----

	step.Traveler (traveler);
	sim_travel_ptr = &sim->sim_travel_array [traveler];
	sim_plan_ptr = sim_travel_ptr->sim_plan_ptr;

	if (sim_plan_ptr == 0) return (false);

	step.sim_travel_ptr = sim_travel_ptr;

	leg_itr = sim_plan_ptr->begin ();
	if (leg_itr == sim_plan_ptr->end ()) return (false);

	next_leg = leg_itr + 1;
	if (next_leg == sim_plan_ptr->end ()) return (false);

	if (next_leg->Type () != DIR_ID) {
		step.Problem (ACCESS_PROBLEM);
		step.Status (2);

		Output_Step (step);
		return (false);
	}
	dir_index = next_leg->Index ();
	step.sim_dir_ptr = sim_dir_ptr = &sim->sim_dir_array [dir_index];
	step.Dir_Index (dir_index);

	//---- locate the load point ----

	type_flag = (sim_travel_ptr->Status () != -2);

	if (type_flag) {

		//---- external link load ----

		start_offset = 0;
		bound_flag = true;
		side_flag = (sim_travel_ptr->random.Probability () >= 0.5);

	} else {

		//---- transit loading ----

		if (sim_travel_ptr->Person () == 0) {
			if (leg_itr->Type () != STOP_ID) {
				step.Problem (ACCESS_PROBLEM);
				step.Status (2);
				return (false);
			}
			sim_stop_ptr = &sim->sim_stop_array [leg_itr->Index ()];

			bound_flag = false;
			side_flag = true;
			start_offset = sim_stop_ptr->Offset ();

		} else {	//---- parking lot load ----

			if (leg_itr->Type () != PARKING_ID) {
				step.Problem (ACCESS_PROBLEM);
				step.Status (2);

				Output_Step (step);
				return (false);
			}
			sim_park_ptr = &sim->sim_park_array [leg_itr->Index ()];

			bound_flag = (sim_park_ptr->Type () == BOUNDARY);

			if (bound_flag) {
				side_flag = (sim_travel_ptr->random.Probability () >= 0.5);
			} else {
				side_flag = (sim_park_ptr->Dir () == sim_dir_ptr->Dir ());
			}
			if (sim_dir_ptr->Dir () == 0) {
				start_offset = sim_park_ptr->Offset_AB ();
			} else {
				start_offset = sim_park_ptr->Offset_BA ();
			}
		}
	}

	//---- set vehicle type attributes ----
	
	step.veh_type_ptr = veh_type_ptr = &sim->veh_type_array [sim_plan_ptr->Veh_Type ()];

	if (bound_flag) {
		sim_travel_ptr->Speed (veh_type_ptr->Max_Speed ());
	} else {
		sim_travel_ptr->Speed (veh_type_ptr->Max_Accel ());
	}
	if (sim_travel_ptr->Speed () > sim_dir_ptr->Speed ()) {
		sim_travel_ptr->Speed (sim_dir_ptr->Speed ());
	}
	cells = (int) veh_type_ptr->Cells ();

	//---- set the access point ----

	cell_rec.Location (dir_index, -1, start_offset, -1);
	step.push_back (cell_rec);
	loaded = false;
	lane = 0;

	load_cell = sim_travel_ptr->First_Cell ();
	step.veh_cell_ptr = load_cell_ptr = &sim->veh_cell_array [load_cell];

	//---- macroscopic loading ----

	if (sim_dir_ptr->Method () == MACROSCOPIC) {

		//---- try the restricted lanes first ----

		sim_lane_ptr = sim_dir_ptr->Lane_Ptr (1);

		if (sim_lane_ptr != 0 && sim_lane_ptr->Available_Space () >= cells) {
			if (Cell_Use (sim_lane_ptr, start_offset, step)) {
				loaded = true;
				cell_rec.lane = 1;
			}
		}

		//---- try the general purpose lanes ----

		if (!loaded) {
			sim_lane_ptr = sim_dir_ptr->Lane (0);

			if (sim_lane_ptr->Available_Space () >= cells) {
				if (Cell_Use (sim_lane_ptr, start_offset, step)) {
					loaded = true;
					cell_rec.lane = 0;
				}
			}
		}

		//---- load the vehicle ----

		if (loaded) {

			end_offset = sim_dir_ptr->Length ();

			//---- check for an event on the loading link ----

			check_leg = next_leg + 1;

			if (check_leg != sim_plan_ptr->end ()) {

				if (check_leg->Type () == STOP_ID) {

					sim_stop_ptr = &sim->sim_stop_array [check_leg->Index ()];

					end_offset = sim_stop_ptr->Offset ();

				} else if (check_leg->Type () == PARKING_ID) {	

					sim_park_ptr = &sim->sim_park_array [check_leg->Index ()];

					if (sim_dir_ptr->Dir () == 0) {
						end_offset = sim_park_ptr->Offset_AB ();
					} else {
						end_offset = sim_park_ptr->Offset_BA ();
					}
				}
			}
			length = end_offset - start_offset;

			//---- calculate the travel time to the end of the link ----

			time = sim->time_step;
			Dir_Data *ptr = &sim->dir_array [dir_index];

			//################## method ? #####################

			flow = sim_dir_ptr->Max_Flow () - pow ((sim_lane_ptr->Volume () - sim_dir_ptr->Density ()), 2);
			speed = flow / sim_dir_ptr->Density ();

			if (speed > 0.0) {
				time = length / speed;
			} else {
				time = length;
			}
//if (traveler == 4931) exe->Write (1, "time=") << time << " vs " << ptr->Time0 () << " speed=" << speed << " flow=" << flow << " volume=" << sim_lane_ptr->Volume () << " density=" << sim_dir_ptr->Density () << " max=" << sim_dir_ptr->Max_Flow ();
			time = sim->time_step + ptr->Time0 () * length / sim_dir_ptr->Length ();

			sim_travel_ptr->Next_Event (time);

			//---- find the insert point ----

			lead_cell = sim_lane_ptr->First_Veh (); 
			cell_rec.leader = lead_cell;

			for (; lead_cell >= 0; lead_cell = lead_cell_ptr->Follower ()) {
				cell_rec.leader = lead_cell;

				//---- find the insert position ----

				lead_cell_ptr = &sim->veh_cell_array [lead_cell];

//if (lead_cell_ptr->Driver () < 0) {
//	sim->Write (1, "lead_cell=") << lead_cell << " cell=" << lead_cell_ptr->Cell ();
//	break;
//}
				sim_travel_ptr = &sim->sim_travel_array [lead_cell_ptr->Driver ()];

				if (sim_travel_ptr->Next_Event () > time) {
					cell_rec.leader = lead_cell_ptr->Leader ();
					break;
				}
			}
			sim_lane_ptr->Add_Volume (cells);
		}

		//---- add to the priority list ----

		//if (!loaded && step.sim_veh_ptr->Leader () < 0 && step.sim_veh_ptr->Priority () > 0) {
		//	priority_list.push_back (step.Vehicle ());
		//}

	} else {		//---- mescoscopic loading ----

		//---- load from the right ----

		if (side_flag) {

			//---- try to load onto the best lanes ----

			for (lane = next_leg->In_Best_High (); lane >= next_leg->In_Best_Low (); lane--) {
				cell_rec.lane = lane;

				loaded = Check_Behind (cell_rec, step);
				if (loaded) break;
			}
			if (!loaded) {

				//---- try to load on the permitted lanes ----

				for (lane = next_leg->In_Lane_High (); lane > next_leg->In_Best_High (); lane--) {
					cell_rec.lane = lane;
					loaded = Check_Behind (cell_rec, step);
					if (loaded) break;
				}
				if (!loaded) {
					for (lane = next_leg->In_Best_Low ()-1; lane >= next_leg->In_Lane_Low (); lane--) {
						cell_rec.lane = lane;
						loaded = Check_Behind (cell_rec, step);
						if (loaded) break;
					}
				}
			}

			//---- start a priority load ----

			//if (!loaded && /*step.sim_veh_ptr->Leader () < 0 && */ step.sim_veh_ptr->Priority () > 0) {
			if (!loaded) {

				//---- try to reserve a cell onto the best lanes ----

				for (lane = next_leg->In_Best_High (); lane >= next_leg->In_Best_Low (); lane--) {
					cell_rec.lane = lane;
					//if (Reserve_Cell (cell_rec, step)) break;
				}
				//if (/*step.sim_veh_ptr->Leader () < 0 &&*/ step.sim_veh_ptr->Priority () > 1) {

				//	//---- try to reserve a cell on the permitted lanes ----

				//	for (lane = next_leg->In_Lane_High (); lane > next_leg->In_Best_High (); lane--) {
				//		cell_rec.Lane (lane);
				//		if (Reserve_Cell (cell_rec, step)) break;
				//	}
				//	if (/*step.sim_veh_ptr->Leader () < 0 && */step.sim_veh_ptr->Priority () > 1) {
				//		for (lane = next_leg->In_Best_Low ()-1; lane >= next_leg->In_Lane_Low (); lane--) {
				//			cell_rec.Lane (lane);
				//			if (Reserve_Cell (cell_rec, step)) break;
				//		}
				//	}
				//}
				//priority_list.push_back (step.Vehicle ());
			}

		} else {	//---- load from the left ----

			//---- try to load onto the best lanes ----

			for (lane = next_leg->In_Best_Low (); lane <= next_leg->In_Best_High (); lane++) {
				cell_rec.lane = lane;
				loaded = Check_Behind (cell_rec, step);
				if (loaded) break;
			}
			if (!loaded) {

				//---- try to load on the permitted lanes ----

				for (lane = next_leg->In_Lane_Low (); lane < next_leg->In_Best_Low (); lane++) {
					cell_rec.lane = lane;
					loaded = Check_Behind (cell_rec, step);
					if (loaded) break;
				}
				if (!loaded) {
					for (lane = next_leg->In_Best_High ()+1; lane <= next_leg->In_Lane_High (); lane++) {
						cell_rec.lane = lane;
						loaded = Check_Behind (cell_rec, step);
						if (loaded) break;
					}
				}
			}

			//---- start a priority load ----

			//if (!loaded &&/* step.sim_veh_ptr->Leader () < 0 &&*/ step.sim_veh_ptr->Priority () > 0) {

			//	//---- try to reserve a cell onto the best lanes ----

			//	for (lane = next_leg->In_Best_Low (); lane <= next_leg->In_Best_High (); lane++) {
			//		cell_rec.Lane (lane);
			//		if (Reserve_Cell (cell_rec, step)) break;
			//	}
			//	if (/*step.sim_veh_ptr->Leader () < 0 && */step.sim_veh_ptr->Priority () > 1) {

			//		//---- try to reserve a cell on the permitted lanes ----

			//		for (lane = next_leg->In_Lane_Low (); lane < next_leg->In_Best_Low (); lane++) {
			//			cell_rec.Lane (lane);
			//			if (Reserve_Cell (cell_rec, step)) break;
			//		}
			//		if (/*step.sim_veh_ptr->Leader () < 0 &&*/ step.sim_veh_ptr->Priority () > 1) {
			//			for (lane = next_leg->In_Best_High ()+1; lane <= next_leg->In_Lane_High (); lane++) {
			//				cell_rec.Lane (lane);
			//				if (Reserve_Cell (cell_rec, step)) break;
			//			}
			//		}
			//	}
			//	priority_list.push_back (step.Vehicle ());
			//}
		}
	}

	//---- process the result ----

	if (!loaded) return (false);

	sim_plan_ptr->Next_Leg ();

	step.push_back (cell_rec);

	Move_Vehicle (step);

	//---- put the vehicle on the network ----

	if (step.sim_travel_ptr->Status () != -4) {
		if (sim_travel_ptr->Person () == 0) {
			stats.num_run_start++;
		} else {
			stats.num_veh_start++;

			//---- output event record ----

			if (sim->event_output.In_Range (VEH_START_EVENT, sim_plan_ptr->Mode (), subarea)) {
				Event_Data event_data;

				event_data.Household (sim_travel_ptr->Household ());
				event_data.Person (sim_travel_ptr->Person ());
				event_data.Tour (sim_plan_ptr->Tour ());
				event_data.Trip (sim_plan_ptr->Trip ());
				event_data.Mode (sim_plan_ptr->Mode ());
				event_data.Schedule (sim_plan_ptr->Start ());
				event_data.Actual (sim->time_step);
				event_data.Event (VEH_START_EVENT);
				event_data.Dir_Index (dir_index);
				event_data.Offset (start_offset);
				event_data.Lane (lane);
				event_data.Route (-1);

				sim->event_output.Output_Event (event_data);
			}
		}
	}
	step.sim_travel_ptr->Status (1);
	return (true);
}
