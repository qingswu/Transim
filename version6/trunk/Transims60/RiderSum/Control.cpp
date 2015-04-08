//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void RiderSum::Program_Control (void)
{
	String key;


	//---- open network files ----

	Data_Service::Program_Control ();

	Read_Select_Keys ();

	//---- open line rider file ----

	key = Get_Control_String (NEW_LINE_RIDER_FILE);

	if (!key.empty ()) {
		Print (1);
		line_rider_flag = true;

		line_rider_file.File_Type ("New Line Rider File");
		line_rider_file.File_ID ("LineRider");

		line_rider_file.Create (Project_Filename (key));
	}
	
	//---- open line group file ----

	key = Get_Control_String (NEW_LINE_GROUP_FILE);

	if (!key.empty ()) {
		Print (1);
		line_group_flag = true;

		line_group_file.File_Type ("New Line Group File");
		line_group_file.File_ID ("LineGroup");

		line_group_file.Create (Project_Filename (key));
	}
	
	//---- open stop route file ----

	key = Get_Control_String (NEW_STOP_ROUTE_FILE);

	if (!key.empty ()) {
		Print (1);
		stop_route_flag = true;

		stop_route_file.File_Type ("New Stop Route File");
		stop_route_file.File_ID ("StopRoute");

		stop_route_file.Create (Project_Filename (key));
	}

	//---- open stop group file ----

	key = Get_Control_String (NEW_STOP_GROUP_FILE);

	if (!key.empty ()) {
		Print (1);
		stop_group_flag = true;

		stop_group_file.File_Type ("New Stop Group File");
		stop_group_file.File_ID ("StopGroup");

		stop_group_file.Create (Project_Filename (key));
	}

	//---- open stop profile ----

	key = Get_Control_String (NEW_STOP_PROFILE);

	if (!key.empty ()) {
		Print (1);
		stop_profile_flag = true;

		stop_profile.File_Type ("New Stop Profile");
		stop_profile.File_ID ("Profile");

		stop_profile.Create (Project_Filename (key));
	}

	//---- open stop vehtype file ----

	key = Get_Control_String (NEW_STOP_VEHTYPE_FILE);

	if (!key.empty ()) {
		Print (1);
		stop_vehtype_flag = true;

		stop_vehtype_file.File_Type ("New Stop VehType File");
		stop_vehtype_file.File_ID ("StopType");

		stop_vehtype_file.Create (Project_Filename (key));
	}

	//---- open stop lines file ----

	key = Get_Control_String (NEW_STOP_LINES_FILE);

	if (!key.empty ()) {
		Print (1);
		stop_lines_flag = true;

		stop_lines.File_Type ("New Stop Lines File");
		stop_lines.File_ID ("Lines");

		stop_lines.Create (Project_Filename (key));
	}

	//---- open stop detail file ----

	key = Get_Control_String (NEW_STOP_DETAIL_FILE);

	if (!key.empty ()) {
		Print (1);
		stop_detail_flag = true;

		stop_detail_file.File_Type ("New Stop Detail File");
		stop_detail_file.File_ID ("Detail");

		stop_detail_file.Create (Project_Filename (key));
	}

	//---- read report types ----

	List_Reports ();

	on_off_flag = (Report_Flag (STOP_SUM) || Report_Flag (STOP_GROUP) || stop_group_flag);

	//---- read the stop equiv ----

	if (Stop_Equiv_Flag ()) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	} else {
		if (stop_group_flag) {
			Error ("A Stop Equivalance File is Required for Stop Group Output");
		}
		if (Report_Flag (STOP_GROUP)) {
			Error ("A Stop Equivalence File is Required for Reporting");
		}
	}

	//---- read the line equiv ----

	if (Line_Equiv_Flag ()) {
		line_equiv.Read (Report_Flag (LINE_EQUIV));
	} else if (line_group_flag || Report_Flag (LINE_GROUP) || stop_profile_flag || stop_vehtype_flag) {
		Error ("A Line Equivalance File is Required for Link Group processing");
	}
}
