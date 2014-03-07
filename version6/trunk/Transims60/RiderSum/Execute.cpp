//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void RiderSum::Execute (void)
{

	//---- read the network ----

	Data_Service::Execute ();

	//---- allocate transit memory ----

	if (rider_flag || on_off_flag) {
		Setup_Riders ();
		Sum_Riders ();
	}

	//---- write the line rider file ----

	if (line_rider_flag) {
		Write_Line_Rider ();
	}
	
	//---- write the line group file ----

	if (line_group_flag) {
		Write_Group_Rider ();
	}

	//---- write the stop route file ----

	if (stop_route_flag) {
		Write_Stop_Route ();
	}

	//---- write the stop profile ----

	if (stop_profile_flag) {
		Write_Stop_Profile ();
	}

	//---- write the stop details ----

	if (stop_detail_flag) {
		Write_Stop_Details ();
	}

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case LINE_RIDERS:		//---- Line Rider Profle ----
				Line_Rider_Report ();
				break;
			case GROUP_RIDERS:		//---- Group Rider Profle ----
				Group_Rider_Report ();
				break;
			case LINE_SUM:			//---- Line Summary Report ----
				Line_Sum_Report ();
				break;
			case LINE_GROUP:		//---- Line Group Report ----
				Line_Group_Report ();
				break;
			case STOP_SUM:			//---- Stop Summary Report ----
				Stop_Sum_Report ();
				break;
			case STOP_GROUP:		//---- Stop Group Report ----
				Stop_Group_Report ();
				break;
			case STOP_RUN:			//---- stop run detail ----
				Stop_Run_Report ();
				break;
			case STOP_ROUTE:		//---- stop run summary ----
				Stop_Route_Report ();
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

void RiderSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case LINE_RIDERS:		//---- Line Rider Profle ----
			Line_Rider_Header ();
			break;
		case GROUP_RIDERS:		//---- Group Rider Profle ----
			Group_Rider_Header ();
			break;
		case LINE_SUM:			//---- Line Summary Report ----
			Line_Sum_Header ();
			break;
		case LINE_GROUP:		//---- Line Group Report ----
			Line_Group_Header ();
			break;
		case STOP_SUM:			//---- Stop Summary Report ----
			Stop_Sum_Header ();
			break;
		case STOP_GROUP:		//---- Stop Group Report ----
			Stop_Group_Header ();
			break;
		case STOP_RUN:			//---- Stop Run Detail ----
			Stop_Run_Header ();
			break;
		case STOP_ROUTE:		//---- Stop Route Summary ----
			Stop_Route_Header ();
			break;
		default:
			break;
	}
}
