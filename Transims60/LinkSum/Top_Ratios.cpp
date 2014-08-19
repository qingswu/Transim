//*********************************************************
//	Top_Ratios.cpp - Create a Top 100 Ratios Report
//*********************************************************

#include "LinkSum.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	Top_100_Ratios
//---------------------------------------------------------

void LinkSum::Top_100_Ratios (int type)
{
	int i, j, k, anode, bnode, an, bn, min_ratio, ratio;
	int base, load, index, use_index;
	Dtime low, high, tod;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Perf_Data perf_data;
	Performance_Data data;

	//---- Top 100 Ratio Report Data ----

	typedef struct {
		int link;
		int from;
		int to;
		int period;
		int base;
		int load;
		int ratio;
	} Ratio_Data;

	Ratio_Data ratios [100], *ptr;

	Show_Message ("Creating the Top 100 Ratios Report -- Record");
	Set_Progress ();

	memset (ratios, '\0', 100 * sizeof (Ratio_Data));

	//---- find Top 100 Ratios ----

	min_ratio = base = load = ratio = 0;

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

			//---- scan each time period ----

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {

				perf_data = period_itr->Total_Performance (index, use_index);

				if (perf_data.Volume () < minimum_volume) continue;

				sum_periods.Period_Range (j, low, high);

				data.Start (low);
				data.End (high);

				data.Get_Data (&perf_data, dir_ptr, &(*link_itr));

				ratio = 0;

				switch (type) {
					case TOP_SPEED:
						if (dir_ptr->Time0 () <= 0) continue;
						base = DTOI ((double) link_itr->Length () / dir_ptr->Time0 ());
						load = data.Speed ();
						break;
					case TOP_TIME_RATIO:
						base = dir_ptr->Time0 ();
						load = data.Time ();
						ratio = data.Time_Ratio () * 1000;
						break;
					case TOP_VC_RATIO:
						if (data.VC_Ratio () <= 0) continue;
						base = DTOI (data.Volume () / data.VC_Ratio ());
						load = DTOI (data.Volume ());
						ratio = DTOI (data.VC_Ratio () * 1000);
						break;
					case TOP_TIME_CHANGE:
						period_ptr = &compare_perf_array [j];

						perf_data = period_ptr->Total_Performance (index, use_index);
						if (perf_data.Volume () < minimum_volume) continue;
						base = perf_data.Time ();
						if (base == 0) continue;
						load = data.Time ();
						break;
					case TOP_VOL_CHANGE:
						period_ptr = &compare_perf_array [j];
						perf_data = period_ptr->Total_Performance (index, use_index);
						base = DTOI (perf_data.Volume ());
						if (base < minimum_volume) continue;
						load = DTOI (data.Volume ());
						break;
				}

				if (ratio == 0 && base > 0) {
					ratio = ((load - base) * 1000 + base / 2) / base;
				}
				if (ratio > min_ratio) {
					ptr = ratios;

					for (k=0; k < 100; k++, ptr++) {
						if (ratio > ptr->ratio) {
							if (ptr->ratio > 0 && k < 99) {
								memmove (ptr+1, ptr, (99-k) * sizeof (Ratio_Data));							
								min_ratio = ratios [99].ratio;
							}
							ptr->link = link_itr->Link ();
							ptr->from = anode;
							ptr->to = bnode;
							ptr->period = j;
							ptr->base = base;
							ptr->load = load;
							ptr->ratio = ratio;
							break;
						}
					}
				}
			}
		}
	}
	End_Progress ();

	//---- print the report ----

	Header_Number (type);
	New_Page ();

	ptr = ratios;

	for (i=0; i < 100; i++, ptr++) {
		if (ptr->ratio == 0) break;

		//---- print the data record ----

		Print (1, String ("%10ld%10ld%10ld  %12.12s   ") % ptr->link % ptr->from % ptr->to % 
			sum_periods.Range_Format (ptr->period));

		if (type == TOP_VC_RATIO || type == TOP_TIME_RATIO) {
			Print (0, String ("%7d  %7d   %5.2lf") % ptr->base % ptr->load % (ptr->ratio / 1000.0));
		} else if (type == TOP_VOL_CHANGE) {
			Print (0, String ("%7d  %7d   %5.1lf") % ptr->base % ptr->load % (ptr->ratio / 10.0));
		} else {
			Print (0, String ("%7.1lf  %7.1lf   %5.1lf") % (ptr->base / 10.0) % (ptr->load / 10.0) % (ptr->ratio / 10.0));
		}
	}
	if (i) {
		Print (2, "Number of Records in the Report = ") << i;
	}
	Header_Number (0);
}

#pragma pack (pop)

//---------------------------------------------------------
//	Top_100_Speed_Header
//---------------------------------------------------------

void LinkSum::Top_100_Speed_Header (void)
{
	Print (1, "Top 100 Speed Reductions Report");
	Print (2, "      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Travel_Time_Header
//---------------------------------------------------------

void LinkSum::Top_100_Travel_Time_Header (void)
{
	Print (1, "Top 100 Travel Time Ratios");
	Print (2, "      Link      From   To-Node   Time-of-Day      Base   Loaded    Ratio");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_VC_Header
//---------------------------------------------------------

void LinkSum::Top_100_VC_Header (void)
{
	Print (1, "Top 100 Volume Capacity Ratios");
	Print (2, "      Link      From   To-Node   Time-of-Day  Capacity   Volume   Ratio");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Time_Change_Header
//---------------------------------------------------------

void LinkSum::Top_100_Time_Change_Header (void)
{
	Print (1, "Top 100 Travel Time Changes");
	Print (2, "      Link      From   To-Node   Time-of-Day  Previous  Current  Percent");
	Print (1);
}

//---------------------------------------------------------
//	Top_100_Volume_Change_Header
//---------------------------------------------------------

void LinkSum::Top_100_Volume_Change_Header (void)
{
	Print (1, "Top 100 Volume Changes");
	Print (2, "      Link      From   To-Node   Time-of-Day  Previous  Current  Percent");
	Print (1);
}

/*********************************************|***********************************************

	Top 100 Speed Reductions Report

	      Link      From   To-Node   Time-of-Day      Base   Loaded  Percent

	dddddddddd  dddddddd  dddddddd  dd:dd..dd:dd   ddddddd  ddddddd   ddddd

	Number of Records in the Report = dddddd

**********************************************|***********************************************/ 
