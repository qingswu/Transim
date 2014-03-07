//*********************************************************
//	Zone_Report.cpp - Field Statistic Report
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Attraction_Report
//---------------------------------------------------------

void TourData::Attraction_Report (void)
{
	int i, zone, attr;
	Integers totals;
	
	Table_Itr table_itr;
	Int_Map_Itr map_itr;

	Show_Message ("Zone Attractions Report -- Record");
	Set_Progress ();

	Header_Number (ATTRACTIONS);
	New_Page ();

	totals.assign (table_groups.size (), 0);

	for (map_itr = zone_map.begin (); map_itr != zone_map.end (); map_itr++) {
		zone = map_itr->first;
		Show_Progress ();

		Print (1, String ("%5d") % zone);

		for (i=0, table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++, i++) {
			attr = table_itr->zone_wt [zone];
			Print (0, String (" %8d") % attr);
			totals [i] += attr;
		}
	}
	End_Progress ();

	Print (2, "Total");

	for (i=0, table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++, i++) {
		Print (0, String (" %8d") % totals [i]);
	}

	Header_Number (0);
}

//---------------------------------------------------------
//	Attraction_Header
//---------------------------------------------------------

void TourData::Attraction_Header (void)
{
	Table_Itr table_itr;

	Print (1, "Zone Attractions Report");

	Print (2, " Zone");

	for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
		Print (0, "  Group_") << table_itr->group;
	}
	Print (1);
}

/*********************************************|***********************************************

	Zone Attractions Report

	 Zone  Group_1  Group_2  Group_3

	ddddd dddddddd dddddddd dddddddd

	Total dddddddd dddddddd dddddddd

**********************************************|***********************************************/ 
