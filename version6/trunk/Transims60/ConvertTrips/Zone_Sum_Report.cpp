//*********************************************************
//	Zone_Sum_Report.cpp - Print the Zone Trip End Report
//*********************************************************

#include "ConvertTrips.hpp"

//---------------------------------------------------------
//	Zone_Sum_Report
//---------------------------------------------------------

void ConvertTrips::Zone_Sum_Report (void)
{
	int z, zone, org_in_tot, des_in_tot, org_out_tot, des_out_tot;
	Int_Map_Itr map_itr;

	//---- print the report ----

	Header_Number (ZONE_TRIP_ENDS);

	if (!Break_Check ((int) zone_map.size () + 7)) {
		Print (1);
		Zone_Sum_Header ();
	}
	org_in_tot = des_in_tot = org_out_tot = des_out_tot = 0;

	for (map_itr = zone_map.begin (); map_itr != zone_map.end (); map_itr++) {
		zone = map_itr->first;
		z = map_itr->second;

		org_in_tot += org_in [z];
		des_in_tot += des_in [z];
		org_out_tot += org_out [z];
		des_out_tot += des_out [z];

		Print (1, String ("%6d   %8d   %8d     %8d   %8d     %8d   %8d") % zone % org_in [z] % des_in [z] % 
			org_out [z] % des_out [z] % (org_out [z] - org_in [z]) % (des_out [z] - des_in [z]));
	}
	Print (2, String (" Total   %8d   %8d     %8d   %8d     %8d   %8d") % org_in_tot % des_in_tot % 
		org_out_tot % des_out_tot % (org_out_tot - org_in_tot) % (des_out_tot - des_in_tot));
		
	Header_Number (0);
}

//---------------------------------------------------------
//	Zone_Sum_Header
//---------------------------------------------------------

void ConvertTrips::Zone_Sum_Header (void)
{
	Print (1, "Zone Trip End Report");
	Print (2, "         ------- Input -------   ------ Output -------   ---- Difference -----");
	Print (1, "  Zone    Origins Destinations    Origins Destinations    Origins Destination");
	Print (1);
}
	 
/*********************************************|***********************************************

	Zone Trip End Report

	         ------- Input -------   ------ Output -------   ---- Difference -----
	  Zone    Origins Destinations    Origins Destinations    Origins Destinations
	
	dddddd   dddddddd   dddddddd     dddddddd   dddddddd     dddddddd   dddddddd

	 Total   dddddddd   dddddddd     dddddddd   dddddddd     dddddddd   dddddddd

**********************************************|***********************************************/ 
