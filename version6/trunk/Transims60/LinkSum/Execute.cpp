//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void LinkSum::Execute (void)
{
	Data_Group_Itr data_itr;
	Dir_Group_Itr dir_itr;

	//---- read the network ----

	Data_Service::Execute ();

	//---- read compare performance file ----

	if (compare_flag) {
		Read_Performance (compare_file, compare_perf_array);
	}
	if (turn_compare_flag) {
		Read_Turn_Delays (turn_compare_file, compare_turn_array);
	}

	//---- flag selected links ----

	if (select_flag) {
		Select_Links ();
	}

	//---- new performance file ----

	if (System_File_Flag (NEW_PERFORMANCE)) {
		Write_Performance ();
	}

	//---- write the link activity file ----

	if (activity_flag) {
		Write_Activity ();
	}

	//---- summarize activity locations on links ----

	if (zone_flag || group_flag) {
		Zone_Summary ();
	}

	//---- write the zone travel file ----

	if (zone_flag) {
		Write_Zone ();
	}

	//---- write the zone group travel time ----

	if (group_flag) {
		Write_Group ();
	}

	//---- write the link direction files ----

	for (dir_itr = dir_group.begin (); dir_itr != dir_group.end (); dir_itr++) {
		Write_Link_Dir (dir_itr);
	}

	//---- write the link data files ----

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
		Write_Link_Data (data_itr);
	}

	//---- write the perf data file ----

	if (summary_flag) {
		if (periods_flag || ratios_flag) {
			Custom_Summaries ();
		} else {
			Summary_File ();
		}
	}

	//---- write the group perf file ----

	if (group_sum_flag) {
		Group_Sum_File ();
	}

	//---- write the turn volume file ----

	if (turn_flag && System_File_Flag (NEW_TURN_DELAY)) {
		Write_Turn_Data ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOP_LINK_VOL:		//---- Top 100 Link Volume Report ----
				Top_100_Report (i);
				break;
			case TOP_LANE_VOL:		//---- Top 100 Lane Volume Report ----
				Top_100_Report (i);
				break;
			case TOP_PERIOD:		//---- Top 100 Period Volume Report ----
				Top_100_Report (i);
				break;
			case TOP_SPEED:			//---- Top 100 Speed Reductions ----
				Top_100_Ratios (i);
				break;
			case TOP_TIME_RATIO:	//---- Top 100 Travel Time Ratios ----
				Top_100_Ratios (i);
				break;
			case TOP_VC_RATIO:		//---- Top 100 Volume Capacity Ratios ----
				Top_100_Ratios (i);
				break;
			case TOP_TIME_CHANGE:	//---- Top 100 Travel Time Changes ----
				Top_100_Ratios (i);
				break;
			case TOP_VOL_CHANGE:	//---- Top 100 Volume Changes ----
				Top_100_Ratios (i);
				break;
			case LINK_REPORT:		//---- Link Events Greater Than dddd ---
				Link_Report (Report_Data ());
				break;
			case LINK_GROUP:		//---- Link Group Report -----
				Link_Group (Report_Data ());
				break;
			case TIME_DISTRIB:		//---- Travel Time Distribution ----
				Travel_Time_Report ();
				break;
			case VC_RATIOS:			//---- Volume Capacity Ratios ----
				Volume_Capacity_Report ();
				break;
			case TIME_CHANGE:		//---- Travel Time Changes ----
				Travel_Time_Change ();
				break;
			case VOLUME_CHANGE:		//---- Volume Changes ----
				Volume_Change ();
				break;
			case TRAVEL_TIME:		//---- Link Group Travel Time ----
				Group_Time_Report ();
				break;
			case PERF_REPORT:		//---- Network Performance Summary ----
				Performance_Report ();
				break;
			case PERF_SUMMARY:		//---- Network Performance Details ----
				Perf_Sum_Report ();
				break;
			case GROUP_REPORT:		//---- Group Performance Summary ----
				Group_Report ();
				break;
			case GROUP_SUMMARY:		//---- Group Performance Details ----
				Group_Sum_Report ();
				break;
			case RELATIVE_GAP:		//---- Relative Gap Report ----
				Relative_Gap_Report ();
				break;
			default:
				break;
		}
	}
	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void LinkSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOP_LINK_VOL:		//---- Top 100 Link Volume Report ----
			Top_100_Link_Header ();
			break;
		case TOP_LANE_VOL:		//---- Top 100 Lane Volume Report ----
			Top_100_Lane_Header ();
			break;
		case TOP_PERIOD:		//---- Top 100 Period Volume Report ----
			Top_100_Time_Header ();
			break;
		case TOP_SPEED:			//---- Top 100 Speed Reductions ----
			Top_100_Speed_Header ();
			break;
		case TOP_TIME_RATIO:	//---- Top 100 Travel Time Distribution ----
			Top_100_Travel_Time_Header ();
			break;
		case TOP_VC_RATIO:		//---- Top 100 Volume Capacity Ratios ----
			Top_100_VC_Header ();
			break;
		case TOP_TIME_CHANGE:	//---- Top 100 Travel Time Changes ----
			Top_100_Time_Change_Header ();
			break;
		case TOP_VOL_CHANGE:	//---- Top 100 Volume Changes ----
			Top_100_Volume_Change_Header ();
			break;
		case LINK_REPORT:		//---- Link Events Greater Than dddd ---
			Link_Report_Header ();
			break;
		case LINK_GROUP:		//---- Link Group Report -----
			Link_Group_Header ();
			break;
		case TIME_DISTRIB:		//---- Travel Time Distribution ----
			Travel_Time_Header ();
			break;
		case VC_RATIOS:			//---- Volume Capacity Ratios ----
			Volume_Capacity_Header ();
			break;
		case TIME_CHANGE:		//---- Travel Time Changes ----
			Time_Change_Header ();
			break;
		case VOLUME_CHANGE:		//---- Volume Changes ----
			Volume_Change_Header ();
			break;
		case TRAVEL_TIME:		//---- Link Group Travel Time ----
			Group_Time_Header ();
			break;
		case PERF_REPORT:		//---- Network Performance Summary ----
			Performance_Header ();
			break;
		case PERF_SUMMARY:		//---- Network Performance Details ----
			Perf_Sum_Header ();
			break;
		case GROUP_REPORT:		//---- Group Performance Summary ----
			Group_Header ();
			break;
		case GROUP_SUMMARY:		//---- Group Performance Details ----
			Group_Sum_Header ();
			break;
		case RELATIVE_GAP:		//---- Relative Gap Report
			Relative_Gap_Header ();
			break;
		default:
			break;
	}
}
