//*********************************************************
//	Sim_Plan_Result.cpp - create/update travel records
//*********************************************************

#include "Sim_Plan_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Sim_Plan_Step -- Sim_Plan_Result
//---------------------------------------------------------

bool Sim_Plan_Step::Sim_Plan_Result (Sim_Trip_Ptr sim_trip_ptr) 
{
	int vehicle, traveler, cells;

	Vehicle_Index veh_index;
	Vehicle_Map_Stat veh_stat;
	Person_Index person_index;
	Person_Map_Stat person_stat;
	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Veh_Type_Data *veh_type_ptr;
	Int_Map_Itr map_itr;
	Sim_Veh_Data sim_veh_data;

#ifdef CHECK
	if (sim_trip_ptr == 0) sim->Error ("Sim_Plan_Step::Sim_Plan_Result: sim_trip_ptr");
#endif
	sim_travel_ptr = &sim_trip_ptr->sim_travel_data;
	sim_plan_ptr = &sim_trip_ptr->sim_plan_data;
#ifdef CHECK
	if (sim_travel_ptr == 0) sim->Error ("Sim_Plan_Step::Sim_Plan_Result: sim_travel_ptr");
	if (sim_plan_ptr == 0) sim->Error ("Sim_Plan_Step::Sim_Plan_Result: sim_plan_ptr");
#endif

	//---- delete the travel plans ----

	if (sim_travel_ptr->Household () == 0 || sim_plan_ptr->First_Leg () < 0) {
		delete sim_trip_ptr;
		return (true);
	}

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
					cells = veh_type_ptr->Cells ();
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
	if (traveler == 0) traveler = 2;

	person_stat = sim->person_map.insert (Person_Map_Data (person_index, traveler));

	if (!person_stat.second) {

		//---- update existing traveler ----

		traveler = person_stat.first->second;

	} else {

		//---- create a new travel record ----

		sim_travel_ptr->Traveler (traveler);
		sim_travel_ptr->random.Seed (sim->Random_Seed () + traveler);
		sim_travel_ptr->Status (NOT_ACTIVE);

		sim->sim_travel_array.push_back (*sim_travel_ptr);
	}
#ifdef CHECK
	if (traveler < 2 || (int) sim->sim_travel_array.size () <= traveler) sim->Error ("Sim_Plan_Step::Sim_Plan_Result: traveler");
#endif
	sim_travel_ptr = &sim->sim_travel_array [traveler];
	sim_travel_ptr->Add_Plan (*sim_plan_ptr);

	delete sim_trip_ptr;
	return (true);
}
