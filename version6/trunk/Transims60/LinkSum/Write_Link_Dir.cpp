//*********************************************************
//	Write_Link_Dir.cpp - Write the Link Direction File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Link_Dir
//---------------------------------------------------------

void LinkSum::Write_Link_Dir (Dir_Group_Itr &group)
{
	int i, j, nrec, index, use_index, type;
	bool save, type_flag;	
	double value;
	Dtime low, high;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int_Map_Itr map_itr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr;
	Perf_Data perf_data;
	Performance_Data data;
	
	nrec = 0;
	value = 0.0;

	Show_Message (String ("Writing %s -- Record") % group->file->File_Type ());
	Set_Progress ();

	//---- process each link in sorted order ----

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		if (select_flag && link_ptr->Use () == 0) continue;

		group->file->Zero_Fields ();
		type_flag = group->file->Lane_Use_Flows ();
		
		//---- flow types ----

		for (type=0; type < 2; type++) {
			if (!type_flag && type > 0) break;

			//---- process each direction ----

			for (i=0; i < 2; i++) {

				if (i) {
					if (link_ptr->Use () == -1) continue;
					index = link_ptr->BA_Dir ();
				} else {
					if (link_ptr->Use () == -2) continue;
					index = link_ptr->AB_Dir ();
				}
				if (index < 0) continue;
				dir_ptr = &dir_array [index];

				use_index = dir_ptr->Use_Index ();
				if (type == 1 && use_index < 0) continue;

				save = false;

				//---- process each time period ----

				for (j=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, j++) {
					perf_period_array.periods->Period_Range (j, low, high);

					data.Start (low);
					data.End (high);

					if (type_flag) {
						if (type == 0) {
							perf_data = period_itr->at (index);
						} else {
							data.Type (1);
							perf_data = period_itr->at (use_index);
						}
					} else {
						perf_data = period_itr->Total_Performance (index, use_index);
					}
					value = 0;

					if (data.Get_Data (&perf_data, dir_ptr, link_ptr, Maximum_Time_Ratio ())) {

						switch (group->field) {
							case TTIME_DATA:
								value = data.Time ().Seconds ();
								break;
							case PERSON_DATA:
								value = data.Persons ();
								break;
							case VOLUME_DATA:
								value = data.Volume ();
								break;
							case ENTER_DATA:
								value = data.Enter ();
								break;
							case EXIT_DATA:
								value = data.Exit ();
								break;
							case FLOW_DATA:
								value = data.Flow ();
								break;
							case SPEED_DATA:
								value = data.Speed ();
								break;
							case RATIO_DATA:
								value = data.Time_Ratio ();
								break;
							case DELAY_DATA:
								value = data.Delay ();
								break;
							case DENSITY_DATA:
								value = data.Density ();
								break;
							case MAX_DENSITY_DATA:
								value = data.Max_Density ();
								break;
							case QUEUE_DATA:
								value = data.Queue ();
								break;
							case MAX_QUEUE_DATA:
								value = data.Max_Queue ();
								break;
							case FAILURE_DATA:
								value = data.Failure ();
								break;
							case VC_DATA:
								value = data.VC_Ratio ();
								break;
							case VMT_DATA:
							case PMT_DATA:
								value = data.Veh_Dist ();
								break;
							case VHT_DATA:
							case PHT_DATA:
								value = data.Veh_Time ();
								break;
							case VHD_DATA:
							case PHD_DATA:
								value = data.Veh_Delay ();
								break;
							case CONG_VMT_DATA:
							case CONG_PMT_DATA:
								value = data.Ratio_Dist ();
								break;
							case CONG_VHT_DATA:
							case CONG_PHT_DATA:
								value = data.Ratio_Time ();
								break;
							case CONG_TIME_DATA:
								value = data.Count ();
								if (value <= 0.0) value = 1.0;
								value = 100.0 * data.Ratios () / value;
								break;
							default:
								value = 0.0;
								break;
						}
						if (value != 0.0) {
							if (data.Volume () > 0 && 
								(group->field == PMT_DATA || group->field == PHT_DATA || group->field == PHD_DATA ||
								group->field == CONG_PMT_DATA || group->field == CONG_PHT_DATA)) {
								value *= data.Persons () / data.Volume ();
							}
							save = true;
						}
					}
					if (group->flip) {
						group->file->Data2 (j, value);
					} else {
						group->file->Data (j, value);
					}

					//---- add the previous value ----

					if (compare_flag) {
						period_ptr = &compare_perf_array [j];

						if (type_flag) {
							if (type == 0) {
								perf_data = period_ptr->at (index);
							} else {
								perf_data = period_ptr->at (use_index);

							}
						} else {
							perf_data = period_ptr->Total_Performance (index, use_index);
						}
						value = 0;

						if (data.Get_Data (&perf_data, dir_ptr, link_ptr, Maximum_Time_Ratio ())) {

							switch (group->field) {
								case TTIME_DATA:
									value = data.Time ().Seconds ();
									break;
								case PERSON_DATA:
									value = data.Persons ();
									break;
								case VOLUME_DATA:
									value = data.Volume ();
									break;
								case ENTER_DATA:
									value = data.Enter ();
									break;
								case EXIT_DATA:
									value = data.Exit ();
									break;
								case FLOW_DATA:
									value = data.Flow ();
									break;
								case SPEED_DATA:
									value = data.Speed ();
									break;
								case RATIO_DATA:
									value = data.Time_Ratio ();
									break;
								case DELAY_DATA:
									value = data.Delay ();
									break;
								case DENSITY_DATA:
									value = data.Density ();
									break;
								case MAX_DENSITY_DATA:
									value = data.Max_Density ();
									break;
								case QUEUE_DATA:
									value = data.Queue ();
									break;
								case MAX_QUEUE_DATA:
									value = data.Max_Queue ();
									break;
								case FAILURE_DATA:
									value = data.Failure ();
									break;
								case VC_DATA:
									value = data.VC_Ratio ();
									break;
								case VMT_DATA:
								case PMT_DATA:
									value = data.Veh_Dist ();
									break;
								case VHT_DATA:
								case PHT_DATA:
									value = data.Veh_Time ();
									break;
								case VHD_DATA:
								case PHD_DATA:
									value = data.Veh_Delay ();
									break;
								case CONG_VMT_DATA:
								case CONG_PMT_DATA:
									value = data.Ratio_Dist ();
									break;
								case CONG_VHT_DATA:
								case CONG_PHT_DATA:
									value = data.Ratio_Time ();
									break;
								case CONG_TIME_DATA:
									value = data.Count ();
									if (value <= 0.0) value = 1.0;
									value = 100.0 * data.Ratios () / value;
									break;
								default:
									value = 0.0;
									break;
							}
							if (value != 0.0) {
								if (data.Volume () > 0 && 
									(group->field == PMT_DATA || group->field == PHT_DATA || group->field == PHD_DATA ||
									group->field == CONG_PMT_DATA || group->field == CONG_PHT_DATA)) {
									value *= data.Persons () / data.Volume ();
								}
								save = true;
							}
						}
						if (group->flip) {
							group->file->Data (j, value);
						} else {
							group->file->Data2 (j, value);
						}
					}
				}

				//---- save non-zero records ----

				if (save || minimum_volume == 0.0) {
					nrec++;

					group->file->Link_Dir (link_ptr->Link () * 2 + i);
					group->file->Link (link_ptr->Link ());
					group->file->Dir (i);
					group->file->Type (type);

					if (compare_flag) {
						group->file->Difference ();
					}
					if (!group->file->Write ()) {
						Error (String ("Writing %s") % group->file->File_Type ());
					}
				}
			}
		}
	}
	End_Progress ();

	Print (2, String ("Number of %s Records = %d") % group->file->File_Type () % nrec);
	Print (1, "Number of Time Periods = ") << group->file->Num_Periods ();

	group->file->Close ();
}
