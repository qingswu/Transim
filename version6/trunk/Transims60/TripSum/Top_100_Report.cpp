//*********************************************************
//	Top_100_report.cpp - Create a Top 100 Link/Lane/VC Report
//*********************************************************

#include "TripSum.hpp"

//---------------------------------------------------------
//	Top_100_Report
//---------------------------------------------------------

void TripSum::Top_100_Report (int report_num)
{
	int i, j, k, dir, from, to, link, volume, cap, tod_cap, lanes, tod_lanes, tod, list, criteria, period;
	int p, num_periods, anode, bnode;
	Dtime low, high, hour, half_hour;

	Ints_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Node_Data *node_ptr;
	Integers *trips;
	Lane_Use_Period *use_ptr;

	typedef struct {
		int criteria;
		int link;
		int from;
		int to;
		int period;
		int volume;
		int capacity;
	} Top_Data;

	Top_Data top_data [100], *top_ptr;

	hour = Dtime (60, MINUTES);
	half_hour = Dtime (30, MINUTES);

	memset (top_data, '\0', 100 * sizeof (Top_Data));
	num_periods = sum_periods.Num_Periods ();
	criteria = 0;

	//---- find the top 100 records ----

	for (link=0, link_itr = link_trip_data.begin (); link_itr != link_trip_data.end (); link_itr++, link++) {
		trips = &(*link_itr);

		link_ptr = &link_array [link];

		node_ptr = &node_array [link_ptr->Anode ()];
		anode = node_ptr->Node ();

		node_ptr = &node_array [link_ptr->Bnode ()];
		bnode = node_ptr->Node ();

		//---- process each direction ----

        for (i=0; i < 2; i++) {
			dir = ((i) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ());

			if (dir < 0) continue;
			dir_ptr = &dir_array [dir];

			if (dir_ptr->Capacity () <= 0) continue;

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;

			list = dir_ptr->First_Lane_Use ();

			//---- set the direction index ----

			if (i) {
				from = bnode;
				to = anode;
			} else {
				from = anode;
				to = bnode;
			}

			//---- process each time period ----

			for (p=0; p < num_periods; p++) {
				j = p * 2 + i;

				volume = trips->at (j);
				if (volume == 0) continue;
				if (link_ptr->AB_Dir () >= 0 && link_ptr->BA_Dir () >= 0) volume /= 2;

				//---- get the time period ----

				sum_periods.Period_Range (p, low, high);

				tod = (high + low + 1) / 2;
				period = high - low + 1;

				//---- time period capacity ----

				cap = (dir_ptr->Capacity () * period + half_hour) / hour;
				if (cap <= 0) continue;

				tod_cap = cap;
				tod_lanes = lanes;

				if (list > 0) {

					//---- get the time period ----

					k = list;

					for (use_ptr = &use_period_array [k]; ; use_ptr = &use_period_array [++k]) {
						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							tod_lanes = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
							tod_cap = (tod_cap * tod_lanes + lanes / 2) / lanes;
							if (tod_cap == 0) tod_cap = cap / 2;
							break;
						}
						if (use_ptr->Periods () == 0) break;
					}
				}

				//---- set the Top 100 criteria ----

				switch (report_num) {
					case TOP_100_LINKS:
						criteria = volume;
						break;
					case TOP_100_LANES:
						if (tod_lanes > 0) {
							criteria = (volume + tod_lanes / 2) / tod_lanes;
						} else {
							criteria = 0;
						}
						break;
					case TOP_100_VC_RATIOS:
						criteria = (volume * 100 + tod_cap / 2) / tod_cap;
						break;
				}
				top_ptr = top_data;

				for (k=0; k < 100; k++, top_ptr++) {
					if (criteria > top_ptr->criteria) {
						if (top_ptr->criteria > 0 && k < 99) {
							memmove (top_ptr+1, top_ptr, (99-k) * sizeof (Top_Data));
						}
						top_ptr->criteria = criteria;
						top_ptr->link = link;
						top_ptr->from = from;
						top_ptr->to = to;
						top_ptr->period = p;
						top_ptr->volume = volume;
						top_ptr->capacity = tod_cap;
						break;
					}
				}
			}
		}
	}

	//---- print the report ----

	top_ptr = top_data;

	Header_Number (report_num);

	if (!Break_Check (100 + 7)) {
		Print (1);
		Top_100_Header (report_num);
	}		

	for (k=0; k < 100; k++, top_ptr++) {
		if (top_ptr->criteria == 0) break;

		//---- print the data record ----

		Print (1, String ("%8d  %8d  %8d   %7d  %12.12s  %7d    %6.2lf") % 
			top_ptr->link % top_ptr->from % top_ptr->to % top_ptr->capacity % 
			sum_periods.Range_Format (top_ptr->period) % 
			top_ptr->volume % ((double) top_ptr->volume / top_ptr->capacity));
	}
	if (k) {
		Print (2, "Number of Records in the Report = ") << k;
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_Header
//---------------------------------------------------------

void TripSum::Top_100_Header (int report_num)
{
	switch (report_num) {
		case TOP_100_LINKS:
			Print (1, "Top 100 Link Trip Ends");
			break;
		case TOP_100_LANES:
			Print (1, "Top 100 Lane Trip Ends");
			break;
		case TOP_100_VC_RATIOS:
			Print (1, "Top 100 Trip/Capacity Ratios");
			break;
	}
	Print (2, "    Link      From   To-Node  Capacity   Time-of-Day  TripEnds  T/C Ratio");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Link Trip End Report
	Top 100 Lane Trip End Report
	Top 100 V/C Ratio Report

	    Link      From   To-Node  Capacity   Time-of-Day  TripEnds  T/C Ratio

	dddddddd  dddddddd  dddddddd   ddddddd  dd:dd..dd:dd  ddddddd     ff.ff

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
