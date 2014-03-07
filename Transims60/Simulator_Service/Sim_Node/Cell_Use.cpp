//*********************************************************
//	Cell_Use.cpp - check the cell use restrictions
//*********************************************************

#include "Sim_Node_Process.hpp"

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Cell_Use
//---------------------------------------------------------

bool Sim_Node_Process::Cell_Use (Sim_Dir_Ptr sim_dir_ptr, int lane, int cell, Travel_Step &step, bool use_flag)
{
	int i, index, num, seed, offset, type, veh_type;
	Use_Type use;

	Lane_Use_Data *use_ptr;
	Lane_Use_Period *period_ptr;
	Link_Dir_Data *use_index;

	step.Delay (0);

	if (lane < 0 || lane >= sim_dir_ptr->Lanes () || cell < 0 || cell > sim_dir_ptr->Max_Cell ()) return (false);
	if (!sim_dir_ptr->Check (lane, cell)) return (false);
	if (cell <= sim_dir_ptr->In_Cell () || cell >= sim_dir_ptr->Out_Cell ()) return (true);

	if (step.sim_travel_ptr == 0) {
		if (step.Traveler () < 0) return (false);
		step.sim_travel_ptr = &sim->sim_travel_array [step.Traveler ()];
	}
	type = step.sim_travel_ptr->sim_plan_ptr->Type ();
	veh_type = step.sim_travel_ptr->sim_plan_ptr->Veh_Type ();

	if (step.veh_type_ptr == 0) {
		step.veh_type_ptr = &sim->veh_type_array [veh_type];
	}
	use = step.veh_type_ptr->Use ();

	if (use_flag) {
		if (use != BUS && use != HOV2 && use != HOV3 && use != HOV4) return (false);
		if (Use_Permission (sim_dir_ptr->Use (), SOV)) return (false);
	}
	if (!Use_Permission (sim_dir_ptr->Use (), use)) return (false);

	if (veh_type >= 0 && sim_dir_ptr->Min_Veh_Type () >= 0) {
		if (sim_dir_ptr->Use_Type () == PROHIBIT) {
			if (veh_type >= sim_dir_ptr->Min_Veh_Type () && veh_type <= sim_dir_ptr->Max_Veh_Type ()) return (false);
		} else {
			if (veh_type < sim_dir_ptr->Min_Veh_Type () || veh_type > sim_dir_ptr->Max_Veh_Type ()) return (false);
		}
	}
	if (type > 0 && sim_dir_ptr->Min_Traveler () > 0) {
		if (sim_dir_ptr->Use_Type () == PROHIBIT) {
			if (type >= sim_dir_ptr->Min_Traveler () && type <= sim_dir_ptr->Max_Traveler ()) return (false);
		} else {
			if (type < sim_dir_ptr->Min_Traveler () || type > sim_dir_ptr->Max_Traveler ()) return (false);
		}
	}
	index = sim_dir_ptr->First_Use ();
	if (index < 0) return (true);

	offset = cell * sim->param.cell_size;

	period_ptr = &sim->use_period_array [index];

	num = period_ptr->Records ();
	index = period_ptr->Index ();

	for (i=0; i < num; i++, index++) {
		use_index = &sim->use_period_index [index];
		use_ptr = &sim->lane_use_array [use_index->Link ()];

		if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) {
			if (use_ptr->Offset () > offset || offset > (use_ptr->Offset () + use_ptr->Length ())) continue;
		}
		if (use_ptr->Type () == REQUIRE) {
			if (!Use_Permission (use_ptr->Use (), use)) continue;
			if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
				if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) continue;
			}
			if (type > 0 && use_ptr->Min_Traveler () > 0) {
				if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) continue;
			}
			return (use_ptr->Low_Lane () <= lane  && lane <= use_ptr->High_Lane ());
		} else {
			if (use_ptr->Low_Lane () > lane || lane > use_ptr->High_Lane ()) continue;

			if (use_ptr->Type () == PROHIBIT) {
				if (use_flag && !Use_Permission (use_ptr->Use (), SOV)) return (false);
				if (!Use_Permission (use_ptr->Use (), use)) continue;
				if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
					if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) continue;
				}
				if (type > 0 && use_ptr->Min_Traveler () > 0) {
					if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) continue;
				}
				return (false);
			} else if (use_ptr->Type () == LIMIT) {
				if (use_flag && Use_Permission (use_ptr->Use (), SOV)) return (false);
				if (!Use_Permission (use_ptr->Use (), use)) return (false);
				if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
					if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) return (false);
				}
				if (type > 0 && use_ptr->Min_Traveler () > 0) {
					if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) return (false);
				}
				return (true);
			} else if (use_ptr->Type () == APPLY) {
				if (!Use_Permission (use_ptr->Use (), use)) continue;
				if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
					if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) continue;
				}
				if (type >= 0 && use_ptr->Min_Traveler () > 0) {
					if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) continue;
				}
				if (use_ptr->Min_Delay () > 0 || use_ptr->Max_Delay () > 0) {
					if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) {
						if (offset < use_ptr->Offset () || offset > use_ptr->Offset () + use_ptr->Length ()) break;
					}
					int diff = 0;
					if (use_ptr->Max_Delay () > use_ptr->Min_Delay ()) {
						seed = abs (index + sim->Random_Seed () + step.Traveler () + lane);
						diff = DTOI ((use_ptr->Max_Delay () - use_ptr->Min_Delay ()) * sim->random.Probability (seed));
					}
					step.Delay (use_ptr->Min_Delay () + diff);
				}
				break;
			}
		}
	}
	return (true);
}

