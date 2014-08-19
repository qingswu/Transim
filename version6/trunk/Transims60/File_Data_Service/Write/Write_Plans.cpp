//*********************************************************
//	Write_Plans.cpp - write a new plan file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Plans
//---------------------------------------------------------

void Data_Service::Write_Plans (void)
{
	int part, num_part, count;
	bool first;

	Plan_File *file = (Plan_File *) System_File_Handle (NEW_PLAN);

	Int_Itr int_itr;
	Trip_Map_Itr trip_itr;
	Time_Map_Itr time_itr;
	Plan_Itr plan_itr;
	Plan_Leg_Itr itr;
	Plan_Data *plan_ptr;
	Partition_Files <Plan_File> new_file_set;

	count = 0;

	if (file->Part_Flag ()) {
		num_part = plan_array.Max_Partition () + 1;
		new_file_set.Initialize (file, num_part);
		Show_Message (String ("Writing %ss -- Record") % file->File_Type ());
	} else {
		num_part = 1;
		Show_Message (String ("Writing %s -- Record") % file->File_Type ());
	}
	Set_Progress ();

	//---- process plan records in sort order  ----

	for (first = true; ; first = false) {
		if (Trip_Sort () == TRAVELER_SORT) {
			if (first) {
				trip_itr = plan_trip_map.begin ();
			} else {
				trip_itr++;
			}
			if (trip_itr == plan_trip_map.end ()) break;
			plan_ptr = &plan_array [trip_itr->second];
		} else if (Trip_Sort () == TIME_SORT) {
			if (first) {
				time_itr = plan_time_map.begin ();
			} else {
				time_itr++;
			}
			if (time_itr == plan_time_map.end ()) break;
			plan_ptr = &plan_array [time_itr->second];
		} else {
			if (first) {
				plan_itr = plan_array.begin ();
			} else {
				plan_itr++;
			}
			if (plan_itr == plan_array.end ()) break;
			plan_ptr = &(*plan_itr);
		}
		Show_Progress ();

		if (plan_ptr->Partition () < 0) continue;

		if (file->Part_Flag ()) {
			part = Partition_Index (plan_ptr->Partition ());
			if (part < 0) continue;
			file = new_file_set [part];
		}
		if (plan_ptr->External_IDs ()) {
			count += Put_Plan_Data (*file, *plan_ptr);
		}
	}
	End_Progress ();

	file->Close ();
	
	Print (2, String ("%s Records = %d") % file->File_Type () % count);
	if (num_part > 1) Print (0, String (" (%d files)") % num_part);
}

//---------------------------------------------------------
//	Put_Plan_Data
//---------------------------------------------------------

int Data_Service::Put_Plan_Data (Plan_File &file, Plan_Data &data)
{
	int count;

	Plan_Leg_Itr itr;

	file.Depart (data.Depart ());
	file.Arrive (data.Arrive ());
	file.Activity (data.Activity ());
	file.Walk (data.Walk ());
	file.Drive (data.Drive ());
	file.Transit (data.Transit ());
	file.Wait (data.Wait ());
	file.Other (data.Other ());
	file.Length (UnRound (data.Length ()));
	file.Cost (UnRound (data.Cost ()));
	file.Impedance (data.Impedance ());

	file.Num_Nest ((int) data.size ());

	count = Put_Trip_Data (file, data);

	//---- write the nested records ----

	for (Plan_Leg_Itr itr = data.begin (); itr != data.end (); itr++) {
		file.Leg_Mode (itr->Mode ());
		file.Leg_Type (itr->Type ());
		file.Leg_ID (itr->ID ());
		file.Leg_Time (itr->Time ());
		file.Leg_Length (UnRound (itr->Length ()));
		file.Leg_Cost (UnRound (itr->Cost ()));
		file.Leg_Impedance (itr->Impedance ());

		if (!file.Write (true)) {
			Error (String ("Writing %s") % file.File_Type ());
		}
		file.Add_Leg ();
		count++;
	}
	return (count);
}
