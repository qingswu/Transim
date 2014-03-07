//*********************************************************
//	Sim_Node_Step.cpp - simulate links entering nodes
//*********************************************************

#include "Sim_Node_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Node_Step -- constructor
//---------------------------------------------------------

Sim_Node_Step::Sim_Node_Step (void) : Static_Service ()
{
	num_vehicles = num_waiting = 0;

#ifdef THREADS
	num_threads = 0;
	sim_node_process = 0;
#endif
}

//---------------------------------------------------------
//	Sim_Node_Step -- Initialize
//---------------------------------------------------------

void Sim_Node_Step::Initialize (void)
{
	Randomize_Nodes ();

	num_vehicles = num_waiting = 0;

#ifdef THREADS
	num_threads = MAX (sim->Num_Threads (), 1);
	sim_node_process = new Sim_Node_Process * [num_threads];

	if (num_threads > 1) {
		for (int i=0; i < num_threads; i++) {
			sim_node_process [i] = new Sim_Node_Process ();
			threads.push_back (thread (*(sim_node_process [i])));
		}
	} else {
		*sim_node_process = new Sim_Node_Process ();
	}
#else
	sim_node_process.Initialize ();
#endif
}

//---------------------------------------------------------
//	Sim_Node_Step -- Start_Processing
//---------------------------------------------------------

void Sim_Node_Step::Start_Processing (void)
{
	Int_Itr itr;

	//---- simulate the links entering each node ----

#ifdef THREADS
	Sim_Node_Process *sim_node_ptr;

	if (num_threads > 1) {
		sim->node_barrier.Start ();

		//---- sum the counters ----

		num_vehicles = num_waiting = 0;

		for (int i=0; i < num_threads; i++) {
			sim_node_ptr = sim_node_process [i];

			num_vehicles += sim_node_ptr->Num_Vehicles ();
			num_waiting += sim_node_ptr->Num_Waiting ();
			sim_node_ptr->Reset_Counters ();
		}
	} else {
		sim_node_ptr = *sim_node_process;

		for (itr = sim->node_list.begin (); itr != sim->node_list.end (); itr++) {
			sim_node_ptr->Node_Processing (*itr);
		}
		num_vehicles = sim_node_ptr->Num_Vehicles ();
		num_waiting = sim_node_ptr->Num_Waiting ();
		sim_node_ptr->Reset_Counters ();
	}
#else
	for (itr = sim->node_list.begin (); itr != sim->node_list.end (); itr++) {
		sim_node_process.Node_Processing (*itr);
	}
	num_vehicles = sim_node_process.Num_Vehicles ();
	num_waiting = sim_node_process.Num_Waiting ();
	sim_node_process.Reset_Counters ();
#endif
	if (num_vehicles > 0 || num_waiting > 0) sim->Active (true);
	sim->Num_Vehicles (num_vehicles);
}

//---------------------------------------------------------
//	Sim_Node_Step -- Stop_Processing
//---------------------------------------------------------

void Sim_Node_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_node_process != 0) {
		sim->node_barrier.Exit ();
		threads.Join_All ();

		for (int i=0; i < num_threads; i++) {
			delete sim_node_process [i];
			sim_node_process [i] = 0;
		}
		delete sim_node_process;
		sim_node_process = 0;
	}
#endif
}

//---------------------------------------------------------
//	Sim_Node_Step -- Randomize_Nodes
//---------------------------------------------------------

void Sim_Node_Step::Randomize_Nodes (void) 
{
	int num_nodes = (int) sim->node_array.size ();
	sim->node_list.assign (num_nodes, 0);
	sim->node_barrier.Num_Barriers (num_nodes);

	//---- randomize the node list ----

	for (int i=0; i < num_nodes; i++) {
		sim->node_list [i] = i;
	}
	sim->random.Randomize (sim->node_list);
}

//---------------------------------------------------------
//	Add_Statistics
//---------------------------------------------------------

void Sim_Node_Step::Add_Statistics (Sim_Statistics &stats)
{
#ifdef THREADS
	if (num_threads > 1) {
		for (int i=0; i < num_threads; i++) {
			stats.Add_Statistics (sim_node_process [i]->Get_Statistics ());
		}
	} else {
		stats.Add_Statistics ((*sim_node_process)->Get_Statistics ());
	}
#else
	stats.Add_Statistics (sim_node_process.Get_Statistics ());
#endif
}
