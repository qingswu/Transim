//*********************************************************
//	Write_Delay.cpp - write the arcview delay file
//*********************************************************

#include "ArcDelay.hpp"

//---------------------------------------------------------
//	Write_Delay
//---------------------------------------------------------

void ArcDelay::Write_Delay (void)
{
	int dir, period, index, flow_index, num;
	double speed, length, vc_ratio, cap_fac;
	Dtime low, high, time, delay;
	String buffer;

	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Link_Perf_Period_Itr period_itr;
	Link_Perf_Data perf_data;

	//---- process each time period ----
		
	Print (1);

	for (period=0, period_itr = link_perf_array.begin (); period_itr != link_perf_array.end (); period_itr++, period++) {

		sum_periods.Period_Range (period, low, high);

		cap_fac = (high - low + 1) / Dtime (1.0, HOURS);

		//---- create a new shape file ----

		buffer = String ("%s_%s_%s.shp") % shapename % low.Time_Label (true) % high.Time_Label (true);

		arcview_delay.First_Open (true);

		if (!arcview_delay.Open (buffer)) {
			File_Error ("Opening ArcView Delay File", arcview_delay.Shape_Filename ());
		}
		arcview_delay.Write_Header ();

		Show_Message (String ("Writing %s %s -- Link") % arcview_delay.File_Type () % high.Time_String ());
		Set_Progress ();

		delay_file->Start (low);
		delay_file->End (high);

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

				if (!Lane_Use_Flows ()) {
					perf_data = period_itr->Total_Link_Perf (index, flow_index);
				} else {
					perf_data = period_itr->at (index);
				}
flow_link:				
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

				delay = time - dir_ptr->Time0 ();
				delay_file->Delay (delay);

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
					arcview_delay.Put_Field (length_field, UnRound (link_ptr->Length ()));
					arcview_delay.Put_Field (lane_len_field, UnRound (link_ptr->Length () * dir_ptr->Lanes ()));

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
					arcview_delay.Put_Field (vhd_field, delay * perf_data.Flow ());
					arcview_delay.Put_Field (cong_time_field, 100.0 * perf_data.Ratio_Count () / num);
					arcview_delay.Put_Field (cong_vmt_field, perf_data.Ratio_VMT ());
					arcview_delay.Put_Field (cong_vht_field, perf_data.Ratio_VHT ());
				}
				if (index_flag) {
					arcview_delay.Put_Field (1, link_itr->Link () * 2 + dir);
				}

				Draw_Link (arcview_delay, link_ptr, dir);

				if (flow_index > 0 && Lane_Use_Flows ()) {
					delay_file->Type (1);
					perf_data = period_itr->at (flow_index);
					goto flow_link;
				}
			}
		}
		End_Progress ();
		arcview_delay.Close ();
	}
}