//*********************************************************
//	Convert_Plan.cpp - convert to subarea plan
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Convert_Plan
//---------------------------------------------------------

void SubareaPlans::Convert_Plan (Plan_Data &plan)
{
	int mode, index, length, cost, impedance, dir;
	bool drive_flag, mode_flag, first_flag, found_flag;
	Dtime time, tod;
	bool new_flag, sub_flag;

	Plan_Data sub_plan;
	Plan_Leg leg_rec, *leg_ptr;
	Plan_Leg_Itr leg_itr;
	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Boundary_Link *data_ptr;

	mode = plan.Mode ();

	drive_flag = (mode != WAIT_MODE && mode != WALK_MODE && mode != BIKE_MODE && 
				mode != TRANSIT_MODE && mode != OTHER_MODE);

	if (!transit_flag) {
		if (mode == TRANSIT_MODE) return;
		mode_flag = (mode == PNR_OUT_MODE || mode == PNR_IN_MODE ||
			mode == KNR_OUT_MODE || mode == KNR_IN_MODE);

		if (mode_flag) {
			first_flag = (mode == PNR_IN_MODE || mode == KNR_IN_MODE);

			//---- keep the drive segments ----

			plan.Mode (DRIVE_MODE);
			plan.Drive (0);
			plan.Transit (0);
			plan.Walk (0);
			plan.Wait (0);
			plan.Other (0);
			plan.Length (0);
			plan.Cost (0);
			plan.Impedance (0);

			tod = plan.Depart ();
			found_flag = false;

			for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++, tod += time) {
				time = leg_itr->Time ();
				length = leg_itr->Length ();
				cost = leg_itr->Cost ();
				impedance = leg_itr->Impedance ();

				if (leg_itr->Mode () == TRANSIT_MODE || leg_itr->Mode () == WAIT_MODE) continue;
				if (leg_itr->Mode () == WALK_MODE && leg_itr->Type () != LOCATION_ID) continue;
				
				if (leg_itr->Mode () == DRIVE_MODE) {
					plan.Add_Drive (time);
				} else if (leg_itr->Mode () == WALK_MODE) {
					plan.Add_Walk (time);
				} else {
					plan.Add_Other (time);
				}
				plan.Add_Length (length);
				plan.Add_Cost (cost);
				plan.Add_Impedance (impedance);

				if (leg_itr->Type () == PARKING_ID) {
					map_itr = pnr_loc.find (leg_itr->ID ());
					if (map_itr != pnr_loc.end ()) {
						leg_rec.ID (map_itr->second);
					} else {
						leg_rec.ID (leg_itr->ID ());
					}
					leg_rec.Type (LOCATION_ID);
					leg_rec.Mode (OTHER_MODE);
					leg_rec.Time (0);
					leg_rec.Length (0);
					leg_rec.Cost (0);
					leg_rec.Impedance (0);

					if (first_flag) {
						if (!found_flag) {
							sub_plan.push_back (leg_rec);
						}
						found_flag = true;
					} else {
						sub_plan.push_back (*leg_itr);
						sub_plan.push_back (leg_rec);
						break;
					}
				}
				sub_plan.push_back (*leg_itr);
			}
			plan.swap (sub_plan);
			sub_plan.clear ();
		}
	} else {
		mode_flag = false;
	}

	if (drive_flag) {

		//---- scan for missing links ----

		new_flag = sub_flag = false;
	
		for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++) {
			if (!leg_itr->Link_Type ()) continue;

			index = leg_itr->Link_ID ();
			map_itr = link_map.find (index);

			if (map_itr == link_map.end ()) {
				new_flag = true;
				if (sub_flag) break;
			} else {
				sub_flag = true;
			}
		}

		//---- adjust the current plan ----

		if (new_flag && sub_flag) {
			sub_plan.Household (plan.Household ());
			sub_plan.Person (plan.Person ());
			sub_plan.Tour (plan.Tour ());
			sub_plan.Trip (plan.Trip ());
			sub_plan.Start (plan.Start ());
			sub_plan.End (plan.End ());
			sub_plan.Duration (plan.Duration ());
			sub_plan.Origin (plan.Origin ());
			sub_plan.Destination (plan.Destination ());
			sub_plan.Purpose (plan.Purpose ());
			sub_plan.Mode (plan.Mode ());
			sub_plan.Constraint (plan.Constraint ());
			sub_plan.Priority (plan.Priority ());
			sub_plan.Vehicle (plan.Vehicle ());
			sub_plan.Veh_Type (plan.Veh_Type ());
			sub_plan.Type (plan.Type ());
			sub_plan.Partition (plan.Partition ());

			sub_plan.Depart (plan.Depart ());
			sub_plan.Arrive (plan.Arrive ());
			sub_plan.Activity (plan.Activity ());

			tod = plan.Depart ();
			sub_flag = new_flag = false;

			for (leg_itr = plan.begin (); leg_itr != plan.end (); leg_itr++, tod += time) {
				time = leg_itr->Time ();
				length = leg_itr->Length ();
				cost = leg_itr->Cost ();
				impedance = leg_itr->Impedance ();

				if (leg_itr->Link_Type ()) {
					index = leg_itr->Link_ID ();
					map_itr = link_map.find (index);

					if (map_itr == link_map.end ()) {
						new_flag = true;

						if (sub_flag) {

							//---- end the trip ----

							sub_flag = false;
							sub_plan.End (tod.Round_Seconds ());
							sub_plan.Arrive (tod.Round_Seconds ());

							leg_ptr = &sub_plan.back ();

							if (leg_ptr->Link_Type ()) {
								map_itr = link_map.find (leg_ptr->Link_ID ());

								if (map_itr == link_map.end ()) {
									Warning ("Plan Link=") << leg_ptr->Link_ID () << " is Not in the Network";
									return;
								}
								link_ptr = &link_array [map_itr->second];

								if (leg_ptr->Link_Dir () == 0) {
									dir = link_ptr->AB_Dir ();
								} else {
									dir = link_ptr->BA_Dir ();
								}
								if (dir < 0) {
									Warning ("Plan Link=") << leg_ptr->Link_ID () << " Direction=" << leg_ptr->Link_Dir () << " Not in the Network";
									return;
								}
								data_ptr = &sublink_array [dir];

								leg_rec.ID (data_ptr->parking);
								leg_rec.Type (PARKING_ID);
								leg_rec.Mode (OTHER_MODE);

								sub_plan.push_back (leg_rec);

								sub_plan.Destination (data_ptr->location);

								leg_rec.ID (data_ptr->location);
								leg_rec.Type (LOCATION_ID);
								leg_rec.Mode (WALK_MODE);

								sub_plan.push_back (leg_rec);
							}
							break;	//---- or next trip ----
						} else {
							continue;
						}
					} else if (!sub_flag) {
						sub_flag = true;

						//---- start the trip ----

						if (new_flag) {
							link_ptr = &link_array [map_itr->second];
							if (leg_itr->Link_Dir () == 0) {
								data_ptr = &sublink_array [link_ptr->AB_Dir ()];
							} else {
								data_ptr = &sublink_array [link_ptr->BA_Dir ()];
							}
							sub_plan.Start (tod.Round_Seconds ());
							sub_plan.Depart (tod.Round_Seconds ());
							sub_plan.Origin (data_ptr->location);

							leg_rec.ID (data_ptr->location);
							leg_rec.Type (LOCATION_ID);
							leg_rec.Mode (WALK_MODE);

							sub_plan.push_back (leg_rec);

							leg_rec.ID (data_ptr->parking);
							leg_rec.Type (PARKING_ID);
							leg_rec.Mode (OTHER_MODE);

							sub_plan.push_back (leg_rec);
						}
					}
				}
				if (sub_flag) {
					sub_plan.push_back (*leg_itr);

					switch (leg_itr->Mode ()) {
						case DRIVE_MODE:
							sub_plan.Add_Drive (time);
							break;
						case TRANSIT_MODE:
							sub_plan.Add_Transit (time);
							break;
						case WALK_MODE:
							sub_plan.Add_Walk (time);
							break;
						case WAIT_MODE:
							sub_plan.Add_Wait (time);
							break;
						default:
							sub_plan.Add_Other (time);
							break;
					}
					sub_plan.Add_Length (length);
					sub_plan.Add_Cost (cost);
					sub_plan.Add_Impedance (impedance);
				}
			}

			//---- save the plan ----

			new_plan_file->Write_Plan (sub_plan);

			if (trip_flag) {
				new_trip_file->Write_Trip (sub_plan);
			}

		} else if (sub_flag) {

			//---- write the existing plan ----

			new_plan_file->Write_Plan (plan);

			if (trip_flag) {
				new_trip_file->Write_Trip (plan);
			}
		}
	}
}
