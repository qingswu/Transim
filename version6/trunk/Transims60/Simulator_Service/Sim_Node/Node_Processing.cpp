//*********************************************************
//	Node_Processing.cpp - select approach links
//*********************************************************

#include "Sim_Node_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Node_Processing
//---------------------------------------------------------

bool Sim_Node_Process::Node_Processing (int node)
{
	Node_Data *node_ptr = &sim->node_array [node];
	if (!sim->method_time_flag [node_ptr->Method ()]) return (false);

	int step_size = sim->method_time_step [node_ptr->Method ()];
	if (step_size <= 0) return (false);

	int i, num, link, list [20];

	for (num = 0, link = sim->node_link [node]; link >= 0; link = sim->link_list [link], num++) {
		list [num] = link;
	}
	if (num == 0) return (false);

	link = (sim->time_step / step_size) % num;

	for (i=0; i < num; i++, link++) {
		if (link >= num) link = 0;
		Link_Processing (list [link]);
	}
	return (true);
}
