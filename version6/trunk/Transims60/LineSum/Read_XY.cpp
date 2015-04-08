//*********************************************************
//	Read_XY.cpp - read the node coordinates
//*********************************************************

#include "LineSum.hpp"

//---------------------------------------------------------
//	Read_XY
//---------------------------------------------------------

void LineSum::Read_XY (void)
{
	int node, node_field, x_field, y_field;
	XY_Point xy;

	node_field = node_xy_file.Required_Field ("NODE", "ID", "N", "STOP");
	x_field = node_xy_file.Required_Field (X_FIELD_NAMES);
	y_field = node_xy_file.Required_Field (Y_FIELD_NAMES);

	//---- read the node coordinates file ----

	Show_Message (String ("Reading %s -- Record") % node_xy_file.File_Type ());
	Set_Progress ();

	//---- read each record ----

	while (node_xy_file.Read ()) {
		Show_Progress ();

		node = node_xy_file.Get_Integer (node_field);
		xy.x = node_xy_file.Get_Double (x_field);
		xy.y = node_xy_file.Get_Double (y_field);

		xy_map.insert (XY_Pt_Map_Data (node, xy));
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % node_xy_file.File_Type () % xy_map.size ());
	
	node_xy_file.Close ();
}
