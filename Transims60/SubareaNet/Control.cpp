//*********************************************************
//	Control.cpp - Program Control
//*********************************************************

#include "SubareaNet.hpp"

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void SubareaNet::Program_Control (void)
{
	String key;
	Strings values;

	Db_Header *file;

	//---- open network files ----

	Data_Service::Program_Control ();

	if (System_File_Flag (TRANSIT_STOP) || System_File_Flag (TRANSIT_ROUTE) ||
		System_File_Flag (TRANSIT_SCHEDULE) || System_File_Flag (TRANSIT_DRIVER) ||
		System_File_Flag (NEW_TRANSIT_STOP) || System_File_Flag (NEW_TRANSIT_ROUTE) ||
		System_File_Flag (NEW_TRANSIT_SCHEDULE) || System_File_Flag (NEW_TRANSIT_DRIVER)) {

		transit_flag = (System_File_Flag (TRANSIT_STOP) && System_File_Flag (TRANSIT_ROUTE) &&
			System_File_Flag (TRANSIT_SCHEDULE) && System_File_Flag (TRANSIT_DRIVER) &&
			System_File_Flag (NEW_TRANSIT_STOP) && System_File_Flag (NEW_TRANSIT_ROUTE) &&
			System_File_Flag (NEW_TRANSIT_SCHEDULE) && System_File_Flag (NEW_TRANSIT_DRIVER));

		if (!transit_flag) {
			Error ("Existing and New Transit Stop, Route, Schedule, and Driver Files are Required");
		}
	}
	zone_flag = System_File_Flag (NEW_ZONE);

	//---- get the projection data ----

	proj_service.Read_Control ();

	Print (2, String ("%s Control Keys:") % Program ());

	//---- get the subarea boundary ----

	key = Get_Control_String (SUBAREA_BOUNDARY_POLYGON);

	if (!key.empty ()) {
		subarea_boundary.File_Type ("Subarea Boundary Polygon");

		subarea_boundary.Set_Projection (proj_service.Input_Projection (), proj_service.Output_Projection ());

		subarea_boundary.Open (Project_Filename (key));
	
		if (!subarea_boundary.Read_Record ()) {
			Error ("Reading the Boundary Polygon");
		}
	} else {
		key = Get_Control_Text (SUBAREA_COORDINATE_BOX);

		if (!key.empty ()) {
			key.Parse (values);
			if ((int) values.size () != 4) {
				Error ("Subarea Coordinate Box does not include Four Values");
			}
			x_min = Round (values [0].Double ());
			y_min = Round (values [1].Double ());
			x_max = Round (values [2].Double ());
			y_max = Round (values [3].Double ());
			box_flag = true;

		} else {

			key = Get_Control_Text (SUBAREA_NODE_CODE);

			if (key.empty ()) {
				Error ("A Subarea Polygon, Coordinate Box, or Node Code is Requried");
			}
			node_code = key.Integer ();

			if (node_code < 1 || node_code > 100) {
				Error ("Subarea Node Code is Out of Range (1..100)");
			}
			node_flag = true;
		}
	}

	//---- get the external offset length ----

	external_offset = Get_Control_Integer (EXTERNAL_OFFSET_LENGTH);

	//---- replicate the field names ----

	Copy_File_Header (NODE, NEW_NODE);
	Copy_File_Header (ZONE, NEW_ZONE);
	Copy_File_Header (LINK, NEW_LINK);
	Copy_File_Header (POCKET, NEW_POCKET);
	Copy_File_Header (CONNECTION, NEW_CONNECTION);
	Copy_File_Header (PARKING, NEW_PARKING);
	Copy_File_Header (LOCATION, NEW_LOCATION);
	Copy_File_Header (ACCESS_LINK, NEW_ACCESS_LINK);
	Copy_File_Header (LANE_USE, NEW_LANE_USE);
	Copy_File_Header (TURN_PENALTY, NEW_TURN_PENALTY);
	Copy_File_Header (SIGN, NEW_SIGN);
	Copy_File_Header (SIGNAL, NEW_SIGNAL);
	Copy_File_Header (TIMING_PLAN, NEW_TIMING_PLAN);
	Copy_File_Header (PHASING_PLAN, NEW_PHASING_PLAN);
	Copy_File_Header (DETECTOR, NEW_DETECTOR);

	if (transit_flag) {
		Copy_File_Header (TRANSIT_STOP, NEW_TRANSIT_STOP);
		Copy_File_Header (TRANSIT_ROUTE, NEW_TRANSIT_ROUTE);
		Copy_File_Header (TRANSIT_SCHEDULE, NEW_TRANSIT_SCHEDULE);
		Copy_File_Header (TRANSIT_DRIVER, NEW_TRANSIT_DRIVER);
	}

	//---- setup the node database ----

	file = System_File_Header (NODE);

	node_db.Replicate_Fields (file, true);
	node_db.File_ID ("Node");

	//---- replicate the timing plan fields ----

	if (System_File_Flag (TIMING_PLAN)) {
		file = System_File_Header (TIMING_PLAN);

		timing_db.Replicate_Fields (file, true);
		timing_db.File_ID ("Timing");
	}
} 
