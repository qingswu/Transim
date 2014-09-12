//*********************************************************
//	Set_Sim_Period.cpp - set subarea attributes by period
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Set_Sim_Period
//---------------------------------------------------------

Dtime Simulator_Service::Set_Sim_Period (void)
{
	int subarea, method;
	Dtime low, high;
	Node_Itr node_itr;
	Sim_Dir_Itr sim_dir_itr;
	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Veh_Ptr sim_veh_ptr;
	Sim_Leg_Ptr leg_ptr;
	Int_Map_Itr map_itr;
	Int_Itr itr;

	if (sim_periods.Period_Range (sim_period + 1, low, high)) {
		sim_period++;
		param.step_size = period_step_size [sim_period];

		max_method = 0;

		for (map_itr = subarea_map.begin (); map_itr != subarea_map.end (); map_itr++) {
			subarea = map_itr->first;

			if (period_subarea_method [sim_period] [subarea] > max_method) {
				max_method = period_subarea_method [sim_period] [subarea];
			}
		}
		for (node_itr = node_array.begin (); node_itr != node_array.end (); node_itr++) {
			node_itr->Method (period_subarea_method [sim_period] [node_itr->Subarea ()]);
		}
		for (sim_dir_itr = sim_dir_array.begin (); sim_dir_itr != sim_dir_array.end (); sim_dir_itr++) {
			method = sim_dir_itr->Method ();
			sim_dir_itr->Method (period_subarea_method [sim_period] [sim_dir_itr->Subarea ()]);

			if (method > NO_SIMULATION && sim_dir_itr->Method () == NO_SIMULATION && sim_dir_itr->Count () > 0) {

				//---- remove vehicles from the link and estimate the next event time ----

				for (itr = sim_dir_itr->begin (); itr != sim_dir_itr->end (); itr++) {
					if (*itr <= 0) continue;
					
					sim_travel_ptr = &sim_travel_array [*itr];
					*itr = 0;

					sim_travel_ptr->Status (OFF_NET_DRIVE);

					sim_plan_ptr = sim_travel_ptr->Get_Plan ();
					if (sim_plan_ptr == 0) continue;

					leg_ptr = sim_plan_ptr->Get_Leg ();

					if (sim_travel_ptr->Vehicle () < 1) continue;
					sim_veh_ptr = &sim_veh_array [sim_travel_ptr->Vehicle ()];

					low = (int) ((sim_dir_itr->Length () - sim_veh_ptr->offset) * leg_ptr->Time () / sim_dir_itr->Length ());
					sim_travel_ptr->Next_Event (time_step + low);
				}
			}
		}
	} else {
		high = MAX_INTEGER;
	}
	return (high);
}
