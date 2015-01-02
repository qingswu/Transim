//*********************************************************
//	Group_Sum_File.cpp - group summary file
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
//	Group_Sum_File
//---------------------------------------------------------

void LinkSum::Group_Sum_File (void)
{
	int i, j, k, k1, index, use_index, link;
	int text_field, value_field, compare_field;
	double loaded_time, length, len, value, percent, factor;
	String buffer, units, vmt, lane_mi;
	bool connect_flag;
	Dtime low, high, tod;

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

	Show_Message (String ("Writing %s -- Record") % group_sum_file.File_Type ());;
	Set_Progress ();

	text_field = group_sum_file.Field_Number ("MEASURE");
	value_field = group_sum_file.Field_Number ("VALUE");
	compare_field = group_sum_file.Field_Number ("COMPARE");

	if (compare_flag) {
		connect_flag = System_Data_Flag (CONNECTION) && (turn_period_array.size () > 0) && (compare_turn_array.size () > 0);
	} else {
		connect_flag = System_Data_Flag (CONNECTION) && (turn_period_array.size () > 0);
	}

	//---- process each link group ----

	for (i = link_equiv.First_Group (); i > 0; i = link_equiv.Next_Group ()) {

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

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {
				sum_periods.Period_Range (j, low, high);

				data.Start (low);
				data.End (high);

				perf_data = period_itr->Total_Performance (index, use_index);

				if (data.Get_Data (&perf_data, dir_ptr, link_ptr, Maximum_Time_Ratio ())) {

					sum_bin [j] [LINKS] += 1;
					sum_bin [j] [LENGTH] += len;
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
				}

				if (compare_flag) {
					period_ptr = &compare_perf_array [j];
					perf_data = period_ptr->Total_Performance (index, use_index);

					if (data.Get_Data (&perf_data, dir_ptr, link_ptr, Maximum_Time_Ratio ())) {

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
			}

			//---- get the turning movements ----

			if (connect_flag) {
				for (k=dir_ptr->First_Connect (); k >= 0; k = connect_ptr->Next_Index ()) {
					connect_ptr = &connect_array [k];

					if (connect_ptr->Type () != LEFT && connect_ptr->Type () != RIGHT &&
						connect_ptr->Type () != UTURN) continue;

					for (j=0, turn_itr = turn_period_array.begin (); turn_itr != turn_period_array.end (); turn_itr++, j++) {
						turn_ptr = &turn_itr->at (k);

						sum_bin [j] [VHD] += turn_ptr->Time () * turn_ptr->Turn ();
						sum_bin [j] [TURNS] += turn_ptr->Turn ();

						if (compare_flag) {
							compare_ptr = &compare_turn_array [j];
							turn_ptr = &compare_ptr->at (k);

							sum_bin [j] [VHD+PREV] += turn_ptr->Time () * turn_ptr->Turn ();
							sum_bin [j] [TURNS+PREV] += turn_ptr->Turn ();
						}
					}
				}
			}
		}

		//---- write the data ----
	
		if (Metric_Flag ()) {
			factor = 1.0 / 1000.0;
			units = "Kilometers";
			vmt = "VKT";
			lane_mi = "km)";
		} else {
			factor = 1.0 / MILETOFEET;
			units = "Miles";
			vmt = "VMT";
			lane_mi = "mi)";
		}
		tod.Hours (1);

		for (j=0; j <= num_inc; j++) {
			len = sum_bin [j] [LINKS];
			if (len == 0.0) continue;

			group_sum_file.Put_Field (text_field, header_label);
			group_sum_file.Put_Field (value_field, 0.0);
			if (compare_flag) group_sum_file.Put_Field (compare_field, 0.0);
			group_sum_file.Write ();


			if (j == num_inc) {
				buffer = "Time Period Total";
				len *= num_inc;
			} else {
				buffer = String ("Time Period %12.12s") % sum_periods.Range_Format (j);

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
			group_sum_file.Put_Field (text_field, buffer);
			group_sum_file.Put_Field (value_field, 0.0);
			if (compare_flag) group_sum_file.Put_Field (compare_field, 0.0);
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Number of Links"));
			group_sum_file.Put_Field (value_field, sum_bin [j] [LINKS]);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [LINKS]);
			group_sum_file.Write ();

			buffer = "Number of Roadway " + units;
			group_sum_file.Put_Field (text_field, buffer);
			group_sum_file.Put_Field (value_field, sum_bin [j] [LENGTH] * factor);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [LENGTH] * factor);
			group_sum_file.Write ();

			buffer = "Number of Lane " + units;
			group_sum_file.Put_Field (text_field, buffer);
			group_sum_file.Put_Field (value_field, sum_bin [j] [LANES] * factor);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [LANES] * factor);
			group_sum_file.Write ();

			buffer = "Vehicle " + units + " of Travel";
			group_sum_file.Put_Field (text_field, buffer);
			group_sum_file.Put_Field (value_field, sum_bin [j] [VMT] * factor);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [VMT+PREV] * factor);
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Vehicle Hours of Travel"));
			group_sum_file.Put_Field (value_field, sum_bin [j] [VHT] / tod);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [VHT+PREV] / tod);
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Vehicle Hours of Delay"));
			group_sum_file.Put_Field (value_field, sum_bin [j] [VHD] / tod);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [VHD+PREV] / tod);
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Number of Queued Vehicles"));
			group_sum_file.Put_Field (value_field, UnRound (sum_bin [j] [QUEUE]));
			if (compare_flag) group_sum_file.Put_Field (compare_field, UnRound (sum_bin [j] [QUEUE+PREV]));
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Maximum Queued Vehicles"));
			group_sum_file.Put_Field (value_field, sum_bin [j] [MAX_QUEUE]);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [MAX_QUEUE+PREV]);
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Number of Cycle Failures"));
			group_sum_file.Put_Field (value_field, sum_bin [j] [FAILURE]);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [FAILURE+PREV]);
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Number of Turning Movements"));
			group_sum_file.Put_Field (value_field, sum_bin [j] [TURNS]);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [TURNS+PREV]);
			group_sum_file.Write ();

			group_sum_file.Put_Field (text_field, String ("Average Link Time Ratio"));
			group_sum_file.Put_Field (value_field, sum_bin [j] [TIME_RATIO] / len);
			if (compare_flag) group_sum_file.Put_Field (compare_field, sum_bin [j] [TIME_RATIO+PREV] / len);
			group_sum_file.Write ();

			buffer = "Average Link Density (/ln-" + lane_mi;
			group_sum_file.Put_Field (text_field, buffer);
			group_sum_file.Put_Field (value_field, UnRound (sum_bin [j] [DENSITY] / len));
			if (compare_flag) group_sum_file.Put_Field (compare_field, UnRound (sum_bin [j] [DENSITY+PREV] / len));
			group_sum_file.Write ();

			buffer = "Maximum Link Density (/ln-" + lane_mi;
			group_sum_file.Put_Field (text_field, buffer);
			group_sum_file.Put_Field (value_field, UnRound (sum_bin [j] [MAX_DEN]));
			if (compare_flag) group_sum_file.Put_Field (compare_field, UnRound (sum_bin [j] [MAX_DEN+PREV]));
			group_sum_file.Write ();

			length = sum_bin [j] [VMT] * factor;
			loaded_time = sum_bin [j] [VHT] / tod;
			if (loaded_time == 0.0) {
				loaded_time = length;
			} else {
				loaded_time = length / loaded_time;
			}
			buffer = "Average " + units + " Per Hour";
			group_sum_file.Put_Field (text_field, buffer);
			group_sum_file.Put_Field (value_field, loaded_time);

			if (compare_flag) {
				length = sum_bin [j] [VMT+PREV] * factor;
				loaded_time = sum_bin [j] [VHT+PREV] / tod;
				if (loaded_time == 0.0) {
					loaded_time = length;
				} else {
					loaded_time = length / loaded_time;
				}
				group_sum_file.Put_Field (compare_field, loaded_time);
			}
			group_sum_file.Write ();

			if (Ratio_Flag ()) {
				value = sum_bin [j] [VMT];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [j] [CONG_VMT] / value;

				if (Metric_Flag ()) {
					buffer = "Percent VKT Congested";
				} else {
					buffer = "Percent VMT Congested";
				}
				group_sum_file.Put_Field (text_field, buffer);
				group_sum_file.Put_Field (value_field, percent);
				if (compare_flag) {
					value = sum_bin [j] [VMT+PREV];
					if (value == 0.0) value = 1.0;
					percent = 100.0 * sum_bin [j] [CONG_VMT+PREV] / value;

					group_sum_file.Put_Field (compare_field, percent);
				}
				group_sum_file.Write ();

				value = sum_bin [j] [VHT];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [j] [CONG_VHT] / value;

				group_sum_file.Put_Field (text_field, String ("Percent VHT Congested"));
				group_sum_file.Put_Field (value_field, percent);
				if (compare_flag) {
					value = sum_bin [j] [VHT+PREV];
					if (value == 0.0) value = 1.0;
					percent = 100.0 * sum_bin [j] [CONG_VHT+PREV] / value;

					group_sum_file.Put_Field (compare_field, percent);
				}
				group_sum_file.Write ();

				value = sum_bin [j] [COUNT];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [j] [CONG_TIME] / value;

				group_sum_file.Put_Field (text_field, String ("Percent Time Congested"));
				group_sum_file.Put_Field (value_field, percent);
				if (compare_flag) {
					value = sum_bin [j] [COUNT+PREV];
					if (value == 0.0) value = 1.0;
					percent = 100.0 * sum_bin [j] [CONG_TIME+PREV] / value;

					group_sum_file.Put_Field (compare_field, percent);
				}
				group_sum_file.Write ();
			}
		}
	}
	End_Progress ();

}
