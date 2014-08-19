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
	int i, j, k, index, use_index, num, link;
	double len, time, speed, factor, flow_factor;
	Dtime low, high, tod;
	bool connect_flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Turn_Period_Itr turn_itr;
	Perf_Data perf_data;
	Turn_Data *turn_ptr;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;
	Int_Set *group;
	Int_Set_Itr group_itr;
	Int_Map_Itr map_itr;
	Performance_Data data;

	Show_Message ("Creating the Group Performance Summary Report -- Record");
	Set_Progress ();

	connect_flag = System_Data_Flag (CONNECTION) && (turn_period_array.size () > 0);

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
			use_index = dir_ptr->Use_Index ();

			//---- process each time period ----

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {
				sum_periods.Period_Range (j, low, high);

				data.Start (low);
				data.End (high);

				perf_data = period_itr->Total_Performance (index, use_index);

				data.Get_Data (&perf_data, dir_ptr, link_ptr);

				//---- check the time ratio ----

				if (select_ratio) {
					if (data.Time_Ratio () < time_ratio) continue;
				}

				//---- check the vc ratio ----

				if (select_vc) {
					if (data.VC_Ratio () < vc_ratio) continue;
				}
				sum_bin [j] [LANE_MILES] += data.Lane_Len ();
				sum_bin [j] [VMT] += data.Veh_Dist ();
				sum_bin [j] [VHT] += data.Veh_Time ();
				sum_bin [j] [VHD] += data.Veh_Delay ();
			}

			//---- get the turning movements ----

			if (connect_flag) {
				for (k=dir_ptr->First_Connect (); k >= 0; k = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [k];

					if (connect_ptr->Type () != LEFT && connect_ptr->Type () != RIGHT &&
						connect_ptr->Type () != UTURN) continue;

					for (j=0, turn_itr = turn_period_array.begin (); turn_itr != turn_period_array.end (); turn_itr++, j++) {
						turn_ptr = turn_itr->Data_Ptr (k);

						sum_bin [j] [TURNS] += turn_ptr->Turn ();
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
			time = sum_bin [j] [VHT] / tod;
			if (time == 0.0) {
				speed = len;
			} else {
				speed = len / time;
			}
			Print (0, String ("%12.2lf %13.2lf %13.2lf %9.2lf %13.2lf %10d") % 
				(sum_bin [j] [LANE_MILES] * factor) % len % time % 
				speed % (sum_bin [j] [VHD] / tod) % ((int) (sum_bin [j] [TURNS])));
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
