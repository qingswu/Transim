//*********************************************************
//	Group_Report.cpp - group performance report
//*********************************************************

#include "LinkSum.hpp"

#define LANE_MILES	0
#define VMT			1
#define VHT			2
#define VHD			3
#define TURNS		4

//---------------------------------------------------------
//	Group_Report
//---------------------------------------------------------

void LinkSum::Group_Report (void)
{
	int i, j, k, lanes, max_lanes, tod_list, index, flow_index, num, link;
	double base_time, loaded_time, len, flow, factor, flow_factor, cap;
	Dtime low, high, tod;
	bool connect_flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Flow_Time_Period_Itr turn_itr;
	Flow_Time_Data flow_data, *turn_ptr;
	Lane_Use_Period *use_ptr;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;
	Int_Set *group;
	Int_Set_Itr group_itr;
	Int_Map_Itr map_itr;

	Show_Message ("Creating the Group Performance Summary Report -- Record");
	Set_Progress ();

	connect_flag = System_Data_Flag (CONNECTION) && (turn_perf_array.size () > 0);

	tod = sum_periods.Increment ();
	if (tod < 1) {
		flow_factor = (double) Dtime (60, MINUTES) / tod;
	} else {
		flow_factor = 1.0;
	}

	//---- process each link group ----

	num = link_equiv.Max_Group ();

	for (i=1; i <= num; i++) {

		group = link_equiv.Group_List (i);
		if (group == 0) continue;

		header_label ("%d. %s") % i % link_equiv.Group_Label (i);

		//---- clear the summary bins -----

		for (itr = sum_bin.begin (); itr != sum_bin.end (); itr++) {
			itr->assign (NUM_SUM_BINS, 0.0);
		}

		//---- process each link in the link group ----

		for (group_itr = group->begin (); group_itr != group->end (); group_itr++) {
			Show_Progress ();
			link = *group_itr;

			map_itr = link_map.find (abs (link));
			if (map_itr == link_map.end ()) continue;

			link_ptr = &link_array [map_itr->second];
		
			if (select_flag && link_ptr->Use () == 0) continue;

			if (link < 0) {
				if (link_ptr->Use () == -1) continue;
				index = link_ptr->BA_Dir ();
			} else {
				if (link_ptr->Use () == -2) continue;
				index = link_ptr->AB_Dir ();
			}
			if (index < 0) continue;

			len = UnRound (link_ptr->Length ());

			dir_ptr = &dir_array [index];

			base_time = dir_ptr->Time0 ();
			if (base_time <= 0.0) continue;

			tod_list = dir_ptr->First_Lane_Use ();
			flow_index = dir_ptr->Flow_Index ();

			lanes = dir_ptr->Lanes ();
			if (lanes < 1) lanes = 1;
			max_lanes = lanes;

			//---- process each time period ----

			for (j=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, j++) {
				flow_data = period_itr->Total_Flow_Time (index, flow_index);

				loaded_time = flow_data.Time ();
				flow = flow_data.Flow ();

				//---- check the time ratio ----

				if (select_ratio) {
					if ((double) loaded_time / dir_ptr->Time0 () < time_ratio) continue;
				}

				if (tod_list >= 0) {

					//---- get the time period ----

					sum_periods.Period_Range (j, low, high);
					tod = (low + high + 1) / 2;

					lanes = max_lanes;
					k = tod_list;

					for (use_ptr = &use_period_array [k]; ; use_ptr = &use_period_array [++k]) {
						if (use_ptr->Start () <= tod && tod < use_ptr->End ()) {
							lanes = use_ptr->Lanes0 () + use_ptr->Lanes1 ();
							break;
						}
						if (use_ptr->Periods () == 0) break;
					}
				}

				//---- check the vc ratio ----

				if (select_vc) {
					cap = dir_ptr->Capacity () * lanes / dir_ptr->Lanes ();
					if (cap > 0) {
						if (flow_factor * flow / cap < vc_ratio) continue;
					}
				}
				sum_bin [j] [LANE_MILES] += lanes * len;
				sum_bin [j] [VMT] += flow * len;
				sum_bin [j] [VHT] += flow * loaded_time;
				sum_bin [j] [VHD] += flow * (loaded_time - base_time);
			}

			//---- get the turning movements ----

			if (connect_flag) {
				for (k=dir_ptr->First_Connect (); k >= 0; k = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [k];

					if (connect_ptr->Type () != LEFT && connect_ptr->Type () != RIGHT &&
						connect_ptr->Type () != UTURN) continue;

					for (j=0, turn_itr = turn_perf_array.begin (); turn_itr != turn_perf_array.end (); turn_itr++, j++) {
						turn_ptr = &turn_itr->at (k);

						sum_bin [j] [TURNS] += turn_ptr->Flow ();
					}
				}
			}
		}

		//---- print the report ----

		Header_Number (GROUP_REPORT);

		if (!Break_Check (num_inc + 7)) {
			Print (1);
			Group_Header ();
		}
		if (Metric_Flag ()) {
			factor = 1.0 / 1000.0;
		} else {
			factor = 1.0 / MILETOFEET;
		}
		tod.Hours (1);

		for (j=0; j <= num_inc; j++) {
			if (j == num_inc) {
				Print (2, "Total        ");
			} else {
				if (sum_bin [j] [VHT] == 0.0) continue;

				Print (1, String ("%-12.12s ") % sum_periods.Range_Format (j));

				sum_bin [num_inc] [LANE_MILES] = MAX (sum_bin [j] [LANE_MILES], sum_bin [num_inc] [LANE_MILES]);
				sum_bin [num_inc] [VMT] += sum_bin [j] [VMT];
				sum_bin [num_inc] [VHT] += sum_bin [j] [VHT];
				sum_bin [num_inc] [VHD] += sum_bin [j] [VHD];
				sum_bin [num_inc] [TURNS] += sum_bin [j] [TURNS];
			}
			len = (sum_bin [j] [VMT] * factor);
			base_time = sum_bin [j] [VHT] / tod;
			if (base_time == 0.0) {
				loaded_time = len;
			} else {
				loaded_time = len / base_time;
			}
			Print (0, String ("%12.2lf %13.2lf %13.2lf %9.2lf %13.2lf %10d") % 
				(sum_bin [j] [LANE_MILES] * factor) % len % base_time % 
				loaded_time % (sum_bin [j] [VHD] / tod) % ((int) (sum_bin [j] [TURNS])));
		}
		Header_Number (0);
	}
	End_Progress ();
}

//---------------------------------------------------------
//	Group_Header
//---------------------------------------------------------

void LinkSum::Group_Header (void)
{
	String units;

	if (Metric_Flag ()) {
		units = "Kilometers";
	} else {
		units = "Miles";
	}
	Print (1, "Group Performance Summary -- ") << header_label;
	Print (2, String ("                   Lane        Vehicle       Vehicle %10.10s     Hours of      Number") % units);
	Print (1, String ("Time Period   %10.10s    %10.10s        Hours       /Hour       Delay      of Turns") % units % units);
	Print (1);
}

/*********************************************|***********************************************

	Group Performance Summary -- sssssssssssssssssss

	                  Lane        Vehicle       Vehicle  KilomMiles    Hours of      Number
	Time Period  KilomMiles    KilomMiles        Hours        /Hour      Delay      of Turns
	
	xx:xx..xx:xx fffffffff.ff ffffffffff.ff ffffffffff.ff ffffff.ff ffffffffff.ff dddddddddd

	Total        fffffffff.ff ffffffffff.ff ffffffffff.ff ffffff.ff ffffffffff.ff dddddddddd

**********************************************|***********************************************/ 
