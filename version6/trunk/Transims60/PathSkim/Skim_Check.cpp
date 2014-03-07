//*********************************************************
//	Skim_Check.cpp - check path for transit load factors
//*********************************************************

#include "PathSkim.hpp"

//---------------------------------------------------------
//	Skim_Check
//---------------------------------------------------------

bool PathSkim::Skim_Check (Plan_Ptr plan_ptr)
{
	if (plan_ptr == 0) return (false);

	int stop, line, run, type, alight;
	double load;
	Dtime tod, best_time, leg_time, ttime, depart, arrive;

	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_RItr run_ritr;
	Line_Run *run_ptr;
	Plan_Leg_Itr leg_itr;
	Veh_Type_Data *veh_type_ptr;

	stop = line = 0;
	best_time = 0;
	tod = plan_ptr->Depart ();

	for (leg_itr = plan_ptr->begin (); leg_itr != plan_ptr->end (); leg_itr++) {
		if (leg_itr->Type () == STOP_ID && leg_itr->Mode () == WALK_MODE) {
			stop = leg_itr->ID ();
		} else if (leg_itr->Type () == ROUTE_ID && leg_itr->Mode () == WAIT_MODE) {
			line = leg_itr->ID ();
		} else if (leg_itr->Type () == STOP_ID && leg_itr->Mode () == TRANSIT_MODE) {

			map_itr = stop_map.find (stop);
			if (map_itr == stop_map.end ()) continue;
			stop = map_itr->second;

			map_itr = line_map.find (line);
			if (map_itr == line_map.end ()) continue;

			line_ptr = &line_array [map_itr->second];

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
				if (stop_itr->Stop () == stop) {
					run = (int) stop_itr->size () - 1;
					for (run_ritr = stop_itr->rbegin (); run_ritr != stop_itr->rend (); run_ritr++, run--) {
						if (run_ritr->Schedule () < tod) {
							load = run_ritr->Load ();

							if (line_ptr->run_types.size () > 0) {
								type = line_ptr->Run_Type (run);
							} else {
								type = line_ptr->Type ();
							}
							veh_type_ptr = &veh_type_array [type];

							if (veh_type_ptr->Capacity () > 0) {
								load /= veh_type_ptr->Capacity ();
							}
							if (load < load_factor) {
								if (leg_itr->Time () > best_time) {
									best_time = leg_itr->Time ();
								}
							} else {
								alight = leg_itr->ID ();
								map_itr = stop_map.find (alight);
								alight = map_itr->second;

								ttime = leg_itr->Time ();
								depart = run_ritr->Schedule ();

								for (++stop_itr; stop_itr != line_ptr->end (); stop_itr++) {
									run_ptr = &stop_itr->at (run);
									arrive = run_ptr->Schedule ();
									ttime -= (arrive - depart);

									load = run_ptr->Load ();

									if (veh_type_ptr->Capacity () > 0) {
										load /= veh_type_ptr->Capacity ();
									}
									if (load < load_factor) {
										if (ttime > best_time) {
											best_time = ttime;
										}
										break;
									}
									if (stop_itr->Stop () == alight) break;
									depart = arrive;
								}
							}
							break;
						}
					}
					break;
				}
			}
			stop = leg_itr->ID ();
		}
		tod += leg_itr->Time ();
	}
	if (best_time > 0) {
		plan_ptr->Other (best_time);
	}
	return (true);
}
