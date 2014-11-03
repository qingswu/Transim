//*********************************************************
//	Get_Trip_Data.cpp - filter the trip data
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Trip_Data
//---------------------------------------------------------

bool Relocate::Get_Trip_Data (Trip_File &file, Trip_Data &trip_rec, int partition)
{
	file.Get_Data (trip_rec);
	if (trip_rec.Partition () < partition) trip_rec.Partition (partition);
	file.Add_Trip (trip_rec.Household (), trip_rec.Person (), trip_rec.Tour ());

	//---- check the selection criteria ----

	if (select_households && !hhold_range.In_Range (trip_rec.Household ())) return (false);
	if (trip_rec.Mode () < MAX_MODE && !select_mode [trip_rec.Mode ()]) return (false);
	if (select_purposes && !purpose_range.In_Range (trip_rec.Purpose ())) return (false);
	if (select_travelers && !traveler_range.In_Range (trip_rec.Type ())) return (false);
	if (select_start_times && !start_range.In_Range (trip_rec.Start ())) return (false);
	if (select_end_times && !end_range.In_Range (trip_rec.End ())) return (false);
	if (select_origins && !org_range.In_Range (trip_rec.Origin ())) return (false);
	if (select_destinations && !des_range.In_Range (trip_rec.Destination ())) return (false);
		
	//---- check the selection records ----

	if (select_flag) {
		Select_Map_Itr sel_itr;

		sel_itr = select_map.Best (trip_rec.Household (), trip_rec.Person (), trip_rec.Tour (), trip_rec.Trip ());
		if (sel_itr == select_map.end ()) return (false);
	}
	Int_Map_Itr map_itr;
	Location_Data *location_ptr;

	map_itr = target_loc_map.find (trip_rec.Origin ());
	if (map_itr != target_loc_map.end ()) {
		location_ptr = &location_array [map_itr->second];
		trip_rec.Origin (location_ptr->Location ());
	} else {
		map_itr = location_map.find (trip_rec.Origin ());
		if (map_itr != location_map.end ()) {
			location_ptr = &location_array [map_itr->second];
			if (location_ptr->Zone () != 1) return (false);
		}
	}
	map_itr = target_loc_map.find (trip_rec.Destination ());
	if (map_itr != target_loc_map.end ()) {
		location_ptr = &location_array [map_itr->second];
		trip_rec.Destination (location_ptr->Location ());
	} else {
		map_itr = location_map.find (trip_rec.Destination ());
		if (map_itr != location_map.end ()) {
			location_ptr = &location_array [map_itr->second];
			if (location_ptr->Zone () != 1) return (false);
		}
	}
	Put_Trip_Data (*((Trip_File *) System_File_Handle (NEW_TRIP)), trip_rec);
	return (false);
}
