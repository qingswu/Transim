//*********************************************************
//	Read_Plans.cpp - Read the Plan File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

void Data_Service::Read_Plans (Plan_File &file)
{
	int i, part, num, count, num_rec, part_num, first, index;
	bool keep_flag;

	Trip_Index trip_index;
	Trip_Map_Stat trip_stat;
	Time_Index time_index;
	Time_Map_Stat time_stat;
	Plan_Data plan_rec;

	count = num_rec = first = index = 0;

	Initialize_Plans (file);

	//---- check the partition number ----

	if (file.Part_Flag () && First_Partition () != file.Part_Number ()) {
		file.Open (0);
	} else if (First_Partition () >= 0) {
		first = First_Partition ();
	}

	//---- process each partition ----

	for (part=0; ; part++) {
		if (part > 0) {
			if (!file.Open (part)) break;
		}

		//---- store the plan data ----

		if (file.Part_Flag ()) {
			part_num = file.Part_Number ();
			Show_Message (String ("Reading %s %d -- Trip") % file.File_Type () % part_num);
		} else {
			part_num = part + first;
			Show_Message (String ("Reading %s -- Trip") % file.File_Type ());
		}
		Set_Progress ();

		while (file.Read (false)) {
			Show_Progress ();

			plan_rec.Clear ();

			keep_flag = Get_Plan_Data (file, plan_rec, part_num);

			num = file.Num_Nest ();
			if (num > 0 && keep_flag) plan_rec.reserve (num);

			for (i=1; i <= num; i++) {
				if (!file.Read (true)) {
					if (plan_rec.Household () == 0) {
						file.Get_Index (trip_index);
					} else {
						plan_rec.Get_Index (trip_index);
					}
					Error (String ("Number of Nested Records for Plan %d-%d-%d-%d") % trip_index.Household () %
						trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
				}
				Get_Plan_Data (file, plan_rec, part_num);
			}
			if (keep_flag) {
				if (!plan_rec.Internal_IDs ()) continue;

				plan_rec.Index ((int) plan_array.size ());

				if (Trip_Sort () == TRAVELER_SORT || Trip_Sort_Flag ()) {
					plan_rec.Get_Index (trip_index);

					trip_stat = plan_trip_map.insert (Trip_Map_Data (trip_index, plan_rec.Index ()));

					if (!trip_stat.second) {
						Warning (String ("Duplicate Plan Index = %d-%d-%d-%d") % 
							trip_index.Household () % trip_index.Person () % trip_index.Tour () % trip_index.Trip ());
						keep_flag = false;
					}
				}
				if (Trip_Sort () == TIME_SORT || Time_Sort_Flag ()) {
					plan_rec.Get_Index (time_index);

					time_stat = plan_time_map.insert (Time_Map_Data (time_index, plan_rec.Index ()));

					if (!time_stat.second) {
						Warning (String ("Duplicate Plan Index = %s-%d-%d") % 
							time_index.Start ().Time_String () % 
							time_index.Household () % time_index.Person ());
						keep_flag = false;
					}
				}
				if (keep_flag) {
					num = (int) plan_rec.size ();
					file.Add_Trip (plan_rec.Household (), plan_rec.Person (), plan_rec.Tour ());
					file.Add_Leg (num);

					plan_array.push_back (plan_rec);
					plan_array.Max_Partition (plan_rec);
					count += num;
				}
			}
		}
		End_Progress ();
		num_rec += Progress_Count ();
	}
	file.Close ();

	Print (2, String ("Number of %s Records = %d") % file.File_Type () % num_rec);
	if (part > 1) Print (0, String (" (%d files)") % part);

	num = (int) plan_array.size ();

	if (num && num != num_rec) {
		Print (1, "Number of Trips = ") << num;
		Print (1, "Number of Legs = ") << count;
	}
	if (num > 0) System_Data_True (PLAN);
}

//---------------------------------------------------------
//	Initialize_Plans
//---------------------------------------------------------

void Data_Service::Initialize_Plans (Plan_File &file)
{
	Required_File_Check (file, LOCATION);
	Required_File_Check (file, VEHICLE_TYPE);

	int percent = System_Data_Reserve (PLAN);

	if (plan_array.capacity () == 0 && percent > 0) {
		int num = file.Estimate_Records ();

		if (percent != 100) {
			num = (int) ((double) num * percent / 100.0);
		} else if (file.Version () > 40) {
			num /= 4;
		}
		if (num > 1) {
			plan_array.reserve (num);
			if (num > (int) plan_array.capacity ()) Mem_Error (file.File_ID ());
		}
	}
}

//---------------------------------------------------------
//	Get_Plan_Data
//---------------------------------------------------------

bool Data_Service::Get_Plan_Data (Plan_File &file, Plan_Data &plan_rec, int partition)
{

	//---- process a header line ----

	if (!file.Nested ()) {
		file.Get_Data (plan_rec);

		//---- check the household id ----

		if (plan_rec.Household () < 1) return (false);

		plan_rec.Depart (file.Depart ());
		plan_rec.Arrive (file.Arrive ());
		plan_rec.Activity (file.Activity ());
		plan_rec.Walk (file.Walk ());
		plan_rec.Drive (file.Drive ());
		plan_rec.Transit (file.Transit ());
		plan_rec.Other (file.Other ());
		plan_rec.Length (file.Length ());
		plan_rec.Cost (file.Cost ());
		plan_rec.Impedance (file.Impedance ());

		if (plan_rec.Partition () < partition) plan_rec.Partition (partition);
		return (true);
	}

	//---- process a nested leg record ----

	Plan_Leg leg_rec;

	leg_rec.Mode (file.Leg_Mode ());
	leg_rec.Type (file.Leg_Type ());
	leg_rec.ID (file.Leg_ID ());
	leg_rec.Time (file.Leg_Time ());
	leg_rec.Length (file.Leg_Length ());
	leg_rec.Cost (file.Leg_Cost ());
	leg_rec.Impedance (file.Leg_Impedance ());

	plan_rec.push_back (leg_rec);
	return (true);
}
