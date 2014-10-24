//*********************************************************
//	Read_Plan.cpp - Read the Plan File
//*********************************************************

#include "PlanTrips.hpp"

//---------------------------------------------------------
//	Read_Plan
//---------------------------------------------------------

void PlanTrips::Read_Plan (void)
{
	int part, num_trips, num_update;

	Trip_File *trip_file;
	Plan_File *plan_file;
	Plan_Data plan;
	Trip_Index trip_index;
	Trip_Map_Itr trip_map_itr;
	Trip_Data *trip_ptr, trip_rec;
	Trip_Map_Stat map_stat;

	plan_file = System_Plan_File ();
	trip_file = System_Trip_File (true);

	num_update = num_trips = 0;

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!plan_file->Open (part)) break;
		}
		if (plan_file->Part_Flag ()) {
			Show_Message (String ("Reading %s %d -- Record") % plan_file->File_Type () % plan_file->Part_Number ());
		} else {
			Show_Message (String ("Reading %s -- Record") % plan_file->File_Type ());
		}
		Set_Progress ();

		while (plan_file->Read_Plan (plan)) {
			Show_Progress ();

			plan.Get_Index (trip_index);
			num_trips++;

			if (trip_flag) {
				trip_map_itr = trip_map.find (trip_index);

				if (trip_map_itr != trip_map.end ()) {
					trip_ptr = &trip_array [trip_map_itr->second];

					trip_ptr->Start (plan.Depart ());
					trip_ptr->End (plan.Arrive ());
					trip_ptr->Duration (plan.Activity ());
					num_update++;
				}
			} else {
				trip_rec.Household (plan.Household ());
				trip_rec.Person (plan.Person ());
				trip_rec.Tour (plan.Tour ());
				trip_rec.Trip (plan.Trip ());

				trip_rec.Start (plan.Depart ());
				trip_rec.End (plan.Arrive ());
				trip_rec.Origin (plan.Origin ());
				trip_rec.Destination (plan.Destination ());
				trip_rec.Vehicle (plan.Vehicle ());

				trip_rec.Mode (plan.Mode ());
				trip_rec.Purpose (plan.Purpose ());
				trip_rec.Constraint (plan.Constraint ());
				trip_rec.Type (plan.Type ());
				trip_rec.Partition (part);

				//---- check the selection criteria ----

				if (select_households && !hhold_range.In_Range (trip_rec.Household ())) continue;
				if (trip_rec.Mode () < MAX_MODE && !select_mode [trip_rec.Mode ()]) continue;
				if (select_purposes && !purpose_range.In_Range (trip_rec.Purpose ())) continue;
				if (select_travelers && !traveler_range.In_Range (trip_rec.Type ())) continue;
				if (select_start_times && !start_range.In_Range (trip_rec.Start ())) continue;
				if (select_end_times && !end_range.In_Range (trip_rec.End ())) continue;
				if (select_origins && !org_range.In_Range (trip_rec.Origin ())) continue;
				if (select_destinations && !des_range.In_Range (trip_rec.Destination ())) continue;
		
				//---- check the selection records ----

				if (select_flag) {
					Select_Map_Itr sel_itr;

					sel_itr = select_map.Best (trip_rec.Household (), trip_rec.Person (), trip_rec.Tour (), trip_rec.Trip ());
					if (sel_itr == select_map.end ()) continue;
				}

				//---- write the record ----

				Put_Trip_Data (*trip_file, trip_rec);

				//map_stat = trip_map.insert (Trip_Map_Data (trip_index, (int) trip_array.size ()));

				//if (!map_stat.second) {
				//	Warning (String ("Duplicate Trip Record = %d-%d-%d-%d") % 
				//		trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
				//} else {
				//	trip_array.push_back (trip_rec);
				//	trip_array.Max_Partition (trip_rec);
				//}
			}
		}
		End_Progress ();
	}
	plan_file->Close ();

	if (!trip_flag) trip_file->Close ();

	Print (2, String ("Number of %s Trips = %d") % plan_file->File_Type () % num_trips);
	if (part > 1) Print (0, String (" (%d files)") % part);

	Print (1, "Number of Trips Updated = ") << num_update;
}
