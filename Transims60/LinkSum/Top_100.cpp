//*********************************************************
//	Top_100.cpp - Create a Top 100 Flow Report
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Top_100_Report
//---------------------------------------------------------

void LinkSum::Top_100_Report (int type)
{
	int i, j, k, anode, bnode, an, bn, max_tim, max_an, max_bn;
	int index, use_index, lanes;
	double volume, min_vol, max_vol;
	Dtime low, high, tod;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Data perf_data;
	Lane_Use_Period *use_ptr;

	//---- Top 100 Data ----

	typedef struct {
		int    link;
		int    from;
		int    to;
		int    period;
		double volume;
	} Volume_Data;

	Volume_Data vol_data [100], *vol_ptr;

	Show_Message ("Creating the Top 100 Volume Report -- Record");
	Set_Progress ();

	memset (vol_data, '\0', 100 * sizeof (Volume_Data));

	//---- find Top 100 Flows ----

	min_vol = 0;

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;

		an = node_array [link_itr->Anode ()].Node ();
		bn = node_array [link_itr->Bnode ()].Node ();

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_itr->Use () == -1) continue;
				index = link_itr->BA_Dir ();
				anode = bn;
				bnode = an;
			} else {
				if (link_itr->Use () == -2) continue;
				index = link_itr->AB_Dir ();
				anode = an;
				bnode = bn;
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];
			use_index = dir_ptr->Use_Index ();

			max_vol = max_tim = max_an = max_bn = 0;

			//---- scan each time period ----

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {
				perf_data = period_itr->Total_Performance (index, use_index);

				volume = perf_data.Volume ();
				if (volume < minimum_volume) continue;

				if (type == TOP_PERIOD) {
					if (volume <= min_vol) continue;

					vol_ptr = vol_data;

					for (k=0; k < 100; k++, vol_ptr++) {
						if (volume > vol_ptr->volume) {
							if (vol_ptr->volume > 0 && k < 99) {
								memmove (vol_ptr+1, vol_ptr, (99-k) * sizeof (Volume_Data));
								min_vol = vol_data [99].volume;
							}
							vol_ptr->link = link_itr->Link ();
							vol_ptr->from = anode;
							vol_ptr->to = bnode;
							vol_ptr->period = j;
							vol_ptr->volume = volume;
							break;
						}
					}
				} else {
					if (type == TOP_LANE_VOL) {
						lanes = dir_ptr->Lanes ();

						k = dir_ptr->First_Lane_Use ();
						if (k >= 0) {
							sum_periods.Period_Range (j, low, high);

							tod = (low + high + 1) / 2;

							for (use_ptr = &use_period_array [k]; ; use_ptr = &use_period_array [++k]) {
								if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
									lanes = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
									break;
								}
								if (use_ptr->Periods () == 0) break;
							}
						}
						if (lanes > 0) {
							volume /= lanes;
						}
					}
					volume = DTOI (volume);

					if (volume > max_vol) {
						max_vol = volume;
						max_tim = j;
						max_an = anode;
						max_bn = bnode;
					}
				}
			}

			//---- add to the list ----

			if (max_vol > min_vol) {
				vol_ptr = vol_data;

				for (k=0; k < 100; k++, vol_ptr++) {
					if (max_vol > vol_ptr->volume) {
						if (vol_ptr->volume > 0 && k < 99) {
							memmove (vol_ptr+1, vol_ptr, (99-k) * sizeof (Volume_Data));							
							min_vol = vol_data [99].volume;
						}
						vol_ptr->link = link_itr->Link ();
						vol_ptr->from = max_an;
						vol_ptr->to = max_bn;
						vol_ptr->period = max_tim;
						vol_ptr->volume = max_vol;
						break;
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	vol_ptr = vol_data;

	Header_Number (type);
	New_Page ();

	for (i=0; i < 100; i++, vol_ptr++) {
		if (vol_ptr->volume <= 0.0) break;

		//---- print the data record ----

		Print (1, String ("%10d%10d%10d  %12.12s%12.1lf") % vol_ptr->link % vol_ptr->from % vol_ptr->to % 
			sum_periods.Range_Format (vol_ptr->period) % vol_ptr->volume);
	}
	if (i) {		Print (2, "Number of Records in the Report = ") << i;
	}
	Header_Number (0);
}

//---------------------------------------------------------
//	Top_100_Link_Header
//---------------------------------------------------------

void LinkSum::Top_100_Link_Header (void)
{
	Print (1, "Top 100 Link Volume Report");
	Print (2, "      Link      From   To-Node   Time-of-Day      Volume");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Lane_Header
//---------------------------------------------------------

void LinkSum::Top_100_Lane_Header (void)
{
	Print (1, "Top 100 Lane Volume Report");
	Print (2, "      Link      From   To-Node   Time-of-Day      Volume");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Time_Header
//---------------------------------------------------------

void LinkSum::Top_100_Time_Header (void)
{
	Print (1, "Top 100 Period Volume Report");
	Print (2, "      Link      From   To-Node   Time-of-Day      Volume");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Link Volume Report

	      Link      From   To-Node   Time-of-Day      Volume

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd  dddddddd.d

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
