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
	static Trip_File *out_file;

	if (part_num != partition) {
		part_num = partition;
		out_file = (Trip_File *) System_File_Handle (NEW_TRIP);

		if (out_file->Part_Flag () && part_num > 1) {
			if (!out_file->Open (part_num)) {
				Error (String ("Opening %s") % out_file->Filename ());
			}
			if (part_num > max_trip_part) max_trip_part = part_num;
		} else {
			max_trip_part = 1;
		}
	}
	if (Data_Service::Get_Trip_Data (file, trip_rec, partition)) {
		if (select_households && !hhold_range.In_Range (trip_rec.Household ())) return (false);
		if (trip_rec.Mode () >= 0 && trip_rec.Mode () < MAX_MODE && !select_mode [trip_rec.Mode ()]) return (false);
		if (select_purposes && !purpose_range.In_Range (trip_rec.Purpose ())) return (false);
		if (select_vehicles && !vehicle_range.In_Range (trip_rec.Veh_Type ())) return (false);
		if (select_travelers && !traveler_range.In_Range (trip_rec.Type ())) return (false);
		if (select_start_times && !start_range.In_Range (trip_rec.Start ())) return (false);
		if (select_end_times && !end_range.In_Range (trip_rec.End ())) return (false);
		if (select_origins && !org_range.In_Range (trip_rec.Origin ())) return (false);
		if (select_destinations && !des_range.In_Range (trip_rec.Destination ())) return (false);

		if (vehicle_flag && trip_rec.Vehicle () > 0) {
			Vehicle_Index veh_index (trip_rec.Household (), trip_rec.Vehicle ());
			Vehicle_Map_Itr map_itr = vehicle_type.find (veh_index);
			if (map_itr != vehicle_type.end ()) {
				trip_rec.Veh_Type (map_itr->second);
			}
		}
		num_new_trip += Put_Trip_Data (*out_file, trip_rec);
	}
	return (false);
}
