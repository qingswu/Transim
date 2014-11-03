//*********************************************************
//	Process_Plan.cpp - filter the plan data
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Process_Plan
//---------------------------------------------------------

bool Relocate::Process_Plan (Plan_Ptr plan_ptr)
{
	int link, dir;
	bool flag;	

	Int_Map_Itr map_itr;
	Location_Data *location_ptr;
	Parking_Data *parking_ptr;
	Link_Data *link_ptr;
	Plan_Leg_Itr leg_itr;

	plan_ptr->Index (0);

	//---- check the selection criteria ----

	if (select_households && !hhold_range.In_Range (plan_ptr->Household ())) return (false);
	if (plan_ptr->Mode () < MAX_MODE && !select_mode [plan_ptr->Mode ()]) return (false);
	if (select_purposes && !purpose_range.In_Range (plan_ptr->Purpose ())) return (false);
	if (select_travelers && !traveler_range.In_Range (plan_ptr->Type ())) return (false);
	if (select_start_times && !start_range.In_Range (plan_ptr->Start ())) return (false);
	if (select_end_times && !end_range.In_Range (plan_ptr->End ())) return (false);
	if (select_origins && !org_range.In_Range (plan_ptr->Origin ())) return (false);
	if (select_destinations && !des_range.In_Range (plan_ptr->Destination ())) return (false);

	flag = false;

	//---- check the selection records ----

	if (select_flag) {
		Select_Map_Itr sel_itr;

		sel_itr = select_map.Best (plan_ptr->Household (), plan_ptr->Person (), plan_ptr->Tour (), plan_ptr->Trip ());
		if (sel_itr == select_map.end ()) return (false);
	}
	map_itr = target_loc_map.find (plan_ptr->Origin ());
	if (map_itr != target_loc_map.end ()) {
		location_ptr = &location_array [map_itr->second];
		plan_ptr->Origin (location_ptr->Location ());
	} else {
		map_itr = location_map.find (plan_ptr->Origin ());
		if (map_itr != location_map.end ()) {
			location_ptr = &location_array [map_itr->second];
			if (location_ptr->Zone () != 1) {
				flag = true;
				goto flag_plan;
			}
		} else {
			flag = true;
			goto flag_plan;
		}
	}
	map_itr = target_loc_map.find (plan_ptr->Destination ());
	if (map_itr != target_loc_map.end ()) {
		location_ptr = &location_array [map_itr->second];
		plan_ptr->Destination (location_ptr->Location ());
	} else {
		map_itr = location_map.find (plan_ptr->Destination ());
		if (map_itr != location_map.end ()) {
			location_ptr = &location_array [map_itr->second];
			if (location_ptr->Zone () != 1) {
				flag = true;
				goto flag_plan;
			}
		} else {
			flag = true;
			goto flag_plan;
		}	
	}

	//---- process plan legs ----

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		if (leg_itr->Type () == LOCATION_ID) {
			map_itr = target_loc_map.find (leg_itr->ID ());
			if (map_itr != target_loc_map.end ()) {
				location_ptr = &location_array [map_itr->second];
				leg_itr->ID (location_ptr->Location ());
			} else {
				map_itr = location_map.find (leg_itr->ID ());
				if (map_itr != location_map.end ()) {
					location_ptr = &location_array [map_itr->second];
					if (location_ptr->Zone () != 1) {
						flag = true;
						break;
					}
				} else {
					flag = true;
					break;
				}
			}
		} else if (leg_itr->Type () == PARKING_ID) {
			map_itr = target_park_map.find (leg_itr->ID ());
			if (map_itr != target_park_map.end ()) {
				parking_ptr = &parking_array [map_itr->second];
				leg_itr->ID (parking_ptr->Parking ());
			} else {
				map_itr = parking_map.find (leg_itr->ID ());
				if (map_itr != parking_map.end ()) {
					parking_ptr = &parking_array [map_itr->second];
					if (parking_ptr->Type () != 1) {
						flag = true;
						break;
					}
				} else {
					flag = true;
					break;
				}
			}
		} else if (leg_itr->Link_Type ()) {
			link = leg_itr->Link_ID ();
			dir = leg_itr->Link_Dir ();

			map_itr = link_map.find (link);
			if (map_itr != link_map.end ()) {
				link_ptr = &link_array [map_itr->second];
				if (link_ptr->Divided () == 0) {
					flag = true;
					break;
				}
			} else {
				flag = true;
				break;
			}
		}
	}
flag_plan:
	if (flag) {
		if (new_select_flag) {
			Trip_Index trip_index;
			Select_Data select_data;
			Select_Map_Stat map_stat;

			plan_ptr->Get_Index (trip_index);

			select_data.Type (plan_ptr->Type ());
			select_data.Partition (plan_ptr->Partition ());

			//---- process the record ----
MAIN_LOCK
			map_stat = select_map.insert (Select_Map_Data (trip_index, select_data));
END_LOCK
			if (!map_stat.second) {
				Warning (String ("Duplicate Selection Record = %d-%d-%d-%d") % 
					trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
			}
		}
		num_problems++;
	}
	flag = (!flag || !delete_flag);

	plan_ptr->Index (flag);
	return (flag);
}
