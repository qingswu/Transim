//*********************************************************
//	Process_Plan.cpp - filter the plan data
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Process_Plan
//---------------------------------------------------------

bool Relocate::Process_Plan (Plan_Ptr plan_ptr)
{
	int link, dir, dir1, dir2;
	bool flag, copy_flag, leg_flag;	
	Use_Type use;

	Int_Map_Itr map_itr;
	Location_Data *location_ptr;
	Parking_Data *parking_ptr;
	Access_Data *access_ptr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Plan_Leg_Itr leg_itr;
	Plan_Leg_Array leg_array;

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
<<<<<<< .working

	flag = false;

=======

	flag = false;
	copy_flag = true;
	leg_flag = false;

>>>>>>> .merge-right.r1529
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

<<<<<<< .working
=======
	dir1 = -1;

>>>>>>> .merge-right.r1529
	switch (plan_ptr->Mode ()) {
		case WALK_MODE:		//---- walk ----
			use = WALK;
			break;
		case BIKE_MODE:		//---- bike ----
			use = BIKE;
			break;
		case HOV2_MODE:		//---- carpool 2+ ----
			use = HOV2;
			break;
		case HOV3_MODE:		//---- carpool 3+ ----
			use = HOV3;
			break;
		case HOV4_MODE:		//---- carpool 4+ ----
			use = HOV4;
			break;
		default:
			use = CAR;
	}

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

				if (link_ptr->Divided () == 0 || link_ptr->Divided () == 3) {
					copy_flag = false;
//Write (1, " link=") << link_ptr->Link () << " divided=" << link_ptr->Divided ();
				} else {
					if (!copy_flag) {
						Plan_Leg leg_data;
						Integers path_array;
						Int_RItr ritr;
//Write (1, " to link=") << link_ptr->Link () << " divided=" << link_ptr->Divided ();

						dir2 = (dir == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();

						if (!Dir_Path (dir1, dir2, use, path_array)) {
//Write (1, " path failed");
							flag = true;
							break;
						}
//Write (1, "GOOD hhold=") << plan_ptr->Household ();
						leg_data.Mode (leg_itr->Mode ());

						for (ritr = path_array.rbegin (); ritr != path_array.rend (); ritr++) {
							if (*ritr == dir2) break;

							dir_ptr = &dir_array [*ritr];
							link_ptr = &link_array [dir_ptr->Link ()];

							leg_data.Type ((dir_ptr->Dir () == 1) ? LINK_BA : LINK_AB);
							leg_data.ID (link_ptr->Link ());
							leg_data.Time (dir_ptr->Time0 ());
							leg_data.Length (link_ptr->Length ());
							leg_data.Cost (0);
							leg_data.Impedance (dir_ptr->Time0 ());

//Write (1, " save=") << *ritr << " link=" << link_ptr->Link ();

							leg_array.push_back (leg_data);
							leg_flag = true;
						}
					}
					copy_flag = true;
					dir1 = (dir == 0) ? link_ptr->AB_Dir () : link_ptr->BA_Dir ();
				}
			} else {
				flag = true;
				break;
			}

		} else if (leg_itr->Access_Type ()) {
			link = leg_itr->Access_ID ();
			dir = leg_itr->Access_Dir ();

			map_itr = target_access_map.find (link);
			if (map_itr != target_access_map.end ()) {
				access_ptr = &access_array [map_itr->second];
				leg_itr->ID (access_ptr->Link ());
			} else {
				map_itr = access_map.find (link);
				if (map_itr != access_map.end ()) {
					access_ptr = &access_array [map_itr->second];
					if (access_ptr->Cost () != 1) {
						flag = true;
						break;
					}
				} else {
					flag = true;
					break;
				}
			}
		}
		if (copy_flag) {
			leg_array.push_back (*leg_itr);
		}
	}
<<<<<<< .working
flag_plan:
	if (flag) {
=======
flag_plan:
	if (flag || !copy_flag) {
>>>>>>> .merge-right.r1529
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
	} else if (leg_flag) {
		plan_ptr->swap (leg_array);
	}
	flag = (!flag || !delete_flag);

	plan_ptr->Index (flag);
	return (flag);
}