//---------------------------------------------------------
//	Cell_Use
//---------------------------------------------------------

bool Sim_Node_Process::Cell_Use (Travel_Step &step)
{
	int lane, lanes, cell, offset;
	
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Veh_Ptr sim_veh_ptr;

	if (step.size () == 0) return (false);

	sim_veh_ptr = &step [0];
	
	if (sim_veh_ptr->link == step.Dir_Index ()) {
		if (step.sim_dir_ptr == 0) {
			step.sim_dir_ptr = &sim->sim_dir_array [sim_veh_ptr->link];
		}
		sim_dir_ptr = step.sim_dir_ptr;
	} else {
		sim_dir_ptr = &sim->sim_dir_array [sim_veh_ptr->link];
	}
	if (sim_dir_ptr->Method () != MESOSCOPIC) return (true);

	lanes = sim_dir_ptr->Lanes ();
	offset = sim_veh_ptr->offset;

	cell = offset / sim->param.cell_size;   //************//

	for (lane = 0; lane < lanes; lane++) {
		if (Cell_Use (sim_dir_ptr, lane, cell, step)) return (true);
	}
	Dtime max_time = sim->time_step + sim->param.max_wait_time;

	if (sim->use_update_time < max_time) {
		int i, index, num;
		Lane_Use_Data *use_ptr;
		Lane_Use_Period *period_ptr;
		Link_Dir_Data *use_index;

		Use_Type use = step.veh_type_ptr->Use ();
		int veh_type = step.sim_travel_ptr->sim_plan_ptr->Veh_Type ();
		int type = step.sim_travel_ptr->sim_plan_ptr->Type ();

		Dir_Data *dir_ptr = &sim->dir_array [sim_veh_ptr->link];

		index = dir_ptr->First_Lane_Use ();
		if (index < 0) goto use_error;

		for (period_ptr = &sim->use_period_array [index]; ; period_ptr = &sim->use_period_array [++index]) {
			if (period_ptr->Start () >= sim->use_update_time && period_ptr->Start () <= max_time) break;
			if (period_ptr->Periods () == 0) goto use_error;
		}
		num = period_ptr->Records ();
		index = period_ptr->Index ();

		for (i=0; i < num; i++, index++) {
			use_index = &sim->use_period_index [index];
			use_ptr = &sim->lane_use_array [use_index->Link ()];

			if (use_ptr->Offset () > 0 || use_ptr->Length () > 0) {
				if (use_ptr->Offset () > offset || offset > (use_ptr->Offset () + use_ptr->Length ())) continue;
			}
			if (use_ptr->Type () == PROHIBIT) {
				if (!Use_Permission (use_ptr->Use (), use)) {
					if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
						if (veh_type < use_ptr->Min_Veh_Type () || veh_type > use_ptr->Max_Veh_Type ()) {
							if (type > 0 && use_ptr->Min_Traveler () > 0) {
								if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) return (true);
							} else {
								return (true);
							}
						}
					} else if (type > 0 && use_ptr->Min_Traveler () > 0) {
						if (type < use_ptr->Min_Traveler () || type > use_ptr->Max_Traveler ()) return (true);
					} else {
						return (true);
					}
				}
			} else {
				if (Use_Permission (use_ptr->Use (), use)) {
					if (veh_type >= 0 && use_ptr->Min_Veh_Type () >= 0) {
						if (veh_type >= use_ptr->Min_Veh_Type () && veh_type <= use_ptr->Max_Veh_Type ()) {
							if (type > 0 && use_ptr->Min_Traveler () > 0) {
								if (type >= use_ptr->Min_Traveler () && type <= use_ptr->Max_Traveler ()) return (true);
							} else {
								return (true);
							}
						}
					} else if (type > 0 && use_ptr->Min_Traveler () > 0) {
						if (type >= use_ptr->Min_Traveler () && type <= use_ptr->Max_Traveler ()) return (true);
					} else {
						return (true);
					}
				}
			}
		}
	}
use_error:
	step.Problem (USE_PROBLEM);
	step.Status (1);
	return (false);
}
