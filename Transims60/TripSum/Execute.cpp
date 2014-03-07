//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void TripSum::Execute (void)
{
	int part, num;

	//---- read the network data ----

	Data_Service::Execute ();

	//---- allocate memory ----

	num = sum_periods.Num_Periods ();

	if (link_flag || loc_trip_flag || zone_trip_flag) {
		Integers data;
		data.assign (2 * num, 0);

		if (link_flag) {
			link_trip_data.assign (link_map.size (), data);
		}
		if (loc_trip_flag) {
			loc_trip_data.assign (location_map.size (), data);
		}
		if (zone_trip_flag) {
			zone_trip_data.assign (zone_map.size (), data);
		}
	}

	//---- set the processing queue ----

	num = trip_file->Num_Parts ();

	for (part=0; part < num; part++) {
		partition_queue.Put (part);
	}
	trip_file->Close ();
	trip_file->Reset_Counters ();

	partition_queue.End_of_Queue ();

	//---- processing threads ---

	Num_Threads (MIN (Num_Threads (), num));

	if (Num_Threads () > 1) {
#ifdef THREADS		
		Threads threads;

		for (int i=0; i < Num_Threads (); i++) {
			threads.push_back (thread (Trip_Processing (this, i)));
		}
		threads.Join_All ();
		Show_Message (1);
#endif
	} else {
		Trip_Processing trip_processing (this, 0);
		trip_processing ();
	}

	//---- combine MPI data ----

	MPI_Processing ();

	//---- trip time distribution ----

	if (diurnal_flag) {
		Write_Time_Distribution ();
	}

	//---- trip time file ----

	if (time_file_flag) {
		Write_Trip_Time ();
	}

	//---- trip length file ----

	if (len_file_flag) {
		Write_Trip_Length ();
	}

	//---- link trip ends ----

	if (link_trip_flag) {
		Write_Link_Trips ();
	}

	//---- location trip ends ----

	if (loc_trip_flag) {
		Write_Location_Trips ();
	}

	//---- zone trip ends ----

	if (zone_trip_flag) {
		Write_Zone_Trips ();
	}

	//---- print processing summary ----

	trip_file->Print_Summary ();

	//---- print reports ----

	for (int i=First_Report (); i != 0; i=Next_Report ()) {

		switch (i) {
			case TOP_100_LINKS:		//---- TOP_100_LINK_TRIP_ENDS ----
			case TOP_100_LANES:		//---- TOP_100_LANE_TRIP_ENDS ----
			case TOP_100_VC_RATIOS:	//---- TOP_100_TRIP/CAPACITY_RATIOS ----
				Top_100_Report (i);
				break;
				break;
			case TRIP_DIURNAL:			//---- TIME_DISTRIBUTION ----
				Trip_Diurnal_Report ();
				break;
			case TRIP_TIME:			//---- TRIP_TIME_REPORT ----
				Trip_Time_Report ();
				break;
			case TRIP_LENGTH:		//---- TRIP_LENGTH_SUMMARY ----
				trip_length.Trip_Sum_Report (TRIP_LENGTH, "Trip Length Summary", "Period");
				break;
			case TRIP_PURPOSE:		//---- TRIP_PURPOSE_SUMMARY ----
				trip_purpose.Trip_Sum_Report (TRIP_PURPOSE, "Trip Purpose Summary", " ", "Purpose");
				break;
			case MODE_LENGTH:		//---- MODE_LENGTH_SUMMARY ----
				mode_length.Trip_Sum_Report (MODE_LENGTH, "Mode Length Summary", " Mode");
				break;
			case MODE_PURPOSE:		//---- MODE_PURPOSE_SUMMARY -----
				mode_purpose.Trip_Sum_Report (MODE_PURPOSE, "Mode Purpose Summary", " Mode-", "Purpose");
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

void TripSum::Page_Header (void)
{
	switch (Header_Number ()) {
		case TOP_100_LINKS:		//---- TOP_100_LINK_TRIP_ENDS ----
		case TOP_100_LANES:		//---- TOP_100_LANE_TRIP_ENDS ----
		case TOP_100_VC_RATIOS:	//---- TOP_100_TRIP/CAPACITY_RATIOS ----
			Top_100_Header (Header_Number ());
			break;
		case TRIP_DIURNAL:		//---- TIME_DISTRIBUTION ----
			Trip_Diurnal_Header ();
			break;
		case TRIP_TIME:			//---- TRIP_TIME_REPORT ----
			Trip_Time_Header ();
			break;
		case TRIP_LENGTH:		//---- TRIP_LENGTH_SUMMARY ----
			trip_length.Trip_Sum_Header ();
			break;
		case TRIP_PURPOSE:		//---- TRIP_PURPOSE_SUMMARY ----
			trip_purpose.Trip_Sum_Header ();
			break;
		case MODE_LENGTH:		//---- MODE_LENGTH_SUMMARY ----
			mode_length.Trip_Sum_Header ();
			break;
		case MODE_PURPOSE:		//---- MODE_PURPOSE_SUMMARY -----
			mode_purpose.Trip_Sum_Header ();
			break;
		default:
			break;
	}
}

