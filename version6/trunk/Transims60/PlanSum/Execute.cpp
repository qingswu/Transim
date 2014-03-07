//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanSum::Execute (void)
{
	int i, j, num, part;

	//---- read the network data ----

	Data_Service::Execute ();

	//---- initialize link delay data ----

	if (new_delay_flag && !System_File_Flag (LINK_DELAY)) {
		link_delay_array.Initialize (&time_periods);

		if (turn_flag) {
			num = (int) connect_array.size ();

			if (num > 0) {
				turn_delay_array.Initialize (&time_periods, num);
			}
		}
	}

	//---- initialize transfer data ----

	if (transfer_flag) {
		Transfer_Data transfer_data;
		transfer_array.assign (stop_array.size (), transfer_data);
	}

	if (boarding_flag) {
		int num = sum_periods.Num_Periods ();
		if (num < 1) num = 1;

		Integers stops;
		stops.assign (stop_array.size (), 0);

		boardings.assign (num, stops);
	}

	//---- set the processing queue ----

	num = plan_file->Num_Parts ();

	plan_file->Close ();
	plan_file->Reset_Counters ();
	
	for (part=0; part < num; part++) {
		partition_queue.Put (part);
	}
	partition_queue.End_of_Queue ();

	//---- processing threads ----

	Num_Threads (MIN (Num_Threads (), num));

	if (Num_Threads () > 1) {
#ifdef THREADS		
		Threads threads;

		for (i=0; i < Num_Threads (); i++) {
			threads.push_back (thread (Plan_Processing (this)));
		}
		threads.Join_All ();
#endif
	} else {
		Plan_Processing plan_processing (this);
		plan_processing ();
	}

	//---- combine MPI data ----

	MPI_Processing ();

	//---- print processing summary ----

	plan_file->Print_Summary ();

	//---- write the link volume file ----

	if (volume_flag) {
		Write_Volumes ();
	}

	//---- calculate the travel times ----

	if (new_delay_flag && Time_Updates ()) {
		Update_Travel_Times ();
	}
	if (System_File_Flag (NEW_LINK_DELAY)) {
		Show_Message (1);
		Write_Link_Delays ();
	}

	//---- transit ridership output ----

	if (rider_flag && System_File_Flag (NEW_RIDERSHIP)) {
		Write_Ridership ();
	}

	//---- write the trip time file ----

	if (time_flag && time_file.Is_Open ()) {
		Write_Times ();
	}

	//---- write access detail file ----

	if (access_file_flag) {
		Write_Access_Details ();
	}

	//---- write access group file ----

	if (access_group_flag) {
		Write_Access_Groups ();
	}

	//---- write stop diurnal file ----

	if (diurnal_flag) {
		Write_Stop_Diurnals ();
	}

	//---- write line on off file ----

	if (on_off_flag) {
		Write_Line_On_Off ();
	}

	//---- write stop boarding file ----

	if (boarding_flag) {
		Write_Boardings ();
	}

	//---- print reports ----

	for (i=First_Report (); i != 0; i=Next_Report ()) {
		switch (i) {
			case TOP_100:			//---- Top 100 V/C Ratio Report ----
				Top_100_Report ();
				break;
			case VC_RATIO:			//---- All V/C Ratios Greater Than x.xx ---
				VC_Ratio (Report_Data ());
				break;
			case LINK_GROUP:		//---- Link Group Report -----
				Link_Group (Report_Data ());
				break;
			case SUM_RIDERS:		//---- Transit Ridership Report ----
				Transit_Report ();
				break;
			case SUM_STOPS:			//---- Transit Stop Report ----
				Stop_Report ();
				break;
			case SUM_TRANSFERS:		//---- Transit Transfer Report ----
				Transfer_Report (0);
				break;
			case XFER_DETAILS:		//---- Transit Transfer Details ----
				num = sum_periods.Num_Periods ();
				if (num == 0) num++;

				for (j=1; j <= num; j++) {
					for (part=0; part < 2; part++) {
						Transfer_Report (j, part);
					}
				}
				break;
			case STOP_GROUP:		//---- Transit Stop Group Report ----
				Stop_Group_Report ();
				break;
			case STOP_GRP_DETAIL:	//---- Transit Stop Group Details ----
				Stop_Group_Detail ();
				break;
			case LINE_GROUP:		//---- Transit Line Group Report ----
				Line_Group_Report ();
				break;
			case LINE_GRP_DETAIL:	//---- Transit Line Group Details ----
				Line_Group_Detail ();
				break;
			case SUM_PASSENGERS:	//---- Transit Passenger Summary ----
				pass_sum_data.Travel_Sum_Report (SUM_PASSENGERS, false);
				break;
			case RIDER_GROUP:		//---- Transit Link Group Summary ----
				Rider_Group ();
				break;
			case LINE_TRANSFERS:	//---- line to line transfers ----
				Line_Transfers_Report ();
				break;
			case ACCESS_DETAILS:	//---- stop group access details ----
				Access_Detail_Report ();
				break;
			case TRIP_TIME:			//---- Trip Time Report ----
				Trip_Time ();
				break;
			case SUM_TRAVEL:		//---- Travel Summary Report ----
				trip_sum_data.Travel_Sum_Report (SUM_TRAVEL);
				break;
			default:
				break;
		}
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PlanSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOP_100:			//---- Top 100 V/C Ratio Report ----
			Top_100_Header ();
			break;
		case VC_RATIO:			//---- All V/C Ratios Greater Than x.xx ---
			VC_Ratio_Header ();
			break;
		case LINK_GROUP:		//---- Link Group Report -----
			Link_Group_Header ();
			break;
		case SUM_RIDERS:		//---- Transit Ridership Report ----
			Transit_Header ();
			break;
		case SUM_STOPS:			//---- Transit Stop Report ----
			Stop_Header ();
			break;
		case SUM_TRANSFERS:		//---- Transit Transfer Report ----
		case XFER_DETAILS:		//---- Transit Transfer Details ----
			Transfer_Header ();
			break;
		case STOP_GROUP:		//---- Transit Stop Group Report ----
			Stop_Group_Header ();
			break;
		case STOP_GRP_DETAIL:	//---- Transit Stop Group Detail ----
			Stop_Group_Detail_Header ();
			break;
		case LINE_GROUP:		//---- Transit Line Group Report ----
			Line_Group_Header ();
			break;
		case LINE_GRP_DETAIL:	//---- Transit Line Group Detail ----
			Line_Group_Detail_Header ();
			break;
		case SUM_PASSENGERS:	//---- Transit Passenger Summary ----
			pass_sum_data.Travel_Sum_Header ();
			break;
		case RIDER_GROUP:		//---- Transit Link Group Summary ----
			Rider_Header ();
			break;
		case LINE_TRANSFERS:	//---- line to line transfers -----
			Line_Transfers_Header ();
			break;
		case ACCESS_DETAILS:	//---- stop group access details -----
			Access_Detail_Header ();
			break;
		case TRIP_TIME:			//---- Trip Time Report ----
			Trip_Time_Header ();
			break;
		case SUM_TRAVEL:		//---- Travel Summary Report ----
			trip_sum_data.Travel_Sum_Header ();
			break;
		default:
			break;
	}
}
