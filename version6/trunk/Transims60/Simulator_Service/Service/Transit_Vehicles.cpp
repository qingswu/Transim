//*********************************************************
//	Transit_Vehicles.cpp - create transit vehicles
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Transit_Vehicles
//---------------------------------------------------------

void Simulator_Service::Transit_Vehicles (void)
{
	int i, j, k, k1, last_veh, route, index, traveler, cells, line_cells, runs;
	String transit_veh;
	bool run_flag;

	Line_Itr line_itr;  
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Vehicle_Map_Itr vehicle_itr;
	Vehicle_Map_Stat map_stat;
	Vehicle_Index veh_index;
	Int_Map_Itr map_itr;
	Veh_Type_Data *veh_type_ptr;
	Sim_Veh_Data veh_data;
	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Travel_Data sim_travel_data;

	//---- initialize the vehicle ID ----

	last_veh = 100000000;
	transit_id = line_array.Vehicle_ID (last_veh);

	transit_veh = (String ("%d") % transit_id);

	i = (int) transit_veh.size ();

	j = (int) (String ("%d") % line_array.Max_Runs ()).size ();
	j = i-j;

	for (k=j; k < i; k++) {
		transit_veh [k] = '#';
	}
	if (line_map.size () > 0) {
		map_itr = --line_map.end ();
		route = map_itr->first;
	} else {
		route = 0;
	}
	k1 = (int) (String ("%d") % route).size ();

	for (k=j-k1; k < j; k++) {
		transit_veh [k] = 'x';
	}
	Print (2, "Transit Vehicle ID = ") << transit_veh;
	Print (1, "Number of Transit Vehicles = ") << line_array.Num_Runs ();

	//---- count vehicle cells ----

	cells = 0;

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		runs = (int) line_itr->begin ()->size ();

		if (line_itr->run_types.size () > 0) {
			for (i=0; i < runs; i++) {
				veh_type_ptr = &veh_type_array [line_itr->Run_Type (i)];
				cells += MAX (((veh_type_ptr->Length () + sim->param.half_cell) / sim->param.cell_size), 1);
			}
		} else {
			veh_type_ptr = &veh_type_array [line_itr->Type ()];
			cells += runs * MAX (((veh_type_ptr->Length () + sim->param.half_cell) / sim->param.cell_size), 1);
		}
	}

	//---- reserve memory ----

	sim_travel_array.reserve (line_array.Num_Runs () + num_travelers);
	sim_veh_array.reserve (2 * num_travelers + cells);

	//---- create a traveler for each transit run ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {

		stop_itr = line_itr->begin ();

		veh_index.Vehicle (0);
				
		veh_type_ptr = &veh_type_array [line_itr->Type ()];
		line_cells = MAX (((veh_type_ptr->Length () + sim->param.half_cell) / sim->param.cell_size), 1);

		run_flag = (line_itr->run_types.size () > 0);

		//---- create the transit vehicles ----

		for (i=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, i++) {

			//---- create the traveler ----

			traveler = (int) sim_travel_array.size ();

			sim_travel_data.Traveler (traveler);
			sim_travel_data.random.Seed (sim->Random_Seed () + traveler);

			sim_travel_array.push_back (sim_travel_data);
			sim_travel_ptr = &sim_travel_array.back ();

			sim_travel_ptr->Household (line_array.Vehicle_ID (line_itr->Route (), i));
			veh_index.Household (sim_travel_ptr->Household ());

			//---- process the record ----

			index = (int) sim_veh_array.size ();

			map_stat = sim_veh_map.insert (Vehicle_Map_Data (veh_index, index));

			if (!map_stat.second) {
				Warning (String ("Duplicate Transit Vehicle = %d-%d") % line_itr->Route () % (i + 1));
			} else {
				sim_travel_ptr->Vehicle (index);

				if (run_flag) {
					veh_type_ptr = &veh_type_array [line_itr->Run_Type (i)];
					cells = MAX (((veh_type_ptr->Length () + sim->param.half_cell) / sim->param.cell_size), 1);
				} else {
					cells = line_cells;
				}
				while (cells-- > 0) {
					sim_veh_array.push_back (veh_data);
				}
			}
		}
		if (i == 0) {
			Warning (String ("Transit Route %d has No Scheduled Service") % line_itr->Route ());
		}
	}
}
