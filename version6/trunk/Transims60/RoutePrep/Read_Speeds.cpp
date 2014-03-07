//*********************************************************
//	Read_Speeds.cpp - read route speed file
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Speeds
//---------------------------------------------------------

void RoutePrep::Read_Speeds (void)
{
	int index;
	String route;
	Doubles speed_data;
	Str_ID_Stat map_stat;

	speed_data.assign (2, 0.0);

	Show_Message (String ("Reading %s -- Record") % speed_file.File_Type ());
	Set_Progress ();

	while (speed_file.Read ()) {
		Show_Progress ();

		route = speed_file.Get_String (speed_route_field);

		if (route.empty ()) continue;

		speed_data [0] = speed_file.Get_Double (speed_peak_field);
		speed_data [1] = speed_file.Get_Double (speed_offpeak_field);

		index = (int) speed_array.size ();

		map_stat = speed_map.insert (Str_ID_Data (route, index));

		if (!map_stat.second) {
			Warning ("Duplicate Route Name = ") << route;
		} else {
			speed_array.push_back (speed_data);
		}
	}
	End_Progress ();

	speed_file.Close ();

	Print (2, String ("Number of %s Records = %d") % speed_file.File_Type () % Progress_Count ());
}
