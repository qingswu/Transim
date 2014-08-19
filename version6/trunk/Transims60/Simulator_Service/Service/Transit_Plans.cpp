//*********************************************************
//	Transit_Plans.cpp - generate transit plans
//*********************************************************

#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Transit_Plans
//---------------------------------------------------------

void Simulator_Service::Transit_Plans (void)
{
	int i, j, index, offset, last_offset, next_offset, traveler, last_leg;
	Dtime time, dwell;
	bool first_flag;

	Line_Itr line_itr;
	Line_Stop_Itr stop_itr;
	Driver_Itr driver_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Stop_Data *stop_ptr;
	Stop_Itr st_itr;
	Sim_Dir_Ptr sim_dir_ptr;
	Sim_Plan_Data sim_plan, *sim_plan_ptr;
	Sim_Leg_Data leg_rec, *leg_ptr, *last_ptr;
	Vehicle_Map_Itr veh_map_itr;
	Vehicle_Index veh_index;
	Veh_Type_Data *veh_type_ptr;
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Connect_Data *connect_ptr;
	Sim_Stop_Data sim_stop_rec, *sim_stop_ptr;
	Integers leg_list;
	Int_RItr int_ritr;

	//---- convert stop offsets to cells ----

	sim_stop_array.assign (stop_array.size (), sim_stop_rec);

	for (i=0, st_itr = stop_array.begin (); st_itr != stop_array.end (); st_itr++, i++) {
		link_ptr = &link_array [st_itr->Link ()];
		offset = st_itr->Offset ();

		if (st_itr->Dir () == 1) {
			index = link_ptr->BA_Dir ();
		} else {
			index = link_ptr->AB_Dir ();
		}
		if (index == 0) {
			Warning (String ("Link %d is Not Available in the %s Direction") % link_ptr->Link () % ((st_itr->Dir () == 0) ? "AB" : "BA"));
			continue;
		}
		dir_ptr = &dir_array [index];
		sim_dir_ptr = &sim_dir_array [index];

		sim_stop_ptr = &sim_stop_array [i];
	
		offset = MIN (MAX (offset, sim_dir_ptr->In_Offset ()), sim_dir_ptr->Out_Offset ());
		sim_stop_ptr->Offset (offset);

		sim_stop_ptr->Min_Lane (dir_ptr->Lanes () + dir_ptr->Left () - 1);
		sim_stop_ptr->Max_Lane (sim_stop_ptr->Min_Lane () + dir_ptr->Right ());
	}

	//---- process each transit route ----

	sim_plan.Clear ();
	transit_plans.assign (line_array.size (), sim_plan);
	transit_legs.reserve (line_array.Route_Stops ());

	for (i=0, line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++, i++) {
		if (line_itr->size () == 0 || line_itr->begin ()->size () == 0) continue;

		//---- create the transit plan ----

		sim_plan_ptr = &transit_plans [i];

		sim_plan_ptr->Mode (TRANSIT_MODE);
		sim_plan_ptr->Vehicle (0);
		last_leg = -1;
		first_flag = true;
		leg_list.clear ();

		traveler = line_array.Vehicle_ID (line_itr->Route (), 0);

		veh_index.Household (traveler);
		veh_index.Vehicle (0);

		veh_map_itr = sim_veh_map.find (veh_index);

		if (veh_map_itr == sim_veh_map.end ()) {
			Warning (String ("Transit Route %d Vehicle was Not Found") % line_itr->Route ());
			continue;
		}
		veh_type_ptr = &veh_type_array [line_itr->Type ()];
		
		dwell = (veh_type_ptr->Min_Dwell () + veh_type_ptr->Max_Dwell ()) / 2;
		if (dwell < Dtime (2, SECONDS)) dwell.Seconds (2);

		time = 0;
		last_offset = 0;
		driver_itr = line_itr->driver_array.begin ();

		for (j=0, stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++, j++) {
			stop_ptr = &stop_array [stop_itr->Stop ()];

			sim_stop_ptr = &sim_stop_array [stop_itr->Stop ()];
			next_offset = sim_stop_ptr->Offset ();

			link_ptr = &link_array [stop_ptr->Link ()];

			if (stop_ptr->Dir () == 0) {
				index = link_ptr->AB_Dir ();
			} else {
				index = link_ptr->BA_Dir ();
			}
			for (; driver_itr != line_itr->driver_array.end (); driver_itr++) {
				sim_dir_ptr = &sim_dir_array [*driver_itr];
				dir_ptr = &dir_array [*driver_itr];

				if (j != 0) {
					leg_rec.Clear ();
					leg_rec.Mode (DRIVE_MODE);
					leg_rec.Type (DIR_ID);
					leg_rec.Index (*driver_itr);
					leg_rec.Max_Speed (sim_dir_ptr->Speed ());
					if (*driver_itr == index) {
						offset = next_offset - last_offset;
					} else {
						offset = sim_dir_ptr->Length () - last_offset;
					}
					leg_rec.Time (dir_ptr->Time0 () * offset / sim_dir_ptr->Length () + 1);
					time += leg_rec.Time ();
					last_leg = transit_legs.Put_Record (leg_rec, last_leg);

					leg_list.push_back (last_leg);

					if (first_flag) {
						sim_plan_ptr->First_Leg (last_leg);
						first_flag = false;
					}
				}
				if (*driver_itr == index) break;
				last_offset = 0;
			}
			last_offset = next_offset;

			leg_rec.Clear ();
			leg_rec.Mode (OTHER_MODE);
			leg_rec.Type (STOP_ID);
			leg_rec.Index (stop_itr->Stop ());
			leg_rec.Stop_Number (j);
			if (j == 0) {
				leg_rec.Time (0);
			} else {
				leg_rec.Time (dwell);
			}
			time += leg_rec.Time ();
			last_leg = transit_legs.Put_Record (leg_rec, last_leg);

			leg_list.push_back (last_leg);

			if (first_flag) {
				sim_plan_ptr->First_Leg (last_leg);
				first_flag = false;
			}
		}
		sim_plan_ptr->End (time);

		//---- set the lane connections ----

		last_ptr = 0;

		for (int_ritr = leg_list.rbegin (); int_ritr != leg_list.rend (); int_ritr++) {
			leg_ptr = transit_legs.Record_Pointer (*int_ritr);
			
			if (leg_ptr->Type () == STOP_ID) {
				sim_stop_ptr = &sim_stop_array [leg_ptr->Index ()];

				leg_ptr->In_Lane_Low (sim_stop_ptr->Min_Lane ());
				leg_ptr->In_Lane_High (sim_stop_ptr->Max_Lane ());
				leg_ptr->In_Best_Low (sim_stop_ptr->Min_Lane ());
				leg_ptr->In_Best_High (sim_stop_ptr->Max_Lane ());

				leg_ptr->Out_Lane_Low (sim_stop_ptr->Min_Lane ());
				leg_ptr->Out_Lane_High (sim_stop_ptr->Max_Lane ());
				leg_ptr->Out_Best_Low (sim_stop_ptr->Min_Lane ());
				leg_ptr->Out_Best_High (sim_stop_ptr->Max_Lane ());

				leg_ptr->Connect (-1);

				if (last_ptr != 0) {
					last_ptr->In_Lane_Low (sim_stop_ptr->Min_Lane ());
					last_ptr->In_Lane_High (sim_stop_ptr->Max_Lane ());
					last_ptr->In_Best_Low (sim_stop_ptr->Min_Lane ());
					last_ptr->In_Best_High (sim_stop_ptr->Max_Lane ());
				}
			} else if (last_ptr != 0) {
				if (last_ptr->Type () == STOP_ID) {
					leg_ptr->Out_Lane_Low (last_ptr->In_Lane_Low ());
					leg_ptr->Out_Lane_High (last_ptr->In_Lane_High ());
					leg_ptr->Out_Best_Low (last_ptr->In_Best_Low ());
					leg_ptr->Out_Best_High (last_ptr->In_Best_High ());

					leg_ptr->Max_Speed (1);
					leg_ptr->Connect (-1);
				} else {

					//---- get the connection to the next link ----

					map2_itr = connect_map.find (Int2_Key (leg_ptr->Index (), last_ptr->Index ()));

					if (map2_itr == connect_map.end ()) {
						if (param.print_problems) {
							dir_ptr = &dir_array [last_ptr->Index ()];
							link_ptr = &link_array [dir_ptr->Link ()];
							index = link_ptr->Link ();

							dir_ptr = &dir_array [leg_ptr->Index ()];
							link_ptr = &link_array [dir_ptr->Link ()];

							Warning (String ("Transit Route %d Connection was Not Found between Links %d and %d") % 
								line_itr->Route () % link_ptr->Link () % index);
						}
						continue;
					}
					connect_ptr = &connect_array [map2_itr->second];

					leg_ptr->Out_Lane_Low (connect_ptr->Low_Lane ());
					leg_ptr->Out_Lane_High (connect_ptr->High_Lane ());
					leg_ptr->Out_Best_Low (connect_ptr->Low_Lane ());
					leg_ptr->Out_Best_High (connect_ptr->High_Lane ());

					last_ptr->In_Lane_Low (connect_ptr->To_Low_Lane ());
					last_ptr->In_Lane_High (connect_ptr->To_High_Lane ());
					last_ptr->In_Best_Low (connect_ptr->To_Low_Lane ());
					last_ptr->In_Best_High (connect_ptr->To_High_Lane ());

					if (connect_ptr->Speed () > 0) {
						leg_ptr->Max_Speed (connect_ptr->Speed ());
					}
					leg_ptr->Connect (map2_itr->second);
				}
			}
			last_ptr = leg_ptr;
		}
	}
}
