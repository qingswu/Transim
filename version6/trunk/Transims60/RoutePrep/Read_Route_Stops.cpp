//*********************************************************
//	Read_Route_Stops.cpp - read route stop data
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Read_Route_Stops
//---------------------------------------------------------

void RoutePrep::Read_Route_Stops (void)
{
	int order;
	String name;

	Stop_Offset_Map stop_offset_map, *stop_offset_ptr;
	Stop_Offset_Data stop_offset;
	Route_Stop_Map_Stat route_stat;

	//---- read route stop file----

	Show_Message (String ("Reading %s -- Record") % route_stop_file.File_Type ());
	Set_Progress ();

	while (route_stop_file.Read ()) {
		Show_Progress ();

		name = route_stop_file.Get_String (rstop_route_field);
		name.Trim ();

		route_stat = route_stop_map.insert (Route_Stop_Map_Data (name, stop_offset_map));

		stop_offset_ptr = &route_stat.first->second;

		order = route_stop_file.Get_Integer (rstop_order_field);
		stop_offset.stop = route_stop_file.Get_Integer (rstop_stop_field);
		stop_offset.offset = route_stop_file.Get_Double (rstop_offset_field);

		stop_offset_ptr->insert (Stop_Offset_Map_Data (order, stop_offset));
	}
	End_Progress ();

	route_stop_file.Close ();

	Print (2, String ("Number of %s Records = %d") % route_stop_file.File_Type () % Progress_Count ());
}
