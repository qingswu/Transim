//*********************************************************
//	Sort_Tours.cpp - sort and write household tours
//*********************************************************

#include "TripPrep.hpp"
#include "Tour_Sort.hpp"

//---------------------------------------------------------
//	Sort_Tours
//---------------------------------------------------------

void TripPrep::Trip_Processing::Sort_Tours (void)
{
	int i, home, tour, trip, person, vehicle;
	bool first_flag;

	Trip_Data *trip_ptr, *last_trip;
	Trip_Ptr_Itr trip_itr;
	Tour_Sort sort_data;
	Tour_Sort_Map sort_map;
	Tour_Sort_Map_Stat sort_stat;
	Tour_Sort_Map_Itr sort_itr;
	Vehicle_Index veh_index;
	Int_Map_Itr map_itr;

	//---- sort the person

	for (i=0, trip_itr = trip_ptr_array.begin (); trip_itr != trip_ptr_array.end (); trip_itr++, i++) {
		trip_ptr = *trip_itr;

		sort_data.Set (trip_ptr->Person (), trip_ptr->Start (), trip_ptr->End (), trip_ptr->Tour (), trip_ptr->Trip ());

		sort_stat = sort_map.insert (Tour_Sort_Map_Data (sort_data, i));

		if (!sort_stat.second) {
			exe->Warning (String ("Duplicate Tour Sort (%d-%d-%d-%d)") %
				trip_ptr->Household () % trip_ptr->Person () % trip_ptr->Tour () % trip_ptr->Trip ());
		}
	}

	//---- renumber the person tours and trips ----

	last_trip = 0;
	home = tour = trip = person = 0;

	for (sort_itr = sort_map.begin (); sort_itr != sort_map.end (); sort_itr++) {
		trip_ptr = trip_ptr_array [sort_itr->second];
		if (!trip_ptr) continue;

		if (person != trip_ptr->Person ()) {
			person = trip_ptr->Person ();
			tour = trip = 1;
			home = trip_ptr->Origin ();
			last_trip = 0;
		} else if (trip_ptr->Origin () == home) {
			tour++;
			trip = 1;
		} else {
			trip++;
		}
		if (last_trip && trip_ptr) {
			last_trip->Duration (trip_ptr->Start () - last_trip->End ());
		}
		if (trip_ptr) {
			trip_ptr->Tour (tour);
			trip_ptr->Trip (trip);
		}
		last_trip = trip_ptr;
	}

	//---- save the sorted tours ----

	vehicle = person = 0;
	first_flag = true;
	
	for (sort_itr = sort_map.begin (); sort_itr != sort_map.end (); sort_itr++) {
		trip_ptr = trip_ptr_array [sort_itr->second];

		if (exe->make_veh_flag) {
			if (person != trip_ptr->Person ()) {
				person = trip_ptr->Person ();
				first_flag = true;
			}
			if (trip_ptr->Mode () == DRIVE_MODE) {
				if (first_flag) {
					first_flag = false;
					vehicle++;

					map_itr = exe->location_parking.find (trip_ptr->Origin ());
				}
				trip_ptr->Vehicle (vehicle);
			} else {
				trip_ptr->Vehicle (0);
			}
		}
		new_trip_file->Write_Trip (*trip_ptr);

		delete trip_ptr;
		trip_ptr = 0;

		trip_ptr_array [sort_itr->second] = trip_ptr;
	}
	trip_ptr_array.clear ();
}
