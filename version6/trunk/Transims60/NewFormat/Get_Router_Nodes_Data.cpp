//*********************************************************
//	Get_Route_Nodes_Data.cpp - process the route nodes file
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Get_Route_Nodes_Data
//---------------------------------------------------------

bool NewFormat::Get_Route_Nodes_Data (Route_Nodes_File &file, Route_Header &data_rec)
{
	if (Data_Service::Get_Route_Nodes_Data (file, data_rec)) {
		if (!file.Nested ()) {
			if (select_routes && !route_range.In_Range (data_rec.Route ())) return (false);
			if (data_rec.Mode () >= NO_TRANSIT && data_rec.Mode () < ANY_TRANSIT && !select_transit [data_rec.Mode ()]) return (false);
			if (select_vehicles && !vehicle_range.In_Range (data_rec.Veh_Type ())) return (false);
		}
		return (true);
	}
	return (false);
}

