//*********************************************************
//	Read_Trips.cpp - read the travel trip file
//*********************************************************

#include "TripSum.hpp"

#include "Shape_Tools.hpp"
#include <math.h>

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void TripSum::Trip_Processing::Read_Trips (int part)
{
	int period, num_periods, code, prev_purp, increment, node;
	double ttime, distance, dx, dy;
	Dtime tod;

	Select_Map_Itr sel_itr;
	Trip_Data trip_data;
	Int_Map_Itr map_itr;
	Int_Map_Stat map_stat;
	Location_Data *location_ptr;
	Integers *ints_ptr;
	Trip_Index prev_index, trip_index;
	Link_Data *link_ptr;
	Node_Data *node_ptr;

	num_periods = exe->sum_periods.Num_Periods ();
	prev_purp = 0;

	//---- open the file partition ----

	if (!file->Open (part)) {
		MAIN_LOCK 
		exe->Error (String ("Opening %s") % file->Filename ()); 
		END_LOCK
	}

	if (thread_flag) {
		MAIN_LOCK
		if (file->Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d") % file->File_Type () % file->Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s") % file->File_Type ());
		}
		END_LOCK
	} else {
		if (file->Part_Flag ()) {
			exe->Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % file->Part_Number ());
		} else {
			exe->Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		exe->Set_Progress ();
	}

	//---- read the trip file ----

	while (file->Read_Trip (trip_data)) {
		if (thread_flag) {
			exe->Show_Dot ();
		} else {
			exe->Show_Progress ();
		}

		//---- check the selection records ----

		if (exe->select_flag) {
			sel_itr = exe->select_map.Best (trip_data.Household (), trip_data.Person (), 
				trip_data.Tour (), trip_data.Trip ());

			if (sel_itr == exe->select_map.end ()) continue;

			if (trip_data.Type () == 0) {
				trip_data.Type (sel_itr->second.Type ());
			}
		}

		//---- check the selection criteria ----

		if (exe->select_households && !exe->hhold_range.In_Range (trip_data.Household ())) continue;
		if (trip_data.Mode () < MAX_MODE && !exe->select_mode [trip_data.Mode ()]) continue;
		if (exe->select_purposes && !exe->purpose_range.In_Range (trip_data.Purpose ())) continue;
		if (exe->select_priorities || !exe->select_priority [trip_data.Priority ()]) continue;
		if (exe->select_travelers && !exe->traveler_range.In_Range (trip_data.Type ())) continue;
		if (exe->select_vehicles && !exe->vehicle_range.In_Range (trip_data.Veh_Type ())) continue;
		if (exe->select_start_times && !exe->start_range.In_Range (trip_data.Start ())) continue;
		if (exe->select_end_times && !exe->end_range.In_Range (trip_data.End ())) continue;
		if (exe->select_origins && !exe->org_range.In_Range (trip_data.Origin ())) continue;
		if (exe->select_destinations && !exe->des_range.In_Range (trip_data.Destination ())) continue;

		if (exe->select_subareas || exe->select_polygon) {
			map_itr = exe->location_map.find (trip_data.Origin ());
			if (map_itr != exe->location_map.end ()) {
				location_ptr = &exe->location_array [map_itr->second];
				link_ptr = &exe->link_array [location_ptr->Link ()];

				if (location_ptr->Dir () == 0) {
					node = link_ptr->Anode ();
				} else {
					node = link_ptr->Bnode ();
				}
				node_ptr = &exe->node_array [node];

				if (exe->select_subareas && !exe->subarea_range.In_Range (node_ptr->Subarea ())) continue;

				if (exe->select_polygon) {
					if (!In_Polygon (exe->polygon_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) continue;
				}
			}
		}

		if (exe->percent_flag && exe->random.Probability () > exe->select_percent) continue;

		//---- summarize trips by time period ----

		if (exe->time_flag) {
			period = exe->sum_periods.Period (trip_data.Start ());
			if (period >= 0) (*start_ptr) [period]++;

			period = exe->sum_periods.Period (trip_data.End ());
			if (period >= 0) (*end_ptr) [period]++;

			period = exe->sum_periods.Period ((trip_data.Start () + trip_data.End ()) / 2);
			if (period >= 0) (*mid_ptr) [period]++;
		}

		//---- summarize trips by link ----

		if (exe->link_flag) {
			period = exe->sum_periods.Period (trip_data.Start ());
			if (period >= 0) {
				map_itr = exe->location_map.find (trip_data.Origin ());
				if (map_itr != exe->location_map.end ()) {
					location_ptr = &exe->location_array [map_itr->second];

					ints_ptr = &(*link_trip_ptr) [location_ptr->Link ()];
					(*ints_ptr) [period * 2]++;
				}
			}
			period = exe->sum_periods.Period (trip_data.End ());
			if (period >= 0) {
				map_itr = exe->location_map.find (trip_data.Destination ());
				if (map_itr != exe->location_map.end ()) {
					location_ptr = &exe->location_array [map_itr->second];

					ints_ptr = &(*link_trip_ptr) [location_ptr->Link ()];
					(*ints_ptr) [period * 2 + 1]++;
				}
			}
		}

		//---- summarize trips by location ----

		if (exe->loc_trip_flag) {
			period = exe->sum_periods.Period (trip_data.Start ());
			if (period >= 0) {
				map_itr = exe->location_map.find (trip_data.Origin ());
				if (map_itr != exe->location_map.end ()) {
					ints_ptr = &(*loc_trip_ptr) [map_itr->second];
					(*ints_ptr) [period * 2]++;
				}
			}
			period = exe->sum_periods.Period (trip_data.End ());
			if (period >= 0) {
				map_itr = exe->location_map.find (trip_data.Destination ());
				if (map_itr != exe->location_map.end ()) {
					ints_ptr = &(*loc_trip_ptr) [map_itr->second];
					(*ints_ptr) [period * 2 + 1]++;
				}
			}
		}

		//---- summarize trips by zone ----

		if (exe->zone_trip_flag) {
			period = exe->sum_periods.Period (trip_data.Start ());
			if (period >= 0) {
				map_itr = exe->location_map.find (trip_data.Origin ());
				if (map_itr != exe->location_map.end ()) {
					location_ptr = &exe->location_array [map_itr->second];

					if (location_ptr->Zone () >= 0) {
						ints_ptr = &(*zone_trip_ptr) [location_ptr->Zone ()];
						(*ints_ptr) [period * 2]++;
					}
				}
			}
			period = exe->sum_periods.Period (trip_data.End ());
			if (period >= 0) {
				map_itr = exe->location_map.find (trip_data.Destination ());
				if (map_itr != exe->location_map.end ()) {
					location_ptr = &exe->location_array [map_itr->second];

					if (location_ptr->Zone () >= 0) {
						ints_ptr = &(*zone_trip_ptr) [location_ptr->Zone ()];
						(*ints_ptr) [period * 2 + 1]++;
					}
				}
			}
		}

		//---- summarize trips by trip table ----

		if (exe->trip_table_flag) {
			int org, des;
			Zone_Data *zone_ptr;

			tod = (trip_data.Start () + trip_data.End ()) / 2;

			period = exe->sum_periods.Period (tod);

			if (period >= 0) {
				org = des = 0;
				map_itr = exe->location_map.find (trip_data.Origin ());
				if (map_itr != exe->location_map.end ()) {
					location_ptr = &exe->location_array [map_itr->second];

					if (location_ptr->Zone () >= 0) {
						zone_ptr = &exe->zone_array [location_ptr->Zone ()];
						org = zone_ptr->Zone ();
					}
				}
				map_itr = exe->location_map.find (trip_data.Destination ());
				if (map_itr != exe->location_map.end ()) {
					location_ptr = &exe->location_array [map_itr->second];

					if (location_ptr->Zone () >= 0) {
						zone_ptr = &exe->zone_array [location_ptr->Zone ()];
						des = zone_ptr->Zone ();
					}
				}
				if (org > 0 && des > 0) {
					org = exe->trip_table_file->Add_Org (org);
					des = exe->trip_table_file->Add_Des (des);

					if (org >= 0 && des >= 0) {
						exe->trip_table_file->Add_Cell_Index (period, org, des, 0, 1);
					}
				}
			}
		}

		if (exe->time_file_flag || exe->time_report) {
			ttime = trip_data.End () - trip_data.Start ();
			increment = (int) (ttime / exe->time_increment + 0.5);

			map_stat = time_inc_ptr->insert (Int_Map_Data (increment, 1));
			if (!map_stat.second) {
				map_stat.first->second++;
			}
		}

		//---- sum the trip length data ----

		if (exe->length_flag) {
			ttime = trip_data.End () - trip_data.Start ();
			dx = dy = 0;

			map_itr = exe->location_map.find (trip_data.Origin ());
			if (map_itr != exe->location_map.end ()) {
				location_ptr = &exe->location_array [map_itr->second];
				dx = location_ptr->X ();
				dy = location_ptr->Y ();
			}
			map_itr = exe->location_map.find (trip_data.Destination ());
			if (map_itr != exe->location_map.end ()) {
				location_ptr = &exe->location_array [map_itr->second];
				dx -= location_ptr->X ();
				dy -= location_ptr->Y ();
			}
			if (exe->distance_type == STRAIGHT) {
				distance = sqrt (dx * dx + dy * dy);
			} else if (exe->distance_type == RIGHT_ANGLE) {
				distance = abs (dx) + abs (dy);
			} else {
				distance = (abs (dx) + abs (dy) + sqrt (dx * dx + dy * dy)) / 2.0;
			}
			if (exe->len_file_flag) {
				increment = (int) (distance / exe->len_increment + 0.5);

				map_stat = len_inc_ptr->insert (Int_Map_Data (increment, 1));
				if (!map_stat.second) {
					map_stat.first->second++;
				}
			}
			if (exe->trip_len_flag) {
				trip_len_ptr->Add_Trip (trip_data.Start (), distance, ttime);
			}
			if (exe->trip_purp_flag) {
				trip_data.Get_Index (trip_index);

				if (prev_index.Household () != trip_index.Household () || prev_index.Person () != trip_index.Person () || prev_index.Tour () != trip_index.Tour ()) {
					prev_purp = 0;
				}
				code = (prev_purp << 16) + trip_data.Purpose ();

				trip_purp_ptr->Add_Trip (code, distance, ttime);

				prev_index = trip_index;
				prev_purp = trip_data.Purpose ();
			}
			if (exe->mode_len_flag) {
				mode_len_ptr->Add_Trip (trip_data.Mode (), distance, ttime);
			}
			if (exe->mode_purp_flag) {
				code = (trip_data.Mode () << 16) + trip_data.Purpose ();

				mode_purp_ptr->Add_Trip (code, distance, ttime);
			}
		}
	}
	if (!thread_flag) exe->End_Progress ();

	file->Close ();
}
