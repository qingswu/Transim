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
	int mode, last_leg;
	bool drive_flag;

	Link_Data *link_ptr;
	Plan_Leg_Itr leg_itr;
	Sim_Trip_Ptr sim_trip_ptr;
	Sim_Travel_Ptr sim_travel_ptr;
	Sim_Plan_Ptr sim_plan_ptr;
	Sim_Leg_Data sim_leg;
	Integers leg_list;

	num_plans++;		//---- count the number of active trips ----

	sim_trip_ptr = new Sim_Trip_Data ();

	//---- deal with non-simulation legs ----

	mode = plan_ptr->Mode ();

	if (mode == OTHER_MODE || mode == RIDE_MODE) return (sim_trip_ptr);
	if (!sim->param.transit_flag && (mode == TRANSIT_MODE || mode == PNR_IN_MODE || mode == PNR_OUT_MODE || 
		mode == KNR_IN_MODE || mode == KNR_OUT_MODE)) return (sim_trip_ptr);

	//---- check the selection criteria ----

	if (sim->select_households && !sim->hhold_range.In_Range (plan_ptr->Household ())) return (sim_trip_ptr);
	if (!sim->select_mode [plan_ptr->Mode ()]) return (sim_trip_ptr);
	if (sim->select_purposes && !sim->purpose_range.In_Range (plan_ptr->Purpose ())) return (sim_trip_ptr);
	if (sim->select_start_times && !sim->start_range.In_Range (plan_ptr->Depart ())) return (sim_trip_ptr);
	if (sim->select_end_times && !sim->end_range.In_Range (plan_ptr->Arrive ())) return (sim_trip_ptr);

	//---- update the household type ----  //****** Router Service *******

	//if (sim->script_flag || sim->hhfile_flag) {
	//	Int2_Map_Itr map2_itr = sim->hhold_type.find (Int2_Key (plan_ptr->Household (), plan_ptr->Person ()));
	//	if (map2_itr != sim->hhold_type.end ()) {
	//		plan_ptr->Type (map2_itr->second);
	//	}
	//}
	if (sim->select_travelers && !sim->traveler_range.In_Range (plan_ptr->Type ())) return (sim_trip_ptr);
	if (sim->select_vehicles && plan_ptr->Vehicle () > 0) {
		if (!sim->vehicle_range.In_Range (plan_ptr->Veh_Type ())) return (sim_trip_ptr);
	}
	if (plan_ptr->Depart () == 0 && plan_ptr->Arrive () == 0) {
		sim->Warning ("Departure and Arrival Times are Zero");
	}
	if (plan_ptr->Depart () < sim->param.start_time_step) return (sim_trip_ptr);

	if (!sim->router_flag) {
		if (!plan_ptr->Internal_IDs ()) return (sim_trip_ptr);
	}

	//---- process a new travel plan ----

	sim_travel_ptr = &sim_trip_ptr->sim_travel_data;
	sim_plan_ptr = &sim_trip_ptr->sim_plan_data;

	sim_travel_ptr->Next_Event (plan_ptr->Depart ());
	sim_travel_ptr->Household (plan_ptr->Household ());
	sim_travel_ptr->Person (plan_ptr->Person ());
	sim_plan_ptr->Tour (plan_ptr->Tour ());
	sim_plan_ptr->Trip (plan_ptr->Trip ());

	//---- create a new plan record ----

	sim_plan_ptr->Start (plan_ptr->Depart ());
	sim_plan_ptr->End (plan_ptr->Arrive ());
	sim_plan_ptr->Activity (plan_ptr->Activity ());
	sim_plan_ptr->Schedule (plan_ptr->Depart ());

	sim_plan_ptr->Origin (plan_ptr->Origin ());
	sim_plan_ptr->Destination (plan_ptr->Destination ());
	sim_plan_ptr->Vehicle (plan_ptr->Vehicle ());
	sim_plan_ptr->Veh_Type (plan_ptr->Veh_Type ());

	sim_plan_ptr->Purpose (plan_ptr->Purpose ());
	sim_plan_ptr->Mode (plan_ptr->Mode ());
	sim_plan_ptr->Constraint (plan_ptr->Constraint ());
	sim_plan_ptr->Type (plan_ptr->Type ());
	sim_plan_ptr->Leg_Pool (Leg_Pool ());

	//---- process each leg ----

	drive_flag = false;
	last_leg = -1;

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		if (leg_itr->ID () < 0) {
			sim_travel_ptr->Household (0);
			return (sim_trip_ptr);
		}
		switch (leg_itr->Type ()) {
			case LINK_AB:
			case USE_AB:
			case LINK_BA:
			case USE_BA:
				link_ptr = &sim->link_array [leg_itr->ID ()];

				if (leg_itr->Link_Dir ()) {
					leg_itr->ID (link_ptr->BA_Dir ());
				} else {
					leg_itr->ID (link_ptr->AB_Dir ());
				}
				if (leg_itr->ID () < 0) {
					sim_travel_ptr->Household (0);
					return (sim_trip_ptr);
				}
				leg_itr->Type (DIR_ID);
				break;
			case USE_ID:
				leg_itr->Type (DIR_ID);
				break;
			default:
				break;
		}
		sim_leg.Mode (leg_itr->Mode ());
		if (sim_leg.Mode () == DRIVE_MODE) drive_flag = true;

		sim_leg.Type (leg_itr->Type ());
		if (sim_leg.Type () == ROUTE_ID) sim->stats.num_transit++;

		sim_leg.Time (leg_itr->Time ());
		sim_leg.Index (leg_itr->ID ());

		last_leg = sim_plan_ptr->Add_Leg (sim_leg, last_leg);

		leg_list.push_back (last_leg);
	}
	sim->stats.num_trips++;
	if (drive_flag) {
		sim->stats.num_veh_trips++;

		Best_Lanes (sim_trip_ptr, leg_list);
	}
	return (sim_trip_ptr);
}
