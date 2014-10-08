//*********************************************************
//	Get_Access_Data.cpp - read the access link file
//*********************************************************

#include "Relocate.hpp"

//---------------------------------------------------------
//	Get_Access_Data
//---------------------------------------------------------

bool Relocate::Get_Access_Data (Access_File &file, Access_Data &access_rec)
{
	int lvalue, index, type;
	Int_Map_Itr map_itr;

	bool stop_flag = System_Data_Flag (TRANSIT_STOP);

	//---- set the end type ----
	
	access_rec.From_Type (file.From_Type ());
	access_rec.To_Type (file.To_Type ());

	if ((access_rec.From_Type () == STOP_ID || access_rec.To_Type () == STOP_ID) 
		&& !stop_flag) return (false);

	//---- check/convert the from id ----

	lvalue = file.From_ID ();
	if (lvalue == 0) return (false);

	type = access_rec.From_Type ();
	index = -1;

	if (type == LOCATION_ID) {
		map_itr = location_map.find (lvalue);
		if (map_itr != location_map.end ()) index = map_itr->second;
	} else if (type == PARKING_ID) {
		map_itr = parking_map.find (lvalue);
		if (map_itr != parking_map.end ()) index = map_itr->second;
	} else if (type == STOP_ID) {
		map_itr = stop_map.find (lvalue);
		if (map_itr != stop_map.end ()) index = map_itr->second;
	} else if (type == NODE_ID) {
		map_itr = node_map.find (lvalue);
		if (map_itr != node_map.end ()) index = map_itr->second;
	} else {
		return (false);
	}
	if (index < 0) {
		if (file.Version () > 40 || !(type == LOCATION_ID && access_rec.To_Type () == STOP_ID)) {
			Warning (String ("Access Link From_ID %d was Not Found in the %s file") % lvalue % ID_Code ((ID_Type) type));
		}
		return (false);
	}
	access_rec.From_ID (index);

	//---- check/convert the to id ----

	lvalue = file.To_ID ();
	type = access_rec.To_Type ();
	index = -1;

	if (type == LOCATION_ID) {
		map_itr = location_map.find (lvalue);
		if (map_itr != location_map.end ()) index = map_itr->second;
	} else if (type == PARKING_ID) {
		map_itr = parking_map.find (lvalue);
		if (map_itr != parking_map.end ()) index = map_itr->second;
	} else if (type == STOP_ID) {
		map_itr = stop_map.find (lvalue);
		if (map_itr != stop_map.end ()) index = map_itr->second;
	} else if (type == NODE_ID) {
		map_itr = node_map.find (lvalue);
		if (map_itr != node_map.end ()) index = map_itr->second;
	} else {
		return (false);
	}
	if (index < 0) {
		if (file.Version () > 40 || !(type == LOCATION_ID && access_rec.From_Type () == STOP_ID)) {
			Warning (String ("Access Link To_ID %d was Not Found in the %s file") % lvalue % ID_Code ((ID_Type) type));
		}
		return (false);
	}
	access_rec.To_ID (index);

	//----- optional fields ----

	access_rec.Time (file.Time ());
	access_rec.Cost (file.Cost ());

	access_rec.Link (file.Link ());
	access_rec.Dir (file.Dir ());

	if (access_rec.Link () == 0) {
		access_rec.Link ((int) access_array.size () + 1);
	}
	access_rec.Notes (file.Notes ());

	return (true);
}
