//*********************************************************
//	Sim_Link_Step.cpp - simulate directional links
//*********************************************************

#include "Sim_Link_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Link_Step -- constructor
//---------------------------------------------------------

Sim_Link_Step::Sim_Link_Step (void) : Static_Service ()
{
	num_vehicles = num_waiting = 0;

#ifdef THREADS
	num_threads = 0;
	sim_link_process = 0;
#endif
}

//---------------------------------------------------------
//	Sim_Link_Step -- destructor
//---------------------------------------------------------

Sim_Link_Step::~Sim_Link_Step (void)
{
	Stop_Processing ();
}

//---------------------------------------------------------
//	Sim_Link_Step -- Initialize
//---------------------------------------------------------

void Sim_Link_Step::Initialize (void)
{
	Randomize_Links ();

	num_vehicles = num_waiting = 0;

#ifdef THREADS
	num_threads = sim->Num_Threads ();

	sim_link_process = new Sim_Link_Process * [num_threads];

	if (num_threads > 1) {
		link_queue.Max_Records (200 + 40 * num_threads);

		for (int i=0; i < num_threads; i++) {
			sim_link_process [i] = new Sim_Link_Process (&link_queue, i);
			threads.push_back (thread (ref (*(sim_link_process [i]))));
		}
	} else {
		*sim_link_process = new Sim_Link_Process ();
	}
#endif
}

//---------------------------------------------------------
//	Sim_Link_Step -- Start_Processing
//---------------------------------------------------------

void Sim_Link_Step::Start_Processing (void)
{
	Int_Itr itr;

	//---- simulate the directional links ----

#ifdef THREADS
	Sim_Link_Process *sim_link_ptr;

	if (num_threads > 1) {
		Threads threads;
		Sim_Dir_Ptr sim_dir_ptr;
#ifdef CHECK
		int count = 0;
#endif

		link_queue.Start_Work ();

		for (itr = link_list.begin (); itr != link_list.end (); itr++) {
			sim_dir_ptr = &sim->sim_dir_array [*itr];
			if (!sim->method_time_flag [sim_dir_ptr->Method ()]) continue;
			if (sim_dir_ptr->Count () > 0 || sim_dir_ptr->load_queue.size () > 0) {
				link_queue.Put (*itr);
#ifdef CHECK
				count++;
#endif
			}
		}
		link_queue.Complete_Work ();
#ifdef CHECK
		if (count != link_queue.Total_Records ()) {
			sim->Write (1, String ("Link Queue %d vs %d") % count % link_queue.Total_Records ());
		}
#endif
		//---- sum the counters ----

		num_vehicles = num_waiting = 0;

		for (int i=0; i < num_threads; i++) {
			sim_link_ptr = sim_link_process [i];

			num_vehicles += sim_link_ptr->Num_Vehicles ();
			num_waiting += sim_link_ptr->Num_Waiting ();
			sim_link_ptr->Reset_Counters ();
		}
	} else {
		sim_link_ptr = *sim_link_process;

		for (itr = link_list.begin (); itr != link_list.end (); itr++) {
			sim_link_ptr->Link_Processing (*itr);
		}
		num_vehicles = sim_link_ptr->Num_Vehicles ();
		num_waiting = sim_link_ptr->Num_Waiting ();
		sim_link_ptr->Reset_Counters ();
	}
#else
	for (itr = link_list.begin (); itr != link_list.end (); itr++) {
		sim_link_process.Link_Processing (*itr);
	}
	num_vehicles = sim_link_process.Num_Vehicles ();
	num_waiting = sim_link_process.Num_Waiting ();

	if (num_vehicles > 0 || num_waiting > 0) {
		Int_Itr int_itr;
		int count_veh, count_wait;
		count_veh = count_wait = 0;

		for (itr = link_list.begin (); itr != link_list.end (); itr++) {
			Sim_Dir_Ptr sim_dir_ptr = &sim->sim_dir_array [*itr];
			int count = 0;

			for (int_itr = sim_dir_ptr->begin (); int_itr != sim_dir_ptr->end (); int_itr++) {
				if (*int_itr > 0) count++;
			}
			if (count != sim_dir_ptr->Count ()) {
				sim->Write (1, " step=") << sim->time_step << " link=" << *itr << " count=" << count << " vs " << sim_dir_ptr->Count ();
			}
			count_veh += count;

			count_wait += (int) sim_dir_ptr->load_queue.size ();
		}
		if (num_vehicles != count_veh || num_waiting != count_wait) {
			sim->Write (1, " step=") << sim->time_step << " veh=" << count_veh << " vs " << num_vehicles << " wait=" << count_wait << " vs " << num_waiting;
		}
	}
	sim_link_process.Reset_Counters ();
#endif
	if (num_vehicles > 0 || num_waiting > 0) sim->Active (true);
	sim->Num_Vehicles (num_vehicles);
}

//---------------------------------------------------------
//	Sim_Link_Step -- Stop_Processing
//---------------------------------------------------------

void Sim_Link_Step::Stop_Processing (void)
{
#ifdef THREADS
	if (sim_link_process != 0) {
		if (num_threads > 1) {
			link_queue.Exit_Queue ();
			threads.Join_All ();
		}
		for (int i=0; i < num_threads; i++) {
			delete sim_link_process [i];
			sim_link_process [i] = 0;
		}
		delete [] sim_link_process;
		sim_link_process = 0;
	}
#endif
}

//---------------------------------------------------------
//	Sim_Link_Step -- Randomize_Links
//---------------------------------------------------------

void Sim_Link_Step::Randomize_Links (void) 
{
	int num_links = (int) sim->dir_array.size ();
	link_list.assign (num_links, 0);

	//---- randomize the link list ----

	for (int i=0; i < num_links; i++) {
		link_list [i] = i;
	}
	sim->random.Randomize (link_list);
}

//---------------------------------------------------------
//	Add_Statistics
//---------------------------------------------------------

void Sim_Link_Step::Add_Statistics (Sim_Statistics &stats)
{
#ifdef THREADS
	if (num_threads > 1) {
		for (int i=0; i < num_threads; i++) {
			stats.Add_Statistics (sim_link_process [i]->Get_Statistics ());
		}
	} else {
		stats.Add_Statistics ((*sim_link_process)->Get_Statistics ());
	}
#else
	stats.Add_Statistics (sim_link_process.Get_Statistics ());
#endif
}

