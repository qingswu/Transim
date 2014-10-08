//*********************************************************
//	Max_Performance.cpp - write the arcview performance file
//*********************************************************

#include "ArcPerf.hpp"

//---------------------------------------------------------
//	Maximum_Performance
//---------------------------------------------------------

void ArcPerf::Maximum_Performance (void)
{
	int dir, period, index, use_index, num, max_period;
	double length, value, max_value;
	Dtime low, high, time;
	String buffer;
	bool flow_flag;

	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Data perf_data, best_data;
	Performance_Data data;

	Show_Message (String ("Writing %s Maximum -- Link") % arcview_perf.File_Type ());
	Set_Progress ();
	
	num = (int) perf_period_array.size ();

	//---- process each link ----

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (link_itr->Use () == 0) continue;
		link_ptr = &(*link_itr);

		perf_file->Link (link_itr->Link ());

		for (dir=0; dir < 2; dir++) {
			if (dir) {
				index = link_itr->BA_Dir ();
			} else {
				index = link_itr->AB_Dir ();
			}
			if (index < 0) continue;

			perf_file->Dir (dir);

			length = link_itr->Length ();

			dir_ptr = &dir_array [index];				
			use_index = dir_ptr->Use_Index ();
				
			perf_file->Type (0);
			flow_flag = true;

use_link:
			max_value = -1;
			max_period = -1;
			best_data.Clear ();

			//---- find the maximum period ----

			for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {

				sum_periods.Period_Range (period, low, high);

				data.Start (low);
				data.End (high);

				if (!Lane_Use_Flows () && flow_flag) {
					perf_data = period_itr->Total_Performance (index, use_index);
				} else {
					perf_data = period_itr->at (index);
				}
				data.Get_Data (&perf_data, dir_ptr, link_ptr);

				perf_file->Time (data.Time ());
				perf_file->Persons (data.Persons ());
				perf_file->Volume (data.Volume ());
				perf_file->Enter (data.Enter ());
				perf_file->Exit (data.Exit ());
				perf_file->Flow (data.Flow ());
				perf_file->Speed (data.Speed ());
				perf_file->Time_Ratio (data.Time_Ratio ());
				perf_file->Delay (data.Delay ());
				perf_file->Density (data.Density ());
				perf_file->Max_Density (data.Max_Density ());
				perf_file->Queue (data.Queue ());
				perf_file->Max_Queue (data.Max_Queue ());
				perf_file->Failure (data.Failure ());
				perf_file->Veh_Dist (data.Veh_Dist ());
				perf_file->Veh_Time (data.Veh_Time ());
				perf_file->Veh_Delay (data.Veh_Delay ());

				arcview_perf.Copy_Fields (*perf_file);

				arcview_perf.Put_Field (length_field, UnRound (link_ptr->Length ()));
				arcview_perf.Put_Field (lane_len_field, data.Lane_Len ());
				arcview_perf.Put_Field (vc_field, data.VC_Ratio ());
				arcview_perf.Put_Field (cong_time_field, 100.0 * data.Ratios () / num);
				arcview_perf.Put_Field (cong_vmt_field, data.Ratio_Dist ());
				arcview_perf.Put_Field (cong_vht_field, data.Ratio_Time ());

				if (index_flag) {
					arcview_perf.Put_Field (1, link_itr->Link () * 2 + dir);
				}
				value = arcview_perf.Get_Double (max_period_field);

				if (value > max_value) {
					max_value = value;
					max_period = period;
					best_data = perf_data;
				}
			}

			if (max_period >= 0) {
				sum_periods.Period_Range (max_period, low, high);

				perf_file->Start (low);
				perf_file->End (high);
		
				data.Start (low);
				data.End (high);

				data.Get_Data (&best_data, dir_ptr, link_ptr);

				perf_file->Time (data.Time ());
				perf_file->Persons (data.Persons ());
				perf_file->Volume (data.Volume ());
				perf_file->Enter (data.Enter ());
				perf_file->Exit (data.Exit ());
				perf_file->Flow (data.Flow ());
				perf_file->Speed (data.Speed ());
				perf_file->Time_Ratio (data.Time_Ratio ());
				perf_file->Delay (data.Delay ());
				perf_file->Density (data.Density ());
				perf_file->Max_Density (data.Max_Density ());
				perf_file->Queue (data.Queue ());
				perf_file->Max_Queue (data.Max_Queue ());
				perf_file->Failure (data.Failure ());
				perf_file->Veh_Dist (data.Veh_Dist ());
				perf_file->Veh_Time (data.Veh_Time ());
				perf_file->Veh_Delay (data.Veh_Delay ());

				arcview_perf.Copy_Fields (*perf_file);

				arcview_perf.Put_Field (length_field, UnRound (length));
				arcview_perf.Put_Field (lane_len_field, data.Lane_Len ());
				arcview_perf.Put_Field (vc_field, data.VC_Ratio ());
				arcview_perf.Put_Field (cong_time_field, 100.0 * data.Ratios () / num);
				arcview_perf.Put_Field (cong_vmt_field, data.Ratio_Dist ());
				arcview_perf.Put_Field (cong_vht_field, data.Ratio_Time ());

				if (index_flag) {
					arcview_perf.Put_Field (1, link_itr->Link () * 2 + dir);
				}
				Draw_Link (arcview_perf, link_ptr, dir);
			}

			if (use_index > 0 && Lane_Use_Flows ()) {
				perf_file->Type (1);
				flow_flag = false;
				index = use_index;
				goto use_link;
			}
		}
	}
	End_Progress ();
	arcview_perf.Close ();
}
