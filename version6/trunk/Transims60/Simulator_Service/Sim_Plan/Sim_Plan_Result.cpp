//*********************************************************
//	Sim_Plan_Result.cpp - create/update travel records
//*********************************************************

#include "Sim_Plan_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Plan_Step -- Sim_Plan_Result
//---------------------------------------------------------

bool Sim_Plan_Step::Sim_Plan_Result (Sim_Travel_Ptr sim_travel_ptr) 
{
	int vehicle, traveler, cells;

	Vehicle_Index veh_index;
	Vehicle_Map_Stat veh_stat;
	Person_Index person_index;
	Person_Map_Stat person_stat;
	Sim_Plan_Ptr sim_plan_ptr, next_plan_ptr;
	Veh_Type_Data *veh_type_ptr;
	Int_Map_Itr map_itr;
	Sim_Veh_Data sim_veh_data;

	//---- delete the travel plans ----

	if (sim_travel_ptr->Household () == 0 || sim_travel_ptr->sim_plan_ptr == 0) {
		if (sim_travel_ptr->sim_plan_ptr != 0) {
			delete sim_travel_ptr->sim_plan_ptr;
		}
		delete sim_travel_ptr;
		return (true);
	}

	//---- update the traveler maps ----

	sim_plan_ptr = sim_travel_ptr->sim_plan_ptr;

	//---- find the vehicle ----

	vehicle = sim_plan_ptr->Vehicle ();

	if (vehicle > 0) {
		veh_index.Household (sim_travel_ptr->Household ());
		veh_index.Vehicle (vehicle);

		vehicle = (int) sim->sim_veh_array.size ();

		veh_stat = sim->sim_veh_map.insert (Vehicle_Map_Data (veh_index, vehicle));

		if (veh_stat.second) {

			//---- create a new vehicle ----

			cells = 1;

			if (sim_plan_ptr->Veh_Type () > 0) {
				map_itr = sim->veh_type_map.find (sim_plan_ptr->Veh_Type ());
				if (map_itr != sim->veh_type_map.end ()) {
					veh_type_ptr = &sim->veh_type_array [map_itr->second];
					cells = MAX (((veh_type_ptr->Length () + sim->param.half_cell) / sim->param.cell_size), 1);
				}
			}
			while (cells-- > 0) {
				sim->sim_veh_array.push_back (sim_veh_data);
			}
		}
	}

	//---- find the traveler ----

	person_index.Household (sim_travel_ptr->Household ());
	person_index.Person (sim_travel_ptr->Person ());

	traveler = (int) sim->sim_travel_array.size ();
	if (traveler == 0) traveler = 1;

	person_stat = sim->person_map.insert (Person_Map_Data (person_index, traveler));

	if (!person_stat.second) {

		//---- update existing traveler ----

		delete sim_travel_ptr;

		traveler = person_stat.first->second;
		sim_travel_ptr = &sim->sim_travel_array [traveler];

		if (sim_travel_ptr->sim_plan_ptr == 0) {
			sim_travel_ptr->sim_plan_ptr = sim_plan_ptr;
			sim_travel_ptr->Status (0);
		} else {
			for (next_plan_ptr = sim_travel_ptr->sim_plan_ptr; ; next_plan_ptr = next_plan_ptr->next_plan_ptr) {
				if (next_plan_ptr->next_plan_ptr == 0) {
					next_plan_ptr->next_plan_ptr = sim_plan_ptr;
					break;
				}
			}
		}

	} else {

		//---- create a new travel record ----

		if (sim->sim_travel_array.size () == 0) {
			Sim_Travel_Data sim_travel_data;
			sim_travel_data.Traveler (0);
			sim->sim_travel_array.push_back (sim_travel_data);
		}
		sim_travel_ptr->Traveler (traveler);
		sim_travel_ptr->random.Seed (sim->Random_Seed () + traveler);
		sim_travel_ptr->Status (-1);

		sim->sim_travel_array.push_back (*sim_travel_ptr);

		delete sim_travel_ptr;

		sim_travel_ptr = &sim->sim_travel_array [traveler];
		sim_travel_ptr->sim_plan_ptr = sim_plan_ptr;
	}
	return (true);
}
