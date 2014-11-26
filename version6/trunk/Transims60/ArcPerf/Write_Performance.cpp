//*********************************************************
//	Write_Performance.cpp - write the arcview performance file
//*********************************************************

#include "ArcPerf.hpp"

//---------------------------------------------------------
//	Write_Performance
//---------------------------------------------------------

void ArcPerf::Write_Performance (void)
{
	int dir, period, index, use_index, num;
	double length;
	Dtime low, high, time, delay;
	String buffer;

	Link_Itr link_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Perf_Period_Itr period_itr;
	Perf_Data perf_data;
	Performance_Data data;

	//---- process each time period ----
		
	Print (1);
	num = (int) perf_period_array.size ();

	for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {

		sum_periods.Period_Range (period, low, high);

		//---- create a new shape file ----

		buffer = String ("%s_%s_%s.shp") % shapename % low.Time_Label (true) % high.Time_Label (true);

		arcview_perf.First_Open (true);

		if (!arcview_perf.Open (buffer)) {
			File_Error ("Opening ArcView Delay File", arcview_perf.Shape_Filename ());
		}
		arcview_perf.Write_Header ();

		Show_Message (String ("Writing %s %s -- Link") % arcview_perf.File_Type () % high.Time_String ());
		Set_Progress ();

		data.Start (low);
		data.End (high);

		perf_file->Start (low);
		perf_file->End (high);

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

				if (!Lane_Use_Flows ()) {
					perf_data = period_itr->Total_Performance (index, use_index);
				} else {
					perf_data = period_itr->at (index);
				}
use_link:		
				if (data.Get_Data (&perf_data, dir_ptr, link_ptr, Maximum_Time_Ratio (), Delete_Time_Ratio ())) {

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
					perf_data = period_itr->at (use_index);
					goto use_link;
				}
			}
		}
		End_Progress ();
		arcview_perf.Close ();
	}
}
