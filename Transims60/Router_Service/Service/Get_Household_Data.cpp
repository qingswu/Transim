//*********************************************************
//	Router_Service.cpp - path building service
//*********************************************************

#include "Router_Service.hpp"

//---------------------------------------------------------
//	Get_Household_Data
//---------------------------------------------------------

bool Router_Service::Get_Household_Data (Household_File &file, Household_Data &household_rec, int partition)
{
	int hhold, person, type;
	Int2_Map_Stat map_stat;

	type = MAX (file.Type (), 1);

	if (script_flag) {
		type = type_script.Execute ();
		if (type == 0) return (false);
	}
	hhold = file.Household ();
	person = file.Person ();

	if (!file.Nested ()) {
		map_stat = hhold_type.insert (Int2_Map_Data (Int2_Key (hhold, 1), type));
	} else {
		map_stat = hhold_type.insert (Int2_Map_Data (Int2_Key (hhold, person), type));
	}
	person = partition;
	household_rec.Household (hhold);
	return (false);
}
