//*********************************************************
//	Read_Trips.cpp - Read the Trips into Memory
//*********************************************************

#include "Simulator.hpp"

//---------------------------------------------------------
//	Read_Trips
//---------------------------------------------------------

void Simulator::Read_Trips (void)
{
	int part, num_rec, part_num, first, num_selected, mode;

	Trip_File *file = System_Trip_File ();

	Trip_Data trip_rec;
	Trip_Index trip_index;
	Trip_Map_Stat map_stat;
	Plan_Data *plan_ptr, plan_rec;
	Trip_Map_Itr map_itr;

	num_rec = first = num_selected = 0;

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

			mode = trip_rec.Mode ();

			if (mode == OTHER_MODE || mode == RIDE_MODE) continue;
			if (!param.transit_flag && (mode == TRANSIT_MODE || mode == PNR_IN_MODE || mode == PNR_OUT_MODE || 
				mode == KNR_IN_MODE || mode == KNR_OUT_MODE)) continue;

			//if (priority_flag) {
			//	trip_rec.Priority (initial_priority);
			//}
			//if (!Selection (&trip_rec)) {
			//	trip_rec.Priority (NO_PRIORITY);
			//} else {
				num_selected++;
			//}

			trip_rec.Get_Index (trip_index);
			file->Add_Trip (trip_index.Household (), trip_index.Person (), trip_index.Tour ());

		    map_itr = plan_trip_map.find (trip_index);

		    if (map_itr == plan_trip_map.end ()) {
				trip_rec.Internal_IDs ();

			    plan_rec = trip_rec;
			    plan_rec.Index ((int) plan_array.size ());

			    map_stat = plan_trip_map.insert (Trip_Map_Data (trip_index, plan_rec.Index ()));

			    plan_array.push_back (plan_rec);
			    plan_array.Max_Partition (plan_rec);
		    } else {
				plan_ptr = &plan_array [map_itr->second];
				plan_ptr->Priority (trip_rec.Priority ());
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
