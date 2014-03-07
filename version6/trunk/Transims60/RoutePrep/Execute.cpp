//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void RoutePrep::Execute (void)
{
	Int_Map_Itr itr;

	//---- read the conversion scripts ----

	if (convert_flag) {
		Read_Scripts ();
	}

	//---- setup network data ----

	Data_Service::Execute ();

	//---- set the new link number ----

	if (System_File_Flag (LINK) && link_map.size () > 0) {
		itr = --link_map.end();
		if (new_link <= itr->first) {
			new_link = itr->first + 1;
		}
	}

	//---- set the new node number ----

	if (System_File_Flag (NODE) && node_map.size () > 0) {
		itr = --node_map.end();
		if (new_node <= itr->first) {
			new_node = itr->first + 1;
		}
	}

	//---- set the new route number ----

	if (System_File_Flag (ROUTE_NODES) && route_map.size () > 0) {
		itr = --route_map.end();
		if (new_route <= itr->first) {
			new_route = itr->first + 1;
		}
	}

	//---- read the node map file ----

	if (node_map_flag) {
		Read_Node_Map ();
	}

	//---- read the input node file ----

	if (input_node_flag) {
		Read_Node_File ();
	}

	//---- read route stop file ----

	if (route_stop_flag) {
		Read_Route_Stops ();
	}

	//---- read route shape file ----

	if (route_shape_flag) {
		Read_Routes ();

		Link_Order ();
	}

	//---- read the station node file ----

	if (station_flag) {
		Read_Stations ();
	}

	//---- read the input route file ----

	if (in_route_flag) {
		if (route_speed_flag) {
			Read_Speeds ();
		}
		if (seg_fac_flag) {
			Read_Speed_Factors ();
		}
		if (System_File_Flag (LINK) && !convert_node_flag) {
			Route_Segments ();
		}
		Input_Routes ();
	}

	//---- write the network files ----

	if (System_File_Flag (NEW_NODE)) {
		Write_Nodes ();
	}
	if (shape_flag) {
		Write_Shapes ();
	}
	if (System_File_Flag (NEW_LINK)) {
		Write_Links ();
	}
	if (new_route_flag) {
		if (mode_type_map.size () > 0) {
			Route_Nodes_Itr route_itr; 
			Route_Node_Itr node_itr;

			for (route_itr = route_nodes_array.begin (); route_itr != route_nodes_array.end (); route_itr++) {
				route_itr->Veh_Type (mode_type_map.Best (route_itr->Mode ()));
			}
		}
		Write_Route_Nodes ();
	}
	if (new_map_flag) {
		Write_Node_Map ();
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void RoutePrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case SCRIPT_REPORT:		//---- Conversion Script ----
			Print (1, "Conversion Script");
			Print (1);
			break;
		case STACK_REPORT:		//---- Conversion Stack ----
			Print (1, "Conversion Stack");
			Print (1);
			break;
		default:
			break;
	}
}
