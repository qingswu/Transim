//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void ArcRider::Program_Control (void)
{
	String key;
	bool z_flag, m_flag;

	//---- set the equivalance flag ----

	Stop_Equiv_Flag (Check_Control_Key (NEW_ARC_STOP_GROUP_FILE));
	Line_Equiv_Flag (Check_Control_Key (NEW_ARC_LINE_GROUP_FILE));

	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	if (Control_Key_Status (BANDWIDTH_FIELD)) {
		bandwidth_flag = !(Get_Control_String (BANDWIDTH_FIELD)).empty ();
	}

	//---- get the projection data ----

	projection.Read_Control ();
	Print (1);

	//---- get the z coordinate flag ----

	m_flag = projection.M_Flag ();
	z_flag = projection.Z_Flag ();

	//---- get the arcview line demand file ----

	key = Get_Control_String (NEW_ARC_LINE_DEMAND_FILE);

	if (!key.empty ()) {
		arcview_line.File_Type ("New Arc Line Demand File");
		arcview_line.File_Access (CREATE);

		if (bandwidth_flag) {
			arcview_line.Shape_Type (POLYGON);
		} else {
			arcview_line.Shape_Type (VECTOR);
		}
		arcview_line.Z_Flag (z_flag);
		arcview_line.M_Flag (m_flag);

		if (!arcview_line.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Line Demand File", arcview_line.Shape_Filename ());
		}
		line_flag = true;

		arcview_line.Add_Field ("ROUTE", DB_INTEGER, 10);
		arcview_line.Add_Field ("NAME", DB_STRING, 40);
		arcview_line.Add_Field ("MODE", DB_STRING, 16, TRANSIT_CODE);
		arcview_line.Add_Field ("STOPS", DB_INTEGER, 10);
		arcview_line.Add_Field ("RUNS", DB_INTEGER, 10);
		arcview_line.Add_Field ("RIDERS", DB_INTEGER, 10);
		arcview_line.Add_Field ("MAX_LOAD", DB_INTEGER, 10);
		arcview_line.Add_Field ("MAX_BOARD", DB_INTEGER, 10);
		arcview_line.Add_Field ("MAX_ALIGHT", DB_INTEGER, 10);
		arcview_line.Add_Field ("MIN_TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		arcview_line.Add_Field ("MAX_TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());

		if (Notes_Name_Flag ()) {
			arcview_line.Add_Field ("NOTES", DB_STRING, 128);
		}
		arcview_line.Write_Header ();

		arcview_line.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview line group file ----

	key = Get_Control_String (NEW_ARC_LINE_GROUP_FILE);

	if (!key.empty ()) {
		arcview_sum.File_Type ("New Arc Line Group File");
		arcview_sum.File_Access (CREATE);

		if (bandwidth_flag) {
			arcview_sum.Shape_Type (POLYGON);
		} else {
			arcview_sum.Shape_Type (VECTOR);
		}
		arcview_sum.Z_Flag (z_flag);
		arcview_sum.M_Flag (m_flag);

		if (!arcview_sum.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Line Group File", arcview_sum.Shape_Filename ());
		}
		sum_flag = true;

		arcview_sum.Add_Field ("GROUP", DB_INTEGER, 10);
		arcview_sum.Add_Field ("NAME", DB_STRING, 40);
		arcview_sum.Add_Field ("ROUTES", DB_INTEGER, 10);
		arcview_sum.Add_Field ("STOPS", DB_INTEGER, 10);
		arcview_sum.Add_Field ("RUNS", DB_INTEGER, 10);
		arcview_sum.Add_Field ("RIDERS", DB_INTEGER, 10);
		arcview_sum.Add_Field ("MAX_LOAD", DB_INTEGER, 10);
		arcview_sum.Add_Field ("MAX_BOARD", DB_INTEGER, 10);
		arcview_sum.Add_Field ("MAX_ALIGHT", DB_INTEGER, 10);
		arcview_sum.Add_Field ("MIN_TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		arcview_sum.Add_Field ("MAX_TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());

		arcview_sum.Write_Header ();

		arcview_sum.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview ridership file ----

	key = Get_Control_String (NEW_ARC_RIDERSHIP_FILE);
	if (!key.empty ()) {
		arcview_rider.File_Type ("New Arc Ridership File");
		arcview_rider.File_Access (CREATE);

		if (bandwidth_flag) {
			arcview_rider.Shape_Type (POLYGON);
		} else {
			arcview_rider.Shape_Type (VECTOR);
		}
		arcview_rider.Z_Flag (z_flag);
		arcview_rider.M_Flag (m_flag);

		if (!arcview_rider.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Ridership File", arcview_rider.Shape_Filename ());
		}
		rider_flag = true;

		arcview_rider.Add_Field ("LINK", DB_INTEGER, 10);
		arcview_rider.Add_Field ("DIR", DB_INTEGER, 1);
		arcview_rider.Add_Field ("SEGMENT", DB_INTEGER, 2);
		arcview_rider.Add_Field ("BOARD", DB_INTEGER, 10);
		arcview_rider.Add_Field ("RIDERS", DB_INTEGER, 10);
		arcview_rider.Add_Field ("ALIGHT", DB_INTEGER, 10);
		arcview_rider.Add_Field ("RUNS", DB_INTEGER, 10);
		arcview_rider.Add_Field ("LOADFAC", DB_DOUBLE, 10.2);
		arcview_rider.Add_Field ("CAPACITY", DB_INTEGER, 10);
		arcview_rider.Add_Field ("CAPFAC", DB_DOUBLE, 10.2);

		if (Notes_Name_Flag ()) {
			arcview_rider.Add_Field ("NOTES", DB_STRING, 128);
		}
		arcview_rider.Write_Header ();

		arcview_rider.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview stop demand file ----

	key = Get_Control_String (NEW_ARC_STOP_DEMAND_FILE);
	if (!key.empty ()) {
		arcview_stop.File_Type ("New Arc Stop Demand File");
		arcview_stop.File_Access (CREATE);
		arcview_stop.Shape_Type (DOT);
		arcview_stop.Z_Flag (z_flag);
		arcview_stop.M_Flag (m_flag);

		if (!arcview_stop.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Stop Demand File", arcview_stop.Shape_Filename ());
		}
		on_off_flag = demand_flag = true;

		arcview_stop.Add_Field ("STOP", DB_INTEGER, 10);
		arcview_stop.Add_Field ("BOARD", DB_INTEGER, 10);
		arcview_stop.Add_Field ("ALIGHT", DB_INTEGER, 10);
		arcview_stop.Add_Field ("TOTAL", DB_INTEGER, 10);

		if (Notes_Name_Flag ()) {
			arcview_stop.Add_Field ("NOTES", DB_STRING, 128);
		}
		arcview_stop.Write_Header ();

		arcview_stop.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview stop group file ----

	key = Get_Control_String (NEW_ARC_STOP_GROUP_FILE);
	if (!key.empty ()) {
		arcview_group.File_Type ("New Arc Stop Group File");
		arcview_group.File_Access (CREATE);
		arcview_group.Shape_Type (DOT);
		arcview_group.Z_Flag (z_flag);
		arcview_group.M_Flag (m_flag);

		if (!arcview_group.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Stop Group File", arcview_group.Shape_Filename ());
		}
		on_off_flag = group_flag = true;

		arcview_group.Add_Field ("GROUP", DB_INTEGER, 10);
		arcview_group.Add_Field ("NAME", DB_STRING, 40);
		arcview_group.Add_Field ("BOARD", DB_INTEGER, 10);
		arcview_group.Add_Field ("ALIGHT", DB_INTEGER, 10);
		arcview_group.Add_Field ("TOTAL", DB_INTEGER, 10);

		arcview_group.Write_Header ();

		arcview_group.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- get the arcview run capacity file ----

	key = Get_Control_String (NEW_ARC_RUN_CAPACITY_FILE);

	if (!key.empty ()) {
		if (!System_File_Flag (VEHICLE_TYPE)) {
			Error ("A Vehicle Type File is required for Run Capacity");
		}
		arcview_cap.File_Type ("New Arc Run Capacity File");
		arcview_cap.File_Access (CREATE);
		arcview_cap.Shape_Type (VECTOR);

		if (bandwidth_flag) {
			arcview_rider.Shape_Type (POLYGON);
		} else {
			arcview_rider.Shape_Type (VECTOR);
		}
		arcview_cap.Z_Flag (z_flag);
		arcview_cap.M_Flag (m_flag);

		if (!arcview_cap.Open (Project_Filename (key))) {
			File_Error ("Opening New Arc Run Capacity File", arcview_cap.Shape_Filename ());
		}
		cap_flag = true;

		arcview_cap.Add_Field ("MODE", DB_STRING, 16);		
		arcview_cap.Add_Field ("ROUTE", DB_INTEGER, 10);
		arcview_cap.Add_Field ("TIME", DB_TIME, TIME_FIELD_SIZE, Time_Format ());
		arcview_cap.Add_Field ("PERIOD", DB_INTEGER, 4);
		arcview_cap.Add_Field ("AVG_RUNS", DB_INTEGER, 4);
		arcview_cap.Add_Field ("AVG_LOAD", DB_INTEGER, 6);
		arcview_cap.Add_Field ("AVG_FAC", DB_DOUBLE, 6.2);
		arcview_cap.Add_Field ("CAP_RUNS", DB_INTEGER, 4);
		arcview_cap.Add_Field ("CAP_LOAD", DB_INTEGER, 6);
		arcview_cap.Add_Field ("CAP_FAC", DB_DOUBLE, 6.2);
		arcview_cap.Add_Field ("MAX_RUN", DB_INTEGER, 4);
		arcview_cap.Add_Field ("MAX_LOAD", DB_INTEGER, 6);
		arcview_cap.Add_Field ("MAX_FAC", DB_DOUBLE, 6.2);
		
		if (Notes_Name_Flag ()) {
			arcview_cap.Add_Field ("NOTES", DB_STRING, 128);
		}
		arcview_cap.Write_Header ();

		arcview_cap.Set_Projection (projection.Input_Projection (), projection.Output_Projection ());
	}

	//---- check for output ----

	if (!line_flag && !sum_flag && !rider_flag && !on_off_flag && !cap_flag) {
		Error ("An Output ArcView File was Not Specified");
	}

	//---- draw lanes ----

	Print (1);
	Read_Draw_Keys ();

	if (bandwidth_flag) {
		if (line_flag) {
			width_field = arcview_line.Required_Field (bandwidth_field);
		} else if (sum_flag) {
			width_field = arcview_sum.Required_Field (bandwidth_field);
		} else if (rider_flag) {
			width_field = arcview_rider.Required_Field (bandwidth_field);
		} else if (cap_flag) {
			width_field = arcview_cap.Required_Field (bandwidth_field);
		} else {
			Error ("Bandwidths require an Arcview Link or Delay File");
		}
	}

	//---- read the stop equiv ----

	if (Stop_Equiv_Flag ()) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}

	//---- read the line equiv ----

	if (Line_Equiv_Flag ()) {
		line_equiv.Read (Report_Flag (LINE_EQUIV));
	}
}
