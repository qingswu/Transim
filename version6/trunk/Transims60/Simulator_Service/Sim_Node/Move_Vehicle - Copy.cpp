//*********************************************************
//	Move_Vehicle.cpp - adjust the vehicle position and references
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Move_Vehicle
//---------------------------------------------------------

bool Sim_Node_Process::Move_Vehicle (Travel_Step &step)
{
	int cell, cells, veh_cell, lead_cell, next_cell;

	Veh_Cell_Ptr veh_cell_ptr, lead_cell_ptr, next_cell_ptr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Lane_Ptr sim_lane_ptr;
	Cell_Itr from_itr, to_itr;

	cells = step.veh_type_ptr->Cells ();
//bool debug = (step.sim_travel_ptr->First_Cell () == 5719 || step.veh_cell_ptr->Driver () == 4931);
//if (debug) sim->Write (1, "size=") << step.size () << " status=" << step.Status () << " time=" << sim->time_step << " driver=" << step.veh_cell_ptr->Driver () << " first=" << step.sim_travel_ptr->First_Cell () << " status=" << step.sim_travel_ptr->Status ();
	//---- process each movement pair ----

	for (from_itr = step.begin (); from_itr != step.end (); from_itr++) {
		to_itr = from_itr + 1;
		if (to_itr == step.end ()) break;
//if (debug) sim->Write (1, "from=") << from_itr->link << ":" << from_itr->lane << " to " << to_itr->link << ":" << to_itr->lane;
		if (from_itr->link != to_itr->link || from_itr->lane != to_itr->lane) {

			//---- remove the vehicle from the current lane ----

			if (from_itr->link >= 0 && from_itr->lane >= 0) {
				sim_dir_ptr = &sim->sim_dir_array [from_itr->link];
				sim_lane_ptr = &sim_dir_ptr->at (from_itr->lane);
	
				veh_cell = step.sim_travel_ptr->First_Cell ();
				veh_cell_ptr = step.veh_cell_ptr;

				lead_cell = veh_cell_ptr->Leader ();
				next_cell = -1;

				for (cell=0; cell < cells; veh_cell_ptr++) {
					if (cell == 0) {
						veh_cell_ptr->Leader (-1);
					} else {
						veh_cell_ptr->Leader (veh_cell - 1);

					}
					next_cell = veh_cell_ptr->Follower ();

					if (++cell < cells) {
						veh_cell_ptr->Follower (++veh_cell);
					} else {
						veh_cell_ptr->Follower (-1);
					}
					veh_cell_ptr->Link (-1);
					veh_cell_ptr->Lane (-1);
				}
				if (lead_cell < 0) {
					sim_lane_ptr->First_Veh (next_cell);
				} else {
					lead_cell_ptr = &sim->veh_cell_array [lead_cell];
					lead_cell_ptr->Follower (next_cell);
				}
				if (next_cell < 0) {
					sim_lane_ptr->Last_Veh (lead_cell);
				} else {
					next_cell_ptr = &sim->veh_cell_array [next_cell];
					next_cell_ptr->Leader (lead_cell);
				}
			}

			//---- add the vehicle to the new lane ----

			if (to_itr->link >= 0 && to_itr->lane >= 0) {
				lead_cell = to_itr->leader;

				sim_dir_ptr = &sim->sim_dir_array [to_itr->link];
				sim_lane_ptr = &sim_dir_ptr->at (to_itr->lane);
				
				veh_cell = step.sim_travel_ptr->First_Cell ();
				veh_cell_ptr = step.veh_cell_ptr;

				if (lead_cell < 0) {
					next_cell = sim_lane_ptr->First_Veh ();
					sim_lane_ptr->First_Veh (veh_cell);
				} else {
					lead_cell_ptr = &sim->veh_cell_array [lead_cell];
					next_cell = lead_cell_ptr->Follower ();
					lead_cell_ptr->Follower (veh_cell);
				}

				for (cell=0; cell < cells; veh_cell_ptr++) {
					veh_cell_ptr->Leader (lead_cell);
					lead_cell = veh_cell;

					if (++cell < cells) {
						veh_cell_ptr->Follower (++veh_cell);
					} else {
						veh_cell_ptr->Follower (next_cell);
					}
					veh_cell_ptr->Link (to_itr->link);
					veh_cell_ptr->Lane (to_itr->lane);
					veh_cell_ptr->Offset (to_itr->offset);
				}
				if (next_cell < 0) {
					sim_lane_ptr->Last_Veh (veh_cell);
				} else {
					next_cell_ptr = &sim->veh_cell_array [next_cell];
					next_cell_ptr->Leader (veh_cell);
				}

			} else {

				step.sim_travel_ptr->Status (0);
				veh_cell_ptr = step.veh_cell_ptr;
//if (debug) sim->Write (1, " remove vehicle link=") << to_itr->link << ":" << to_itr->lane;
				for (cell=0; cell < cells; veh_cell_ptr++) {
					veh_cell_ptr->Driver (-1);
					veh_cell_ptr->Link (to_itr->link);
					veh_cell_ptr->Lane (to_itr->lane);
					veh_cell_ptr->Offset (to_itr->offset);
					if (cell == 0) veh_cell_ptr->Leader (-1);
					if (++cell == cells) veh_cell_ptr->Follower (-1);
				}
//if (debug) sim->Write (1, "status=") << step.sim_travel_ptr->Status () << " event=" << step.sim_travel_ptr->Next_Event ();
			}

		} else {

			//---- move forward ----

			veh_cell_ptr = step.veh_cell_ptr;

			for (cell=0; cell < cells; cell++, veh_cell_ptr++) {
				veh_cell_ptr->Offset (to_itr->offset);
			}
		}
	}
	return (true);
}
