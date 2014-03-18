//*********************************************************
//	Sim_Transit_Update.cpp - Transit Run Update Class
//*********************************************************

#include "Sim_Transit_Update.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Transit_Update constructor
//---------------------------------------------------------

Sim_Transit_Update::Sim_Transit_Update () : Sim_Update_Data ()
{
	Type (SIM_TRANSIT_UPDATE);
	
	run_update_time = 0;
}

//---------------------------------------------------------
//	Update_Check
//---------------------------------------------------------

bool Sim_Transit_Update::Update_Check (void)
{
	if (run_update_time > sim->time_step) return (false);

	int i, j, traveler;

	Sim_Plan_Itr plan_itr;
	Sim_Plan_Ptr plan_ptr;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Person_Index person_index;
	Person_Map_Stat person_stat;
	Sim_Travel_Data *sim_travel_ptr, sim_travel_data;
	Vehicle_Index veh_index;
	Vehicle_Map_Itr veh_itr;

	run_update_time = MAX_INTEGER;

	for (i=0, plan_itr = sim->transit_plans.begin (); plan_itr != sim->transit_plans.end (); plan_itr++, i++) {
		line_ptr = &sim->line_array [i];

		stop_itr = line_ptr->begin ();

		for (j=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, j++) {
			if (run_itr->Schedule () < sim->time_step) continue;
			if (run_itr->Schedule () > sim->time_step) {
				if (run_itr->Schedule () < run_update_time) {
					run_update_time = run_itr->Schedule ();
				}
				continue;
			}
			sim->stats.num_runs++;

			//---- find the traveler ----

			person_index.Household (sim->line_array.Vehicle_ID (line_ptr->Route (), j));
			person_index.Person (0);

			traveler = (int) sim->sim_travel_array.size ();

			if (traveler == 0) {
				sim_travel_data.Traveler (0);
				sim->sim_travel_array.push_back (sim_travel_data);
				traveler = 1;
			}
			person_stat = sim->person_map.insert (Person_Map_Data (person_index, traveler));

			//---- create a new traveler ----

			sim->sim_travel_array.push_back (sim_travel_data);

			sim_travel_ptr = &sim->sim_travel_array [traveler];

			sim_travel_ptr->Household (person_index.Household ());
			sim_travel_ptr->Person (0);
			sim_travel_ptr->Traveler (traveler);
			sim_travel_ptr->random.Seed (sim->Random_Seed () + traveler);
			sim_travel_ptr->Status (-1);

			//sim_travel_ptr->Type (0);
			//sim_travel_ptr->Status (-2);

			plan_ptr = sim_travel_ptr->sim_plan_ptr = new Sim_Plan_Data ();

			*plan_ptr = (*plan_itr);

			plan_ptr->Tour (1);
			plan_ptr->Trip (j+1);

			plan_ptr->Start (run_itr->Schedule ());
			plan_ptr->End (plan_ptr->Start () + plan_ptr->Duration ());

			veh_index.Household (sim_travel_ptr->Household ());
			veh_index.Vehicle (0);

			veh_itr = sim->sim_veh_map.find (veh_index);	

			plan_ptr->Vehicle (veh_itr->second);
		}
	}
	return (true);
}

