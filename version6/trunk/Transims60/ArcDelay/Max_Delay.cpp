//*********************************************************
//	Max_Delay.cpp - write the arcview delay file
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Maximum_Delay
//---------------------------------------------------------

void ArcDelay::Maximum_Delay (void)
{
	int dir, period, index, flow_index, num, max_period;
	double speed, length, vc_ratio, cap_fac, value, max_value;
	Dtime low, high, time;
	String buffer;
	bool flow_flag;

	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Link_Perf_Data perf_data, best_data;

	Show_Message (String ("Writing %s Maximum -- Link") % arcview_delay.File_Type ());
	Set_Progress ();

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Use () == 0) continue;
		link_ptr = &(*link_itr);

		delay_file->Link (link_itr->Link ());

		for (dir=0; dir < 2; dir++) {
			if (dir) {
				index = link_itr->BA_Dir ();
			} else {
				index = link_itr->AB_Dir ();
			}
			if (index < 0) continue;

			delay_file->Dir (dir);

			length = link_itr->Length ();

			dir_ptr = &dir_array [index];				
			flow_index = dir_ptr->Flow_Index ();
				
			delay_file->Type (0);
			flow_flag = true;

flow_link:
			max_value = -1;
			max_period = -1;
			best_data.Clear ();

			//---- find the maximum period ----

			for (period=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, period++) {

				sum_periods.Period_Range (period, low, high);

				cap_fac = (high - low + 1) / Dtime (1.0, HOURS);

				if (!Lane_Use_Flows () && flow_flag) {
					perf_data = period_itr->Total_Link_Perf (index, flow_index);
				} else {
					perf_data = period_itr->at (index);
				}
				delay_file->Flow (perf_data.Flow ());

				if (perf_data.Time () > 0) {
					speed = length / perf_data.Time ();
				} else {
					speed = length / dir_ptr->Time0 ();
				}
				if (speed < 0.1) speed = 0.1;
				delay_file->Speed (speed);

				time = (int) (length / speed + 0.5);
				if (time < 1) time = 1;

				delay_file->Time (time);

				delay_file->Delay (time - dir_ptr->Time0 ());

				if (dir_ptr->Time0 () > 0) {
					delay_file->Time_Ratio (time * 100.0 / dir_ptr->Time0 ());
				} else {
					delay_file->Time_Ratio (100);
				}

				num = perf_data.Occupancy ();
				if (num < 1 || Ratio_Flag ()) num = 1;

				delay_file->Density (UnRound ((perf_data.Density () + (num / 2)) / num));
				delay_file->Max_Density (UnRound (perf_data.Max_Density ()));

				delay_file->Queue (UnRound ((perf_data.Queue () + (num / 2)) / num));

				delay_file->Max_Queue (perf_data.Max_Queue ());
				delay_file->Cycle_Failure (perf_data.Failure ());

				delay_file->Num_Nest (0);

				arcview_delay.Copy_Fields (*delay_file);

				if (vmt_field >= 0) {
					if (dir_ptr->Capacity () > 0) {
						vc_ratio = 100.0 * perf_data.Flow () / (dir_ptr->Capacity () * cap_fac);
					} else {
						vc_ratio = 0.0;
					}
					num = perf_data.Occupancy ();
					if (num < 1) num = 1;

					arcview_delay.Put_Field (vc_field, vc_ratio);
					arcview_delay.Put_Field (vmt_field, perf_data.Flow () * link_ptr->Length ());
					arcview_delay.Put_Field (vht_field, perf_data.Time () * perf_data.Flow ());
					arcview_delay.Put_Field (cong_time_field, 100.0 * perf_data.Ratio_Count () / num);
					arcview_delay.Put_Field (cong_vmt_field, perf_data.Ratio_VMT ());
					arcview_delay.Put_Field (cong_vht_field, perf_data.Ratio_VHT ());
				}
				if (index_flag) {
					arcview_delay.Put_Field (1, link_itr->Link () * 2 + dir);
				}

				value = arcview_delay.Get_Double (max_period_field);

				if (value > max_value) {
					max_value = value;
					max_period = period;
					best_data = perf_data;
				}
			}

			if (max_period >= 0) {
				sum_periods.Period_Range (max_period, low, high);

				delay_file->Start (low);
				delay_file->End (high);

				cap_fac = (high - low + 1) / Dtime (1.0, HOURS);

				perf_data = best_data;

				delay_file->Flow (perf_data.Flow ());

				if (perf_data.Time () > 0) {
					speed = length / perf_data.Time ();
				} else {
					speed = length / dir_ptr->Time0 ();
				}
				if (speed < 0.1) speed = 0.1;
				delay_file->Speed (speed);

				time = (int) (length / speed + 0.5);
				if (time < 1) time = 1;

				delay_file->Time (time);

				delay_file->Delay (time - dir_ptr->Time0 ());

				if (dir_ptr->Time0 () > 0) {
					delay_file->Time_Ratio (time * 100.0 / dir_ptr->Time0 ());
				} else {
					delay_file->Time_Ratio (100);
				}

				num = perf_data.Occupancy ();
				if (num < 1 || Ratio_Flag ()) num = 1;

				delay_file->Density (UnRound ((perf_data.Density () + (num / 2)) / num));
				delay_file->Max_Density (UnRound (perf_data.Max_Density ()));

				delay_file->Queue (UnRound ((perf_data.Queue () + (num / 2)) / num));

				delay_file->Max_Queue (perf_data.Max_Queue ());
				delay_file->Cycle_Failure (perf_data.Failure ());

				delay_file->Num_Nest (0);

				arcview_delay.Copy_Fields (*delay_file);

				if (vmt_field >= 0) {
					if (dir_ptr->Capacity () > 0) {
						vc_ratio = 100.0 * perf_data.Flow () / (dir_ptr->Capacity () * cap_fac);
					} else {
						vc_ratio = 0.0;
					}
					num = perf_data.Occupancy ();
					if (num < 1) num = 1;

					arcview_delay.Put_Field (vc_field, vc_ratio);
					arcview_delay.Put_Field (vmt_field, perf_data.Flow () * link_ptr->Length ());
					arcview_delay.Put_Field (vht_field, perf_data.Time () * perf_data.Flow ());
					arcview_delay.Put_Field (cong_time_field, 100.0 * perf_data.Ratio_Count () / num);
					arcview_delay.Put_Field (cong_vmt_field, perf_data.Ratio_VMT ());
					arcview_delay.Put_Field (cong_vht_field, perf_data.Ratio_VHT ());
				}
				if (index_flag) {
					arcview_delay.Put_Field (1, link_itr->Link () * 2 + dir);
				}
				Draw_Link (arcview_delay, link_ptr, dir);
			}

			if (flow_index > 0 && Lane_Use_Flows ()) {
				delay_file->Type (1);
				flow_flag = false;
				index = flow_index;
				goto flow_link;
			}
		}
	}
	End_Progress ();
	arcview_delay.Close ();
}
