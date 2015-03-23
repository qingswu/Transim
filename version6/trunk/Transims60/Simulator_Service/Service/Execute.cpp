//*********************************************************
//	Execute.cpp - simulation service execution
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Simulator_Service::Execute (void)
{
	int subarea, group, index;

	Int_Map_Itr map_itr;
	Int2_Map_Itr part_itr;
	Int2_Key part_key;
	Node_Itr node_itr;
	Sim_Group_Itr group_itr;
	Shorts subareas;
	Integers subarea_group;

#ifdef ROUTING
	Converge_Service::Execute ();
#else
	Data_Service::Execute ();
#endif

	if (!Control_Flag ()) {
		Error ("Simulator_Service::Program_Control has Not Been Called");
	}
	Num_Subareas ((int) subarea_map.size ());

	subarea_group.assign (Num_Subareas (), -1);

	for (map_itr = subarea_map.begin (); map_itr != subarea_map.end (); map_itr++) {
		subarea = map_itr->first;
		index = map_itr->second;

		for (group=0, group_itr = sim_group_array.begin (); group_itr != sim_group_array.end (); group_itr++, group++) {
			if (group_itr->subareas.In_Range (subarea)) {
				if (subarea_group [index] >= 0) {
					Warning (String ("Subarea %d was Reassigned to Simulation Group %d") % subarea % group_itr->group);
				}
				subarea_group [index] = group;
			}
		}
		if (subarea_group [index] < 0) {
			Warning (String ("Subarea %d was Not Assigned to a Simulation Group") % subarea);
			subarea_group [index] = 0;
		}
	}
	if (Num_Subareas () > 1) {
		Print (1);
		Write (1, "Number of Subareas = ") << Num_Subareas ();
	}

	//---- define the period subarea methods ----

	subareas.assign (max_subarea + 1, 0);
	period_subarea_method.assign (sim_periods.Num_Periods (), subareas);

	for (index=0; index < sim_periods.Num_Periods (); index++) {
		for (map_itr = subarea_map.begin (); map_itr != subarea_map.end (); map_itr++) {
			subarea = map_itr->first;
			group = map_itr->second;

			period_subarea_method [index] [subarea] = (short) sim_group_array [subarea_group [group]].methods [index];
		}
	}

	//---- initialize the simulator processing steps ----

	sim_update_step.Initialize ();
	sim_plan_step.Initialize ();
	sim_travel_step.Initialize ();
	sim_node_step.Initialize ();
	sim_output_step.Initialize ();

	Show_Message (1);
}

//---------------------------------------------------------
//	Stop_Simulation
//---------------------------------------------------------

Sim_Statistics * Simulator_Service::Stop_Simulation (void)
{
	sim_travel_step.Add_Statistics (stats);
	sim_node_step.Add_Statistics (stats);

	sim_update_step.Stop_Processing ();
	sim_plan_step.Stop_Processing ();
	sim_travel_step.Stop_Processing ();
	sim_node_step.Stop_Processing ();
	sim_output_step.Stop_Processing ();

	return (&stats);
}
