//*********************************************************
//	Custom_Summaries.cpp - network performance data file
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
#define PREV		17

//---------------------------------------------------------
//	Custom_Summaries
//---------------------------------------------------------

void LinkSum::Custom_Summaries (void)
{
	int i, j, k, p, num_p, r, num_r, n, index, use_index;
	int text_field, value_field, compare_field, link_count, num_sum;
	double length, value, percent, factor, time, person_fac, lane_len, ratio, hours;
	String buffer, units, vmt, lane_mi, type;
	bool connect_flag, first_ratio;
	Dtime low, high, tod, period, period_low, period_high;

	Link_Itr link_itr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Perf_Data perf_data;
	Turn_Period_Itr turn_itr;
	Turn_Period *compare_ptr;
	Turn_Data *turn_ptr;
	Connect_Data *connect_ptr;
	Doubles_Itr itr;
	Performance_Data data;

	Show_Message (String ("Writing %s -- Record") % summary_file.File_Type ());
	Set_Progress ();

	text_field = summary_file.Field_Number ("MEASURE");
	value_field = summary_file.Field_Number ("VALUE");
	compare_field = summary_file.Field_Number ("COMPARE");

	if (periods_flag) {
		num_p = data_periods.Num_Periods ();
	} else {
		num_p = 1;
	}
	if (ratios_flag) {
		num_r = (int) data_ratios.size ();
	} else {
		num_r = 1;
	}

	//---- clear the summary bins -----

	for (itr = sum_bin.begin (); itr != sum_bin.end (); itr++) {
		itr->assign (NUM_SUM_BINS, 0.0);
	}
	if (compare_flag) {
		connect_flag = System_Data_Flag (CONNECTION) && (turn_period_array.size () > 0) && (compare_turn_array.size () > 0);
	} else {
		connect_flag = System_Data_Flag (CONNECTION) && (turn_period_array.size () > 0);
	}
	
	type = (person_flag) ? "Person" : "Vehicle";
	link_count = 0;

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (select_flag && link_itr->Use () == 0) continue;
		
		length = UnRound (link_itr->Length ());

		for (i=0; i < 2; i++) {
			if (i) {
				if (link_itr->Use () == -1) continue;
				index = link_itr->BA_Dir ();
			} else {
				if (link_itr->Use () == -2) continue;
				index = link_itr->AB_Dir ();
			}
			if (index < 0) continue;
			dir_ptr = &dir_array [index];
			use_index = dir_ptr->Use_Index ();
			link_count++;

			for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {

				perf_period_array.periods->Period_Range (j, low, high);

				data.Start (low);
				data.End (high);

				perf_data = period_itr->Total_Performance (index, use_index);

				if (data.Get_Data (&perf_data, dir_ptr, &(*link_itr), Maximum_Time_Ratio ())) {

					if (person_flag && data.Volume () > 0) {
						person_fac = data.Persons () / data.Volume ();
					} else {
						person_fac = 1.0;
					}
					lane_len = data.Lane_Len ();

					for (p=0; p < num_p; p++) {
						if (periods_flag) {
							data_periods.Period_Range (p, period_low, period_high);
						} else {
							period_low = low;
							period_high = high;
						}
						if (low < period_low || high > period_high) continue;

						for (r=0; r < num_r; r++) {
							if (ratios_flag) {
								ratio = UnRound (data_ratios [r]);
							} else {
								ratio = UnRound (Congested_Ratio ());
							}
							n = (p * num_r + r);

							sum_bin [n] [LINKS] += 1;
							sum_bin [n] [LENGTH] += length;
							sum_bin [n] [LANES] += lane_len;
							sum_bin [n] [VMT] += data.Veh_Dist () * person_fac;
							sum_bin [n] [VHT] += data.Veh_Time () * person_fac;
							sum_bin [n] [VHD] += data.Veh_Delay () * person_fac;
							sum_bin [n] [TIME_RATIO] += data.Time_Ratio () * lane_len;
							sum_bin [n] [DENSITY] += data.Density () * person_fac;
							sum_bin [n] [MAX_DEN] = MAX (sum_bin [n] [MAX_DEN], data.Max_Density () * person_fac);
							sum_bin [n] [QUEUE] += data.Queue () * person_fac;
							sum_bin [n] [MAX_QUEUE] = MAX (sum_bin [n] [MAX_QUEUE], data.Max_Queue () * person_fac);
							sum_bin [n] [FAILURE] += data.Failure () * person_fac;
							sum_bin [n] [COUNT] += lane_len;

							if (data.Time_Ratio () >= ratio) {
								sum_bin [n] [CONG_VMT] += data.Veh_Dist () * person_fac;
								sum_bin [n] [CONG_VHT] += data.Veh_Time () * person_fac;
								sum_bin [n] [CONG_TIME] += lane_len;
							}
						}
					}
					if (compare_flag) {
						period_ptr = &compare_perf_array [j];

						perf_data = period_ptr->Total_Performance (index, use_index);

						if (data.Get_Data (&perf_data, dir_ptr, &(*link_itr), Maximum_Time_Ratio ())) {
					
							if (person_flag && data.Volume () > 0) {
								person_fac = data.Persons () / data.Volume ();
							} else {
								person_fac = 1.0;
							}
							lane_len = data.Lane_Len ();

							for (p=0; p < num_p; p++) {
								if (periods_flag) {
									data_periods.Period_Range (p, period_low, period_high);
								} else {
									period_low = low;
									period_high = high;
								}
								if (low < period_low || high > period_high) continue;

								for (r=0; r < num_r; r++) {
									if (ratios_flag) {
										ratio = UnRound (data_ratios [r]);
									} else {
										ratio = UnRound (Congested_Ratio ());
									}
									n = (p * num_r + r);

									sum_bin [n] [VMT+PREV] += data.Veh_Dist () * person_fac;
									sum_bin [n] [VHT+PREV] += data.Veh_Time () * person_fac;
									sum_bin [n] [VHD+PREV] += data.Veh_Delay () * person_fac;
									sum_bin [n] [TIME_RATIO+PREV] += data.Time_Ratio () * lane_len;
									sum_bin [n] [DENSITY+PREV] += data.Density () * person_fac;
									sum_bin [n] [MAX_DEN+PREV] = MAX (sum_bin [n] [MAX_DEN+PREV], data.Max_Density () * person_fac);
									sum_bin [n] [QUEUE+PREV] += data.Queue () * person_fac;
									sum_bin [n] [MAX_QUEUE+PREV] = MAX (sum_bin [n] [MAX_QUEUE+PREV], data.Max_Queue () * person_fac);
									sum_bin [n] [FAILURE+PREV] += data.Failure () * person_fac;
									sum_bin [n] [COUNT+PREV] += lane_len;

									if (data.Time_Ratio () >= ratio) {
										sum_bin [n] [CONG_VMT+PREV] += data.Veh_Dist () * person_fac;
										sum_bin [n] [CONG_VHT+PREV] += data.Veh_Time () * person_fac;
										sum_bin [n] [CONG_TIME+PREV] += lane_len;
									}
								}
							}
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
						turn_ptr = turn_itr->Data_Ptr (k);
						
						turn_period_array.periods->Period_Range (j, low, high);

						for (p=0; p < num_p; p++) {
							if (periods_flag) {
								data_periods.Period_Range (p, period_low, period_high);
							} else {
								period_low = low;
								period_high = high;
							}
							if (low >= period_low && high <= period_high) {
								n = (p * num_r);
								sum_bin [n] [VHD] += turn_ptr->Time () * turn_ptr->Turn ();
								sum_bin [n] [TURNS] += turn_ptr->Turn ();
							}
						}
						if (compare_flag) {
							compare_ptr = &compare_turn_array [j];
							turn_ptr = compare_ptr->Data_Ptr (k);

							for (p=0; p < num_p; p++) {
								if (periods_flag) {
									data_periods.Period_Range (p, period_low, period_high);
								} else {
									period_low = low;
									period_high = high;
								}
								if (low >= period_low && high <= period_high) {
									n = (p * num_r);
									sum_bin [n] [VHD+PREV] += turn_ptr->Time () * turn_ptr->Turn ();
									sum_bin [n] [TURNS+PREV] += turn_ptr->Turn ();
								}
							}
						}
					}
				}
			}
		}
	}
	End_Progress ();

	//---- write the data ----
	
	if (Metric_Flag ()) {
		factor = 1.0 / 1000.0;
		units = "Kilometers";
		vmt = (person_flag) ? "PKT" : "VKT";
		lane_mi = "km)";
	} else {
		factor = 1.0 / MILETOFEET;
		units = "Miles";
		vmt = (person_flag) ? "PMT" : "VMT";
		lane_mi = "mi)";
	}
	tod.Hours (1);
	if (link_count == 0) link_count = 1;

	for (p=0; p < num_p; p++) {
		if (periods_flag) {
			data_periods.Period_Range (p, period_low, period_high);
		} else {
			low = period_low;
			high = period_high;
		}
		first_ratio = true;
		period = period_high - period_low;
		hours = (double) period / (double) tod;

		buffer = String ("Time Period %12.12s") % data_periods.Range_Format (p);

		for (r=0; r < num_r; r++, first_ratio = false) {
			if (ratios_flag) {
				ratio = UnRound (data_ratios [r]);
			} else {
				ratio = UnRound (Congested_Ratio ());
			}
			n = (p * num_r + r);

			lane_len = sum_bin [n] [LANES];

			num_sum = (int) (sum_bin [n] [LINKS] / link_count);

			if (first_ratio) {
				summary_file.Put_Field (text_field, buffer);
				summary_file.Put_Field (value_field, 0.0);
				if (compare_flag) summary_file.Put_Field (compare_field, 0.0);
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("Number of Links"));
				summary_file.Put_Field (value_field, sum_bin [n] [LINKS] / num_sum);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [LINKS] / num_sum);
				summary_file.Write ();

				buffer = "Number of Roadway " + units;
				summary_file.Put_Field (text_field, buffer);
				summary_file.Put_Field (value_field, sum_bin [n] [LENGTH] * factor / num_sum);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [LENGTH] * factor / num_sum);
				summary_file.Write ();

				buffer = "Number of Lane " + units;
				summary_file.Put_Field (text_field, buffer);
				summary_file.Put_Field (value_field, sum_bin [n] [LANES] * factor / num_sum);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [LANES] * factor / num_sum);
				summary_file.Write ();

				buffer = type + " " + units + " of Travel";
				summary_file.Put_Field (text_field, buffer);
				summary_file.Put_Field (value_field, sum_bin [n] [VMT] * factor);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [VMT+PREV] * factor);
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("%s Hours of Travel") % type);
				summary_file.Put_Field (value_field, sum_bin [n] [VHT] / tod);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [VHT+PREV] / tod);
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("%s Hours of Delay") % type);
				summary_file.Put_Field (value_field, sum_bin [n] [VHD] / tod);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [VHD+PREV] / tod);
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("Number of Queued %ss") % type);
				summary_file.Put_Field (value_field, UnRound (sum_bin [n] [QUEUE]));
				if (compare_flag) summary_file.Put_Field (compare_field, UnRound (sum_bin [n] [QUEUE+PREV]));
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("Maximum Queued %ss") % type);
				summary_file.Put_Field (value_field, sum_bin [n] [MAX_QUEUE]);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [MAX_QUEUE+PREV]);
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("Number of Cycle Failures"));
				summary_file.Put_Field (value_field, sum_bin [n] [FAILURE]);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [FAILURE+PREV]);
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("Number of Turning Movements"));
				summary_file.Put_Field (value_field, sum_bin [n] [TURNS]);
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [TURNS+PREV]);
				summary_file.Write ();

				summary_file.Put_Field (text_field, String ("Average Link Time Ratio"));
				summary_file.Put_Field (value_field, sum_bin [n] [TIME_RATIO] / (lane_len * 100.0));
				if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [TIME_RATIO+PREV] / (lane_len * 100.0));
				summary_file.Write ();

				value = sum_bin [n] [LINKS];

				buffer = "Average Link Density (/ln-" + lane_mi;
				summary_file.Put_Field (text_field, buffer);
				summary_file.Put_Field (value_field, UnRound (sum_bin [n] [DENSITY] / value));
				if (compare_flag) summary_file.Put_Field (compare_field, UnRound (sum_bin [n] [DENSITY+PREV] / value));
				summary_file.Write ();

				buffer = "Maximum Link Density (/ln-" + lane_mi;
				summary_file.Put_Field (text_field, buffer);
				summary_file.Put_Field (value_field, UnRound (sum_bin [n] [MAX_DEN]));
				if (compare_flag) summary_file.Put_Field (compare_field, UnRound (sum_bin [n] [MAX_DEN+PREV]));
				summary_file.Write ();

				length = sum_bin [n] [VMT] * factor;
				time = sum_bin [n] [VHT] / tod;
				if (time == 0.0) {
					time = length;
				} else {
					time = length / time;
				}
				buffer = "Average " + units + " Per Hour";
				summary_file.Put_Field (text_field, buffer);
				summary_file.Put_Field (value_field, time);

				if (compare_flag) {
					length = sum_bin [n] [VMT+PREV] * factor;
					time = sum_bin [n] [VHT+PREV] / tod;
					if (time == 0.0) {
						time = length;
					} else {
						time = length / time;
					}
					summary_file.Put_Field (compare_field, time);
				}
				summary_file.Write ();
			}
			summary_file.Put_Field (text_field, String ("Congested Ratio=%.2lf") % (ratio / 100.0));
			summary_file.Put_Field (value_field, 0.0);
			if (compare_flag) summary_file.Put_Field (compare_field, 0.0);
			summary_file.Write ();

			buffer = "Congested " + type + " " + units;
			summary_file.Put_Field (text_field, buffer);
			summary_file.Put_Field (value_field, sum_bin [n] [CONG_VMT] * factor);
			if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [CONG_VMT+PREV] * factor);
			summary_file.Write ();

			value = sum_bin [n] [VMT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [n] [CONG_VMT] / value;

			buffer = String ("Percent %s Congested") % vmt;
			summary_file.Put_Field (text_field, buffer);
			summary_file.Put_Field (value_field, percent);
			if (compare_flag) {
				value = sum_bin [n] [VMT+PREV];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [n] [CONG_VMT+PREV] / value;

				summary_file.Put_Field (compare_field, percent);
			}
			summary_file.Write ();

			buffer = "Congested " + type + " Hours";
			summary_file.Put_Field (text_field, buffer);
			summary_file.Put_Field (value_field, sum_bin [n] [CONG_VHT] / tod);
			if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [CONG_VHT+PREV] / tod);
			summary_file.Write ();

			value = sum_bin [n] [VHT];
			if (value == 0.0) value = 1.0;
			percent = 100.0 * sum_bin [n] [CONG_VHT] / value;

			buffer = String ("Percent %sHT Congested") % ((person_flag ? "P" : "V"));
			summary_file.Put_Field (text_field, buffer);
			summary_file.Put_Field (value_field, percent);
			if (compare_flag) {
				value = sum_bin [n] [VHT+PREV];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [n] [CONG_VHT+PREV] / value;

				summary_file.Put_Field (compare_field, percent);
			}
			summary_file.Write ();

			value = factor * hours / num_sum;

			buffer = "Congested Duration (hrs*ln-" + lane_mi;
			summary_file.Put_Field (text_field, buffer);
			summary_file.Put_Field (value_field, sum_bin [n] [CONG_TIME] * value);
			if (compare_flag) summary_file.Put_Field (compare_field, sum_bin [n] [CONG_TIME+PREV] * value);
			summary_file.Write ();

			value = sum_bin [n] [COUNT];
			if (value == 0.0) value = 1.0;

			percent = 100.0 * sum_bin [n] [CONG_TIME] / value;

			summary_file.Put_Field (text_field, String ("Percent Time Congested"));
			summary_file.Put_Field (value_field, percent);
			if (compare_flag) {
				value = sum_bin [n] [COUNT+PREV];
				if (value == 0.0) value = 1.0;
				percent = 100.0 * sum_bin [n] [CONG_TIME+PREV] / value;

				summary_file.Put_Field (compare_field, percent);
			}
			summary_file.Write ();
		}
	}
}
