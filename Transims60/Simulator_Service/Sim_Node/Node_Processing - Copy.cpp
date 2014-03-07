//*********************************************************
//	Node_Processing.cpp - simulate links entering a node
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Node_Processing
//---------------------------------------------------------

bool Sim_Node_Process::Node_Processing (int node)
{
	int veh_cell, next_cell, cell, cells;
	bool next_flag;

	Int_Itr load_itr;
	Sim_Dir_Ptr sim_dir_ptr, new_dir_ptr;
	Sim_Lane_Ptr new_lane_ptr;
	Sim_Lane_Itr sim_lane_itr;
	Sim_Travel_Ptr sim_travel_ptr;
	Veh_Cell_Ptr veh_cell_ptr;
	Veh_Type_Data *veh_type_ptr;
	Travel_Step step;
	Cell_Data cell_rec;

	//sim_travel_ptr->Status ();		//---- if vehicle parked, rest the traveler status ----

	for (int index = sim->node_link [node]; index >= 0; index = sim->link_list [index]) {
		step.Dir_Index (index);
		step.sim_dir_ptr = sim_dir_ptr = &sim->sim_dir_array [index];

		for (sim_lane_itr = sim_dir_ptr->begin (); sim_lane_itr != sim_dir_ptr->end (); sim_lane_itr++) {
			step.sim_lane_ptr = &(*sim_lane_itr);

			for (veh_cell = sim_lane_itr->First_Veh (); veh_cell >= 0; veh_cell = next_cell) {

				step.veh_cell_ptr = veh_cell_ptr = &sim->veh_cell_array [veh_cell];
				step.veh_type_ptr = veh_type_ptr = &sim->veh_type_array [veh_cell_ptr->Type ()];
				cells = veh_type_ptr->Cells ();

//bool debug = (veh_cell == 5719);
//if (debug) exe->Write (1, " link=") << index << " cells=" << cells << " driver=" << veh_cell_ptr->Driver () << " time=" << sim->time_step;
				//---- current vehicle location ----

				cell_rec = veh_cell_ptr->Location ();

				step.assign (1, cell_rec);

				next_cell = veh_cell_ptr->Follower ();

				cell = veh_cell_ptr->Cell () + 1;

				if (cell < cells) {
					for (++veh_cell_ptr; cell < cells; cell++, veh_cell_ptr++) {
						if (veh_cell_ptr->Link () == cell_rec.link && veh_cell_ptr->Lane () == cell_rec.lane) {
							next_cell = veh_cell_ptr->Follower ();
						}
					}
					veh_cell_ptr = step.veh_cell_ptr;
				}

				if (veh_cell_ptr->Driver () < 0) {
//if (debug) exe->Write (1, " no driver cell=") << veh_cell << " num=" << veh_cell_ptr->Cell ();
					cell_rec.lane = -1;
					step.push_back (cell_rec);

					Move_Vehicle (step);

					//step.Problem (DRIVER_PROBLEM);
					step.Problem (TOTAL_PROBLEM);
					step.Status (3);

					Output_Step (step);
					continue;
				}

				step.Traveler (veh_cell_ptr->Driver ());
				step.sim_travel_ptr = sim_travel_ptr = &sim->sim_travel_array [veh_cell_ptr->Driver ()];
//if (veh_cell == 8379) exe->Write (0, " hhold=") << sim_travel_ptr->Household ();
				if (sim_travel_ptr->sim_plan_ptr == 0) {
//exe->Write (1, " no plan cell=") << veh_cell << " num=" << veh_cell_ptr->Cell () << " traveler=" << step.Traveler ();
					//step.Problem (PLAN_PROBLEM);
					step.Problem (PATH_PROBLEM);
					step.Status (3);

					Output_Step (step);
					continue;
				}

				if (sim_travel_ptr->Next_Event () > sim->time_step) {
					//if (veh_cell_ptr->Cell () == 0 && veh_cell_ptr->Lane () >= 0) {
					if (veh_cell_ptr->Cell () == 0) {
						num_vehicles++;
					}
					continue;
				}

				//---- get the current leg ----

				Sim_Leg_Itr leg_itr = sim_travel_ptr->sim_plan_ptr->begin ();

				if (leg_itr == sim_travel_ptr->sim_plan_ptr->end ()) {
					sim_travel_ptr->Next_Plan ();
					continue;
				}
				
				//---- check the arrival time constraint ----

				if ((sim_travel_ptr->sim_plan_ptr->End () + sim->param.max_end_variance) < sim->time_step) {
//if (debug) exe->Write (1, " arrive link=") << index << " cells=" << cells << " driver=" << veh_cell_ptr->Driver ();
					cell_rec.lane = -1;
					step.push_back (cell_rec);

					Move_Vehicle (step);
//sim->Write (1, "ARRIVAL traveler=") << step.Traveler ();

					step.Problem (ARRIVAL_PROBLEM);
					step.Status (3);

					Output_Step (step);

					sim_travel_ptr->Next_Plan ();
					continue;
				}

				//************ do something **********

				if (++leg_itr == sim_travel_ptr->sim_plan_ptr->end ()) {
					sim_travel_ptr->Next_Plan ();
					continue;
				}


				if (leg_itr->Type () == PARKING_ID) {

					cell_rec.lane = -1;

				} else {

					//---- check for space on the next link ----

					if (leg_itr->Type () != DIR_ID) {
//if (debug) sim->Write (1, "household=") << sim_travel_ptr->Household () << " trip=" << sim_travel_ptr->sim_plan_ptr->Tour () << "-" << sim_travel_ptr->sim_plan_ptr->Trip () << " type=" << leg_itr->Type () << " step=" << sim->time_step;
						continue;
					}
					cell_rec.link = leg_itr->Index ();
					cell_rec.lane = 0;
					cell_rec.offset = 0;

					new_dir_ptr = &sim->sim_dir_array [leg_itr->Index ()];

					//---- try the restricted lanes first ----

					next_flag = false;
					new_lane_ptr = new_dir_ptr->Lane_Ptr (1);

					if (new_lane_ptr != 0 && new_lane_ptr->Available_Space () >= cells) {
						if (Cell_Use (new_lane_ptr, 0, step)) {
							next_flag = true;
							cell_rec.lane = 1;
						}
					}

					//---- try the general purpose lanes ----

					if (!next_flag) {
						new_lane_ptr = new_dir_ptr->Lane (0);

						if (new_lane_ptr->Available_Space () >= cells) {
							if (Cell_Use (new_lane_ptr, 0, step)) {
								next_flag = true;
								cell_rec.lane = 0;
							}
						}
					}
					if (!next_flag) continue;

					cell_rec.leader = new_lane_ptr->Last_Veh ();

					sim_lane_itr->Add_Volume (-cells);
					new_lane_ptr->Add_Volume (cells);
				}
				step.push_back (cell_rec);

				Move_Vehicle (step);

				//---- move to the next leg ----

				sim_travel_ptr->Next_Event (sim->time_step + leg_itr->Time ());

				sim_travel_ptr->sim_plan_ptr->Next_Leg ();
			}
		}

		//---- update the load list ----

		if (sim_dir_ptr->load_queue.size () > 0) {
			Integers reload;

			for (load_itr = sim_dir_ptr->load_queue.begin (); load_itr != sim_dir_ptr->load_queue.end (); load_itr++) {
				if (!Load_Vehicle (*load_itr)) {
					reload.push_back (*load_itr);
					num_waiting++;			//---- number of vehicles still in the loading queue ----
				} else {
					num_vehicles++;
				}
			}
			sim_dir_ptr->load_queue.swap (reload);
		}
	}
	return (node > 0);
}
