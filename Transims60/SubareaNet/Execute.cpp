//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SubareaNet.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SubareaNet::Execute (void)
{

	//---- read the network ----

	No_Warnings (true);

	Data_Service::Execute ();

	No_Warnings (false);

	//---- save the node file ----

	Write_Node ();

	//---- save the shape file ----

	if (System_File_Flag (NEW_SHAPE)) {
		Write_Shapes ();
	}

	//---- write the phasing records ----

	if (System_File_Flag (NEW_PHASING_PLAN)) {
		Write_Phasing_Plans ();
	}

	//---- write the boundary records ----

	Write_Boundary ();

	//---- save the transit files ----

	if (transit_flag) {
		Write_Route ();
	}

	//---- summary statistics ----

	Break_Check (7);
	Print (1);
	Write (1, "Number of Subarea Node Records = ") << nnode;
	Write (1, "Number of Subarea Link Records = ") << nlink;
	Write (1, "Number of Subarea Pocket Records = ") << npocket;
	Write (1, "Number of Subarea Connection Records = ") << nconnect;
	Write (1, "Number of Subarea Locations = ") << nlocation;
	Write (1, "Number of Subarea Parking Lots = ") << nparking;

	Break_Check (11);
	if (System_File_Flag (NEW_ACCESS_LINK)) {
		Write (1, "Number of Subarea Access Links = ") << naccess;
	}	
	if (System_File_Flag (NEW_ZONE)) {
		Write (1, "Number of Subarea Zone Records = ") << nzone;
	}
	if (System_File_Flag (NEW_SHAPE)) {
		Write (1, "Number of Subarea Shape Records = ") << nshape;
	}
	if (System_File_Flag (NEW_LANE_USE)) {
		Write (1, "Number of Subarea Lane Use Records = ") << nlane_use;
	}
	if (System_File_Flag (NEW_TURN_PENALTY)) {
		Write (1, "Number of Subarea Turn Penalty Records = ") << nturn;
	}
	if (System_File_Flag (NEW_SIGN)) {
		Write (1, "Number of Subarea Sign Records = ") << nsign;
	}
	if (System_File_Flag (NEW_SIGNAL)) {
		Write (1, "Number of Subarea Signal Records = ") << nsignal;
	}
	if (System_File_Flag (NEW_TIMING_PLAN)) {
		Write (1, "Number of Subarea Timing Plan Records = ") << ntiming;
	}
	if (System_File_Flag (NEW_PHASING_PLAN)) {
		Write (1, "Number of Subarea Phasing Plan Records = ") << nphasing;
	}
	if (System_File_Flag (NEW_DETECTOR)) {
		Write (1, "Number of Subarea Detector Records = ") << ndetector;
	}

	if (transit_flag) {
		Write (1, "Number of Subarea Transit Stop Records = ") << nstop;
		Write (1, "Number of Subarea Transit Route Records = ") << nroute;
		Write (1, "Number of Subarea Transit Schedule Records = ") << nschedule;
		Write (1, "Number of Subarea Transit Driver Records = ") << ndriver;
	}
	if (System_File_Flag (NEW_PERFORMANCE)) {
		Write (1, "Number of Subarea Performance Records = ") << nperf;
	}
	if (System_File_Flag (NEW_TURN_DELAY)) {
		Write (1, "Number of Subarea Turn Delay Records = ") << ndelay;
	}
	Break_Check (7);
	Write (2, "Number of Boundary Links = ") << nboundary;
	Write (1, "Number of Short Links Skipped = ") << nshort;
	Write (1, "Number of New Zones = ") << new_zone;
	Write (1, "Number of New Locations = ") << new_location;
	Write (1, "Number of New Parking Lots = ") << new_parking;
	if (System_File_Flag (NEW_ACCESS_LINK)) {
		Write (1, "Number of New Access Links = ") << new_access;
	}
	if (transit_flag) {
		Write (1, "Number of New Transit Stops = ") << new_stop;
		Write (1, "Number of New Transit Routes = ") << new_route;
	}
	Show_Message (1);

	Exit_Stat (DONE);
}
