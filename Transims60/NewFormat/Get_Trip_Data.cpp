//*********************************************************
//	Get_Trip_Data.cpp - Read the Trip File
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool NewFormat::Get_Trip_Data (Trip_File &file, Trip_Data &trip_rec, int partition)
{
	static int part_num = -1;
	static Trip_File *out_file = 0;

	if (part_num != partition) {
		part_num = partition;

		if (new_trip_flag) {
			out_file = (Trip_File *) System_File_Handle (NEW_TRIP);

			if (out_file != 0 && out_file->Part_Flag () && part_num > 1) {
				if (!out_file->Open (part_num)) {
					Error (String ("Opening %s") % out_file->Filename ());
				}
				if (part_num > max_trip_part) max_trip_part = part_num;
			} else {
				max_trip_part = 1;
			}
		} else {
			max_trip_part = 1;
		}
	}
	if (file.Version () < 60) {
		trip_rec.Household (file.Household ());
		if (trip_rec.Household () < 1) return (false);

		trip_rec.Person (file.Person ());
		trip_rec.Tour (file.Tour ());
		if (trip_rec.Tour () < 1) trip_rec.Tour (1);
		trip_rec.Trip (file.Trip ());

		trip_rec.Start (file.Start ());
		trip_rec.End (file.End ());
		trip_rec.Duration (file.Duration ());

		trip_rec.Origin (file.Origin ());
		trip_rec.Destination (file.Destination ());

		trip_rec.Purpose (file.Purpose ());
		trip_rec.Constraint (file.Constraint ());
		trip_rec.Priority (file.Priority ());

		int vehicle = file.Vehicle ();
		int veh_type = file.Veh_Type ();

		if (file.Version () <= 40) {
			trip_rec.Mode (Trip_Mode_Map (file.Mode ()));

			if (vehicle > 0) {
				Veh_ID_Map_Itr itr = vehicle40_map.find (vehicle);

				if (itr == vehicle40_map.end () || itr->second.Household () != trip_rec.Household ()) return (false);

				vehicle = itr->second.Vehicle ();
			}
		} else {
			trip_rec.Mode (file.Mode ());
		}
		if (vehicle > 0) {
			Vehicle_Index veh_index (trip_rec.Household (), vehicle);

			Vehicle_Map_Itr itr = vehicle_type.find (veh_index);

			if (itr != vehicle_type.end ()) {
				veh_type = itr->second;
			} else if (file.Version () < 50 || veh_type == 0) {
				return (false);
			}
		} else {
			veh_type = 0;
		}
		trip_rec.Vehicle (vehicle);
		trip_rec.Veh_Type (veh_type);
		trip_rec.Type (file.Type ());
		trip_rec.Partition (file.Partition ());
		if (trip_rec.Partition () < partition) trip_rec.Partition (partition);

	} else {

		if (!Data_Service::Get_Trip_Data (file, trip_rec, partition)) return (false);
	}
	if (select_households && !hhold_range.In_Range (trip_rec.Household ())) return (false);
	if (trip_rec.Mode () >= 0 && trip_rec.Mode () < MAX_MODE && !select_mode [trip_rec.Mode ()]) return (false);
	if (select_purposes && !purpose_range.In_Range (trip_rec.Purpose ())) return (false);
	if (select_vehicles && !vehicle_range.In_Range (trip_rec.Veh_Type ())) return (false);
	if (select_travelers && !traveler_range.In_Range (trip_rec.Type ())) return (false);
	if (select_start_times && !start_range.In_Range (trip_rec.Start ())) return (false);
	if (select_end_times && !end_range.In_Range (trip_rec.End ())) return (false);
	if (select_origins && !org_range.In_Range (trip_rec.Origin ())) return (false);
	if (select_destinations && !des_range.In_Range (trip_rec.Destination ())) return (false);

	if (new_trip_flag) {
		num_new_trip += Put_Trip_Data (*out_file, trip_rec);
	}
	if (new_plan_flag) return (true);
	return (false);
}
