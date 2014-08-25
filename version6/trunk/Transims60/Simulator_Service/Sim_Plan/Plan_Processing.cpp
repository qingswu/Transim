//*********************************************************
//	Plan_Processing - select travel plans for processing
//*********************************************************

#include "Sim_Plan_Process.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Plan_Processing
//---------------------------------------------------------

Sim_Trip_Ptr Sim_Plan_Process::Plan_Processing (Plan_Data *plan_ptr)
{
	int index, mode, type, dir, leg, last_leg;
	string label;
	bool drive_flag;

	Plan_Leg_Itr leg_itr;
	Sim_Trip_Ptr sim_trip_ptr;
	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Data sim_leg;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Integers leg_list;

	num_plans++;		//---- count the number of active trips ----

	//---- deal with non-simulation legs ----

	mode = plan_ptr->Mode ();

	if (mode == OTHER_MODE || mode == RIDE_MODE) goto skip;
	if (!sim->param.transit_flag && (mode == TRANSIT_MODE || mode == PNR_IN_MODE || mode == PNR_OUT_MODE || 
		mode == KNR_IN_MODE || mode == KNR_OUT_MODE)) goto skip;

	//---- check the selection criteria ----

	if (sim->select_households && !sim->hhold_range.In_Range (plan_ptr->Household ())) goto skip;
	if (!sim->select_mode [plan_ptr->Mode ()]) goto skip;
	if (sim->select_purposes && !sim->purpose_range.In_Range (plan_ptr->Purpose ())) goto skip;
	if (sim->select_start_times && !sim->start_range.In_Range (plan_ptr->Depart ())) goto skip;
	if (sim->select_end_times && !sim->end_range.In_Range (plan_ptr->Arrive ())) goto skip;

	//---- update the household type ----  //****** Router Service *******

	//if (sim->script_flag || sim->hhfile_flag) {
	//	Int2_Map_Itr map2_itr = sim->hhold_type.find (Int2_Key (plan_ptr->Household (), plan_ptr->Person ()));
	//	if (map2_itr != sim->hhold_type.end ()) {
	//		plan_ptr->Type (map2_itr->second);
	//	}
	//}
	if (sim->select_travelers && !sim->traveler_range.In_Range (plan_ptr->Type ())) goto skip;
	if (sim->select_vehicles && plan_ptr->Vehicle () > 0) {
		if (!sim->vehicle_range.In_Range (plan_ptr->Veh_Type ())) goto skip;
	}
	if (plan_ptr->Depart () == 0 && plan_ptr->Arrive () == 0) {
		sim->Warning ("Departure and Arrival Times are Zero");
	}
	if (plan_ptr->Depart () < sim->param.start_time_step) goto skip;

	//---- process a new travel plan ----

	index = 0;

	sim_trip_ptr = new Sim_Trip_Data ();

	sim_travel_ptr = &sim_trip_ptr->sim_travel_data;
	sim_plan_ptr = &sim_trip_ptr->sim_plan_data;

	sim_travel_ptr->Household (plan_ptr->Household ());
	sim_travel_ptr->Person (plan_ptr->Person ());
	sim_plan_ptr->Tour (plan_ptr->Tour ());
	sim_plan_ptr->Trip (plan_ptr->Trip ());

	//---- create a new plan record ----

	sim_plan_ptr->Start (plan_ptr->Depart ());
	sim_plan_ptr->End (plan_ptr->Arrive ());
	sim_plan_ptr->Activity (plan_ptr->Activity ());
	sim_plan_ptr->Schedule (sim_plan_ptr->Start ());

	//---- convert the origin ----

	index = plan_ptr->Origin ();

	map_itr = sim->location_map.find (index);

	if (map_itr == sim->location_map.end ()) {
		label = "Origin";
		goto clean_up;
	}
	sim_plan_ptr->Origin (map_itr->second);

	//---- convert the destination ----

	index = plan_ptr->Destination ();

	map_itr = sim->location_map.find (index);

	if (map_itr == sim->location_map.end ()) {
		label = "Destination";
		goto clean_up;
	}
	sim_plan_ptr->Destination (map_itr->second);

	sim_plan_ptr->Vehicle (plan_ptr->Vehicle ());

	//---- convert the vehicle type ----

	index = plan_ptr->Veh_Type ();

	if (index > 0) {
		map_itr = sim->veh_type_map.find (index);
		if (map_itr == sim->veh_type_map.end ()) {
			label = "Vehicle Type";
			goto clean_up;
		}
		sim_plan_ptr->Veh_Type (map_itr->second);
	}
	sim_plan_ptr->Purpose (plan_ptr->Purpose ());
	sim_plan_ptr->Mode (plan_ptr->Mode ());
	sim_plan_ptr->Constraint (plan_ptr->Constraint ());
	sim_plan_ptr->Type (plan_ptr->Type ());
	sim_plan_ptr->Leg_Pool (Leg_Pool ());

	//---- process each leg ----

	drive_flag = false;
	last_leg = -1;

	for (leg = 0, leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++, leg++) {

		type = leg_itr->Type ();
		index = leg_itr->ID ();

		mode = leg_itr->Mode ();
		if (mode == DRIVE_MODE) drive_flag = true;

		sim_leg.Mode (mode);
		sim_leg.Type (type);
		sim_leg.Time (leg_itr->Time ());

		switch (type) {
			case LOCATION_ID:
				map_itr = sim->location_map.find (index);

				if (map_itr == sim->location_map.end ()) {
					label = "Location";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case PARKING_ID:
				map_itr = sim->parking_map.find (index);

				if (map_itr == sim->parking_map.end ()) {
					label = "Parking Lot";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case STOP_ID:
				map_itr = sim->stop_map.find (index);

				if (map_itr == sim->stop_map.end ()) {
					label = "Stop";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case NODE_ID:
				map_itr = sim->node_map.find (index);

				if (map_itr == sim->node_map.end ()) {
					label = "Node";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case ACCESS_ID:
			case ACCESS_AB:
			case ACCESS_BA:
				index = leg_itr->Access_ID ();
				dir = leg_itr->Access_Dir ();

				map_itr = sim->access_map.find (index);

				if (map_itr == sim->access_map.end ()) {
					label = "Access";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			case ROUTE_ID:
				map_itr = sim->line_map.find (index);

				if (map_itr == sim->line_map.end ()) {
					label = "Route";
					goto clean_up;
				}
				sim->stats.num_transit++;
				index = map_itr->second;
				break;
			case LINK_ID:
			case LINK_AB:
			case LINK_BA:
			case USE_AB:
			case USE_BA:
				index = leg_itr->Link_ID ();
				dir = leg_itr->Link_Dir ();

				map_itr = sim->link_map.find (index);

				if (map_itr == sim->link_map.end ()) {
					label = "Link";
					goto clean_up;
				}
				index = map_itr->second;

				if (mode == DRIVE_MODE) {
					link_ptr = &sim->link_array [index];
					if (dir == 1) {
						index = link_ptr->BA_Dir ();
					} else {
						index = link_ptr->AB_Dir ();
					}
					sim_leg.Type (DIR_ID);

					if (index < 0) {
						index = leg_itr->ID ();
						label = "Link Direction";
						goto clean_up;
					}
				}
				break;
			case DIR_ID:
				map_itr = sim->dir_map.find (index);

				if (map_itr == sim->dir_map.end ()) {
					label = "Link Direction";
					goto clean_up;
				}
				index = map_itr->second;
				break;
			default:
				label = "Type";
				index = type;
				goto clean_up;
		}
		sim_leg.Index (index);

		last_leg = sim_plan_ptr->Add_Leg (sim_leg, last_leg);

		leg_list.push_back (last_leg);
	}
	sim->stats.num_trips++;
	if (drive_flag) {
		sim->stats.num_veh_trips++;

		if (!Best_Lanes (sim_trip_ptr, leg_list)) {
			//step.Problem (CONNECT_PROBLEM);
			//step.Status (2);
			//Output_Step (step);
			//sim_travel_ptr->Next_Plan ();
		}
	}
	return (sim_trip_ptr);

clean_up:
	sim->Warning (String ("Plan %d-%d-%d-%d %s %d was Not Found") % plan_ptr->Household () % 
		plan_ptr->Person () % plan_ptr->Tour () % plan_ptr->Trip () % label % index);
	return (sim_trip_ptr);

skip:
	return (new Sim_Trip_Data ());
}
