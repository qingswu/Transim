//*********************************************************
//	Input_Trips.cpp - Read the Trips into Memory
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Input_Trips
//---------------------------------------------------------

void Router::Input_Trips (void)
{
	int part, num_rec, part_num, first, num_selected, priority;
	Trip_File *file = (Trip_File *) System_File_Handle (TRIP);

	Trip_Data trip_rec;
	Trip_Index trip_index;
	Trip_Map_Stat map_stat;
	Plan_Data *plan_ptr, plan_rec;
	Trip_Map_Itr map_itr;

	num_rec = first = num_selected = 0;
	total_records = select_records = select_weight = 0;

	//---- check the partition number ----

	if (file->Part_Flag () && First_Partition () != file->Part_Number ()) {
		file->Open (0);
	} else if (First_Partition () >= 0) {
		first = First_Partition ();
	}

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file->Open (part)) break;
		}

		//---- store the trip data ----

		if (file->Part_Flag ()) {
			part_num = file->Part_Number ();
			Show_Message (String ("Reading %s %d -- Record") % file->File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Record") % file->File_Type ());
		}
		Set_Progress ();

		while (file->Read ()) {
			Show_Progress ();

			file->Get_Data (trip_rec);

			if (trip_rec.Household () < 1) continue;

			if (trip_rec.Partition () < part_num) trip_rec.Partition (part_num);

			priority = initial_priority;

			if (priority_flag) {
				trip_rec.Priority (priority);
			} else if (trip_rec.Priority () == NO_PRIORITY) {
				trip_rec.Priority (MEDIUM);
			}
			if (!Selection (&trip_rec)) {
				trip_rec.Priority (NO_PRIORITY);
				priority = NO_PRIORITY;
			} else {
				num_selected++;
			}
			if (!trip_rec.Internal_IDs ()) continue;

			trip_rec.Get_Index (trip_index);
			file->Add_Trip (trip_index.Household (), trip_index.Person (), trip_index.Tour ());

		    map_itr = plan_trip_map.find (trip_index);

		    if (map_itr == plan_trip_map.end ()) {
			    plan_rec = trip_rec;
			    plan_rec.Index ((int) plan_array.size ());
				plan_rec.Priority (CRITICAL);

			    map_stat = plan_trip_map.insert (Trip_Map_Data (trip_index, plan_rec.Index ()));

			    plan_array.push_back (plan_rec);
			    plan_array.Max_Partition (plan_rec);
				plan_ptr = &plan_rec;
		    } else {
				plan_ptr = &plan_array [map_itr->second];
				if (plan_ptr->Priority () == 0) {
					plan_ptr->Priority (MEDIUM);
				}
				if (priority_flag || priority == NO_PRIORITY) {
					plan_ptr->Priority (priority);
				}
				if (plan_ptr->Start () != trip_rec.Start ()) {
					shift = plan_ptr->Start () - trip_rec.Start ();

					plan_ptr->Start (trip_rec.Start ());
					plan_ptr->End (trip_rec.End ());

					duration = plan_ptr->Arrive () - plan_ptr->Depart ();

					new_time = plan_ptr->Depart () - shift;
					if (new_time < 0) new_time = 0;

					plan_ptr->Depart (new_time); 
					plan_ptr->Arrive (new_time + duration);
				}
			}
			total_records++;
			if (select_priorities && select_priority [plan_ptr->Priority ()]) {
				select_records++;
				select_weight += plan_ptr->Priority ();
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file->Close ();

	Print (2, String ("Number of %s Records = %d") % file->File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	if (num_selected < num_rec) {
		Print (1, "Number of Selected Trip = ") << num_selected;
		if (num_rec > 0) {
			Print (0, String (" (%.1lf%%)") % (100.0 * num_selected / num_rec) % FINISH);
		}
	}
}
