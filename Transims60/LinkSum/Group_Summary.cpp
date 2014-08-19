//*********************************************************
//	Group_Summary.cpp - group performance summary
//*********************************************************

#include "LinkSum.hpp"

#define LINKS		0
#define LENGTH		1
#define LANES		2
#define MAX_DEN		3
#define MAX_QUEUE	4
#define VMT			5
#define VHT			6
#define VHD			7
#define TIME_RATIO	8
#define DENSITY		9
#define QUEUE		10
#define FAILURE		11
#define TURNS		12
#define CONG_VMT	13
#define CONG_VHT	14
#define CONG_TIME	15
#define COUNT		16
#define PREV		14

//---------------------------------------------------------
//	Group_Sum_Report
//---------------------------------------------------------

void LinkSum::Group_Sum_Report (void)
{
	int i, j, k, k1, index, use_index, num, link;
	double time, length, len, base, diff, value, percent, factor, flow_factor;
	Dtime low, high, tod;
	bool connect_flag;
	String units, vmt, lane_mi;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Perf_Data perf_data;
	Turn_Period_Itr turn_itr;
	Turn_Period *compare_ptr;
	Turn_Data *turn_ptr;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;
	Int_Set *group;
	Int_Set_Itr group_itr;
	Int_Map_Itr map_itr;
	Performance_Data data;

	Show_Message ("Creating the Group Performance Summary -- Record");
	Set_Progress ();
	
	connect_flag = System_Data_Flag (CONNECTION) && (turn_period_array.size () > 0) && (compare_turn_array.size () > 0);

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

			dir_ptr = &dir_array [index];
			use_index = dir_ptr->Use_Index ();

			length = UnRound (link_ptr->Length ());

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
				sum_bin [j] [LINKS] += 1;
				sum_bin [j] [LENGTH] += length;
				sum_bin [j] [LANES] += data.Lane_Len ();
				sum_bin [j] [VMT] += data.Veh_Dist ();
				sum_bin [j] [VHT] += data.Veh_Time ();
				sum_bin [j] [VHD] += data.Veh_Delay ();
				sum_bin [j] [TIME_RATIO] += data.Time_Ratio ();
				sum_bin [j] [DENSITY] += data.Density ();
				sum_bin [j] [MAX_DEN] = MAX (sum_bin [j] [MAX_DEN], data.Max_Density ());
				sum_bin [j] [QUEUE] += data.Queue ();
				sum_bin [j] [MAX_QUEUE] = MAX (sum_bin [j] [MAX_QUEUE], data.Max_Queue ());
				sum_bin [j] [FAILURE] += data.Failure ();

				if (Ratio_Flag ()) {
					sum_bin [j] [CONG_VMT] += data.Ratio_Dist ();
					sum_bin [j] [CONG_VHT] += data.Ratio_Time ();
					sum_bin [j] [CONG_TIME] += data.Ratios ();
					sum_bin [j] [COUNT] += data.Count ();
				}

				if (compare_flag) {
					period_ptr = &compare_perf_array [j];
					perf_data = period_ptr->Total_Performance (index, use_index);

					data.Get_Data (&perf_data, dir_ptr, link_ptr);

					sum_bin [j] [VMT+PREV] += data.Veh_Dist ();
					sum_bin [j] [VHT+PREV] += data.Veh_Time ();
					sum_bin [j] [VHD+PREV] += data.Veh_Delay ();
					sum_bin [j] [TIME_RATIO+PREV] += data.Time_Ratio ();
					sum_bin [j] [DENSITY+PREV] += data.Density ();
					sum_bin [j] [MAX_DEN+PREV] = MAX (sum_bin [j] [MAX_DEN+PREV], data.Max_Density ());
					sum_bin [j] [QUEUE+PREV] += data.Queue ();
					sum_bin [j] [MAX_QUEUE+PREV] = MAX (sum_bin [j] [MAX_QUEUE+PREV], data.Max_Queue ());
					sum_bin [j] [FAILURE+PREV] += data.Failure ();

					if (Ratio_Flag ()) {
						sum_bin [j] [CONG_VMT+PREV] += data.Ratio_Dist ();
						sum_bin [j] [CONG_VHT+PREV] += data.Ratio_Time ();
						sum_bin [j] [CONG_TIME+PREV] += data.Ratios ();
						sum_bin [j] [COUNT+PREV] += data.Count ();
					}
				}
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

						if (compare_flag) {
							compare_ptr = &compare_turn_array [j];
							turn_ptr = compare_ptr->Data_Ptr (k);

							sum_bin [j] [TURNS+PREV] += turn_ptr->Turn ();
						}
					}
				}
			}
		}

		//---- print the report ----

		Header_Number (GROUP_SUMMARY);

		if (!Break_Check (num_inc * 16)) {
			Print (1);
			Group_Sum_Header ();
		}
		if (Metric_Flag ()) {
			factor = 1.0 / 1000.0;
			units = "Kilometers";
			vmt = "VKT";
			lane_mi = "km";
		} else {
			factor = 1.0 / MILETOFEET;
			units = "Miles";
			vmt = "VMT";
			lane_mi = "mi";
		}
		tod.Hours (1);

		for (j=0; j <= num_inc; j++) {
			len = sum_bin [j] [LINKS];
			if (len == 0.0) continue;

			if (j < num_inc) {
				for (k = 0; k <= COUNT; k++) {
					k1 = k + PREV;
					if (k < VMT) {
						sum_bin [num_inc] [k] = MAX (sum_bin [j] [k], sum_bin [num_inc] [k]);

						if (compare_flag) {
							sum_bin [num_inc] [k1] = MAX (sum_bin [j] [k1], sum_bin [num_inc] [k1]);
						}
					} else {
						sum_bin [num_inc] [k] += sum_bin [j] [k];

						if (compare_flag) {
							sum_bin [num_inc] [k1] += sum_bin [j] [k1];
						}
					}
				}
			}
			if (j < num_inc && sum_bin [j] [VHT] == 0.0 && (!compare_flag || sum_bin [j] [VHT+PREV] == 0.0)) continue;
			if (j) {
				if (!Break_Check ((Ratio_Flag () ? 19 : 16))) {
					Print (1);
				}
			}
			Print (1, String ("Time Period%22c") % BLANK);
			if (j == num_inc) {
				Print (0, "       Total");
				len *= num_inc;
			} else {
				Print (0, String ("%12.12s") % sum_periods.Range_Format (j));
			}
			Print (1, String ("Number of Links                 %13.2lf") % sum_bin [j] [LINKS]);
			Print (1, String ("Number of Roadway %-10.10s    %13.2lf") % units % (sum_bin [j] [LENGTH] * factor));
			Print (1, String ("Number of Lane %-10.10s       %13.2lf") % units % (sum_bin [j] [LANES] * factor));
			Print (1, String ("Vehicle %-20.20s    %13.2lf") % (units + " of Travel") % (sum_bin [j] [VMT] * factor));
			if (compare_flag) {
				base = sum_bin [j] [VMT+PREV] * factor;
				diff = sum_bin [j] [VMT] * factor - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Vehicle Hours of Travel         %13.2lf") % (sum_bin [j] [VHT] / tod));
			if (compare_flag) {
				base = sum_bin [j] [VHT+PREV] / tod;
				diff = sum_bin [j] [VHT] / tod - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}	
			Print (1, String ("Vehicle Hours of Delay          %13.2lf") % (sum_bin [j] [VHD] / tod));
			if (compare_flag) {
				base = sum_bin [j] [VHD+PREV] / tod;
				diff = sum_bin [j] [VHD] / tod - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Number of Queued Vehicles       %13.2lf") % UnRound (sum_bin [j] [QUEUE]));
			if (compare_flag) {
				base = sum_bin [j] [QUEUE+PREV];
				diff = sum_bin [j] [QUEUE] - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Maximum Queued Vehicles         %13.2lf") % sum_bin [j] [MAX_QUEUE]);
			if (compare_flag) {
				base = sum_bin [j] [MAX_QUEUE+PREV];
				diff = sum_bin [j] [MAX_QUEUE] - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Number of Cycle Failures        %13.2lf") % sum_bin [j] [FAILURE]);
			if (compare_flag) {
				base = sum_bin [j] [FAILURE+PREV];
				diff = sum_bin [j] [FAILURE] - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Number of Turning Movements     %13.2lf") % sum_bin [j] [TURNS]);
			if (compare_flag) {
				base = sum_bin [j] [TURNS+PREV];
				diff = sum_bin [j] [TURNS] - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Average Link Time Ratio         %13.2lf") % (sum_bin [j] [TIME_RATIO] / len));
			if (compare_flag) {
				base = sum_bin [j] [TIME_RATIO+PREV] / len;
				diff = sum_bin [j] [TIME_RATIO] / len - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Average Link Density (/ln-%s)   %13.2lf") % lane_mi % UnRound (sum_bin [j] [DENSITY] / len));
			if (compare_flag) {
				base = UnRound (sum_bin [j] [DENSITY+PREV] / len);
				diff = UnRound (sum_bin [j] [DENSITY] / len) - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			Print (1, String ("Maximum Link Density (/ln-%s)   %13.2lf") % lane_mi % UnRound (sum_bin [j] [MAX_DEN]));
			if (compare_flag) {
				base = UnRound (sum_bin [j] [MAX_DEN+PREV]);
				diff = UnRound (sum_bin [j] [MAX_DEN]) - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}
			length = sum_bin [j] [VMT] * factor;
			time = sum_bin [j] [VHT] / tod;
			if (time == 0.0) {
				time = length;
			} else {
				time = length / time;
			}
			Print (1, String ("Average %-19.19s     %13.2lf") % (units + " per Hour") % time);
			if (compare_flag) {
				length = sum_bin [j] [VMT+PREV] * factor;
				base = sum_bin [j] [VHT+PREV] / tod;
				if (base == 0.0) {
					base = length;
				} else {
					base = length / base;
				}
				diff = time - base;

				Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
			}

			if (Ratio_Flag ()) {
				value = sum_bin [j] [VMT];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [j] [CONG_VMT] / value;

				Print (1, String ("Percent %s Congested           %13.2lf") % vmt % percent);
				if (compare_flag) {
					value = sum_bin [j] [VMT+PREV];
					if (value == 0.0) value = 1.0;
					base = 100.0 * sum_bin [j] [CONG_VMT+PREV] / value;
					diff = percent - base;

					Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
				}
				value = sum_bin [j] [VHT];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [j] [CONG_VHT] / value;

				Print (1, String ("Percent VHT Congested           %13.2lf") % percent);
				if (compare_flag) {
					value = sum_bin [j] [VHT+PREV];
					if (value == 0.0) value = 1.0;
					base = 100.0 * sum_bin [j] [CONG_VHT+PREV] / value;
					diff = percent - base;

					Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
				}
				value = sum_bin [j] [COUNT];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [j] [CONG_TIME] / value;

				Print (1, String ("Percent Time Congested          %13.2lf") % percent);
				if (compare_flag) {
					value = sum_bin [j] [COUNT+PREV];
					if (value == 0.0) value = 1.0;
					base = 100.0 * sum_bin [j] [CONG_TIME+PREV] / value;
					diff = percent - base;

					Print (0, String (" %13.2lf %13.2lf  (%.2lf%%)") % base % diff % ((base > 0.0) ? (100.0 * diff / base) : 0.0) % FINISH);
				}
			}
		}
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Group_Sum_Header
//---------------------------------------------------------

void LinkSum::Group_Sum_Header (void)
{
	Print (1, "Group Performance Details -- ") << header_label;
	Print (2, String ("%36cCurrent") % BLANK);
	if (compare_flag) {
		Print (0, "      Previous      Difference  Percent");
	}
	Print (1);
}

/*********************************************|***********************************************

	Group Performance Details -- sssssssssssssssssssssssss
	
	                                    Current      Previous      Difference  Percent

	Time Period                      xx:xx..xx:xx
	Number of Links                  fffffffff.ff
	Number of Roadway Miles          fffffffff.ff
	Number of Lane Miles             fffffffff.ff
	Vehicle Miles of Travel          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Vehicle Hours of Travel          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Vehicle Hours of Delay           fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Queued Vehicles          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Maximum Queued Vehicles          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Number of Cycle Failures         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Number of Turning Movements      fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Time Ratio          fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Density             fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Link Max Density         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Average Miles Per Hour           fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of VMT Congested         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of VHT Congested         fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)
	Percent of Time Congested        fffffffff.ff  fffffffff.ff  fffffffff.ff  (f.ff%)

**********************************************|***********************************************/ 
