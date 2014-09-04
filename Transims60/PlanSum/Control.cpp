//*********************************************************
//	Control.cpp - process the control parameters
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Program_Control
//---------------------------------------------------------

void PlanSum::Program_Control (void)
{
	String key;
	bool link_flag, report_flag;

	link_flag = report_flag = false;


	//---- initialize the MPI thread range ----

	MPI_Setup ();

	//---- set equivalence flags ---
	
	if (Report_Flag (LINK_GROUP) || Report_Flag (RIDER_GROUP)) {
		Link_Equiv_Flag (true);
		link_flag = true;
	}
	if (skim_flag || trip_flag) {
		Zone_Equiv_Flag (true);
		zone_flag = true;
	}
	access_file_flag = Check_Control_Key (NEW_ACCESS_DETAIL_FILE);
	access_group_flag = Check_Control_Key (NEW_ACCESS_GROUP_FILE);
	diurnal_flag = Check_Control_Key (NEW_STOP_DIURNAL_FILE);
	boarding_flag = Check_Control_Key (NEW_STOP_BOARDING_FILE);

	if (!Check_Control_Key (NEW_LINE_ON_OFF_FILE)) {
		Stop_Equiv_Flag (Report_Flag (STOP_GROUP) || Report_Flag (STOP_GRP_DETAIL) || 
			Report_Flag (LINE_TRANSFERS) || Report_Flag (ACCESS_DETAILS) || 
			access_file_flag || access_group_flag || diurnal_flag || boarding_flag);

		Line_Equiv_Flag (Report_Flag (LINE_GROUP) || Report_Flag (LINE_GRP_DETAIL) || access_group_flag);
	}

	//---- create the network files ----

	Data_Service::Program_Control ();
	
	Read_Select_Keys ();
	Read_Flow_Time_Keys ();

	plan_file = (Plan_File *) System_File_Handle (PLAN);
	if (!plan_file->Part_Flag ()) Num_Threads (1);

	zone_file_flag = System_File_Flag (ZONE);
	select_flag = System_File_Flag (SELECTION);
	new_perf_flag = System_File_Flag (NEW_PERFORMANCE);

	if (new_perf_flag) {
		if (!System_File_Flag (LINK) || !System_File_Flag (NODE)) {
			Error ("A Link and Node File are Required for Performance Output");
		}
		if (!System_File_Flag (VEHICLE_TYPE)) {
			Warning ("PCE and Occupancy Performance Data Require a Vehicle Type File");
			Show_Message (1);
		}
	}

	if (System_File_Flag (PERFORMANCE)) {
		link_flag = true;
	}
	turn_flag = System_File_Flag (CONNECTION);

	Print (2, String ("%s Control Keys:") % Program ());

	//---- new trip time file ----

	key = Get_Control_String (NEW_TRIP_TIME_FILE);

	if (!key.empty ()) {
		time_file.File_Type ("New Trip Time File");
		time_file.File_ID ("Time");

		if (Master ()) {
			time_file.Create (Project_Filename (key));
		}
		time_flag = true;
	}

	//---- new link volume file ----

	key = Get_Control_String (NEW_LINK_VOLUME_FILE);

	if (!key.empty ()) {
		volume_file.File_Type ("New Link Volume File");
		volume_file.File_ID ("Volume");

		if (Master ()) {
			volume_file.Create (Project_Filename (key));

			volume_file.Num_Decimals (1);
			volume_file.Data_Units (Performance_Units_Map (FLOW_DATA));
			volume_file.Copy_Periods (sum_periods);

			volume_file.Create_Fields ();
			volume_file.Write_Header ();
		}
		volume_flag = new_perf_flag = link_flag = true;
	}

	//---- new access detail file ----

	key = Get_Control_String (NEW_ACCESS_DETAIL_FILE);

	if (!key.empty ()) {
		access_detail_file.File_Type ("New Access Detail File");
		access_detail_file.File_ID ("Access");

		if (Master ()) {
			access_detail_file.Create (Project_Filename (key));
		}
		access_file_flag = true;
	}

	//---- new access group file ----

	key = Get_Control_String (NEW_ACCESS_GROUP_FILE);

	if (!key.empty ()) {
		access_group_file.File_Type ("New Access Group File");
		access_group_file.File_ID ("Access");

		if (Master ()) {
			access_group_file.Create (Project_Filename (key));
		}
		access_group_flag = true;
	}

	//---- new stop diurnal file ----

	key = Get_Control_String (NEW_STOP_DIURNAL_FILE);

	if (!key.empty ()) {
		diurnal_file.File_Type ("New Stop Diurnal File");
		diurnal_file.File_ID ("Diurnal");

		if (Master ()) {
			diurnal_file.Create (Project_Filename (key));
		}
		diurnal_flag = true;
	}

	//---- new line on off file ----

	key = Get_Control_String (NEW_LINE_ON_OFF_FILE);

	if (!key.empty ()) {
		on_off_file.File_Type ("New Line On Off File");
		on_off_file.File_ID ("On_Off");

		if (Master ()) {
			on_off_file.Create (Project_Filename (key));
		}
		on_off_flag = true;
	}

	//---- new stop boarding file ----

	key = Get_Control_String (NEW_STOP_BOARDING_FILE);

	if (!key.empty ()) {
		boarding_file.File_Type ("New Stop Boarding File");
		boarding_file.File_ID ("BOARDING");

		if (Master ()) {
			boarding_file.Create (Project_Filename (key));
		}
		boarding_flag = true;
	}

	//---- read report types ----

	List_Reports ();

	if (Report_Flag (TOP_100) || Report_Flag (VC_RATIO) || Report_Flag (LINK_GROUP)) {
		new_perf_flag = link_flag = report_flag = true;
		cap_factor = (double) sum_periods.Range_Length () / (Dtime (1, HOURS) * sum_periods.Num_Periods ());
	}
	if (link_flag && (!System_File_Flag (LINK) || !System_File_Flag (NODE))) {
		Error ("Link and Node Files are Required for Link-Based Output");
	}
	if (System_File_Flag (LANE_USE) && !System_File_Flag (LINK)) {
		Error ("A Link File is Required for Lane-Use Processing");
	}
	travel_flag = Report_Flag (SUM_TRAVEL);

	//---- ridership summary ----

	passenger_flag = Report_Flag (SUM_PASSENGERS);
	transfer_flag = (Report_Flag (SUM_STOPS) || Report_Flag (STOP_GROUP) || Report_Flag (STOP_GRP_DETAIL));
	rider_flag = (System_File_Flag (NEW_RIDERSHIP) || Report_Flag (SUM_RIDERS) || Report_Flag (RIDER_GROUP) || 
					Report_Flag (LINE_GROUP) || Report_Flag (LINE_GRP_DETAIL));
	xfer_flag = Report_Flag (SUM_TRANSFERS);
	xfer_detail = Report_Flag (XFER_DETAILS);
	line_xfer_flag = Report_Flag (LINE_TRANSFERS);
	access_flag = Report_Flag (ACCESS_DETAILS) || access_file_flag || diurnal_flag;

	if (passenger_flag || transfer_flag || rider_flag || xfer_flag || xfer_detail || on_off_flag || 
		boarding_flag || access_flag || access_group_flag) {

		if (!System_File_Flag (TRANSIT_STOP) || !System_File_Flag (TRANSIT_ROUTE) ||
			!System_File_Flag (TRANSIT_SCHEDULE)) {

			Error ("Transit Network Files are Required for Ridership Output");
		}
		if ((passenger_flag || rider_flag) && !System_File_Flag (TRANSIT_DRIVER)) {
			Error ("A Transit Driver File is needed for Passengers Summaries");
		}
	} else {
		System_File_False (TRANSIT_STOP);
		System_File_False (TRANSIT_ROUTE);
		System_File_False (TRANSIT_SCHEDULE);
		System_File_False (TRANSIT_DRIVER);
	}

	//---- process support data ----

	if (Link_Equiv_Flag ()) {
		link_equiv.Read (Report_Flag (LINK_EQUIV));
	}
	if (Zone_Equiv_Flag ()) {
		zone_equiv.Read (Report_Flag (ZONE_EQUIV));
	}
	if (Stop_Equiv_Flag ()) {
		stop_equiv.Read (Report_Flag (STOP_EQUIV));
	}
	if (Line_Equiv_Flag ()) {
		line_equiv.Read (Report_Flag (LINE_EQUIV));
	}

	//---- allocate work space ----

	if (time_flag || Report_Flag (TRIP_TIME)) {
		time_flag = true;

		int periods = sum_periods.Num_Periods ();
		
		start_time.assign (periods, 0);
		mid_time.assign (periods, 0);
		end_time.assign (periods, 0);
	}

	//---- transfer arrays ----

	if (xfer_flag || xfer_detail) {
		Integers rail;

		rail.assign (10, 0);
		total_on_array.assign (10, rail);

		if (xfer_detail) {
			int num = sum_periods.Num_Periods ();
			if (num < 1) num = 1;

			walk_on_array.assign (num, total_on_array);
			drive_on_array.assign (num, total_on_array);
		}
	}

	if (line_xfer_flag || access_flag || access_group_flag) {
		int i, num;
		Int_Set *list;
		Int_Set_Itr list_itr;

		num = stop_equiv.Max_Group ();

		for (i=1; i <= num; i++) {
			list = stop_equiv.Group_List (i);
			if (list == NULL) continue;

			for (list_itr = list->begin (); list_itr != list->end (); list_itr++) {
				xfer_stop.insert (Int_Map_Data (*list_itr, i));
			}
		}
	}

	//---- initialize the trip summary data ----

	if (travel_flag) {
		trip_sum_data.Copy_Periods (sum_periods);
	}

	//---- initialize the passenger summary data ----

	if (passenger_flag) {
		pass_sum_data.Copy_Periods (sum_periods);
	}
}
