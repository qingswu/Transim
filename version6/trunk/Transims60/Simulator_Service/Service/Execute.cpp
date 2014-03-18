//*********************************************************
//	Execute.cpp - simulation service execution
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void Simulator_Service::Execute (void)
{
	int i;

	Int2_Map_Itr part_itr;
	Int2_Key part_key;
	Node_Itr node_itr;

	//Router_Service::Execute ();
	Data_Service::Execute ();

	if (!Control_Flag ()) {
		Error ("Simulator_Service::Program_Control has Not Been Called");
	}
	Num_Subareas ((int) subarea_map.size ());

	for (i=0; i < Num_Subareas (); i++) {
		param.subarea_method.push_back (NO_SIMULATION);

		if (macro_range.In_Range (i)) {
			param.subarea_method [i] = MACROSCOPIC;
		}
		if (meso_range.In_Range (i)) {
			param.subarea_method [i] = MESOSCOPIC;
		}
		if (micro_range.In_Range (i)) {
Write (1, "Microscopic Processing is Not Implemented");
			param.subarea_method [i] = MICROSCOPIC;
		}
	}
	if (Num_Subareas () > 1) {
		Print (1);
		Write (1, "Number of Subareas = ") << Num_Subareas ();
		if (Num_Parts () != Num_Subareas ()) {
			Write (1, "Number of Partitions = ") << Num_Parts ();
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
