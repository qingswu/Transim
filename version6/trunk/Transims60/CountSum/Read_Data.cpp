//*********************************************************
//	Read_Data.cpp - read data file
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Read_Data
//---------------------------------------------------------

void CountSum::Read_Data (void)
{
	int id, volume, speed, count, index, dir_index, day;
	Dtime time;
	String text;
	Strings date_time;
	bool select_flag, flag;

	Str_Itr str_itr;
	Data_Itr data_itr;
	Int_Map_Itr map_itr;
	ID_Offset_Itr id_itr;
	Offset_Index_Itr offset_itr;
	Vol_Spd_Period_Array *day_array_ptr;
	Vol_Spd_Period *period_ptr;
	Vol_Spd_Data *vol_spd_ptr;
	Strs_Itr days_itr;
	Str_ID_Stat str_id_stat;
	Int2_Key dir_offset;

	//---- read data files ----

	count = 0;

	Show_Message (1);

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
		select_flag = (data_itr->select_days.size () > 0);

		Show_Message (String ("Reading %s -- Record") % data_itr->file->File_Type ());
		Set_Progress ();

		while (data_itr->file->Read_Record ()) {
			Show_Progress ();

			//---- get the signal/station number ----

			id = data_itr->file->Get_Integer (data_itr->id_field);
			if (id == 0) continue;

			if (method_flag) {
				map_itr = detect_dir_map.find (id);
				if (map_itr == detect_dir_map.end ()) continue;
				dir_index = map_itr->second;
				dir_offset.first = dir_index;
				dir_offset.second = 0;
			} else {
				id_itr = id_offset_map.find (id);
				if (id_itr == id_offset_map.end ()) continue;
				dir_index = id_itr->second.first;
				dir_offset = id_itr->second;
			}
			offset_itr = offset_index_map.find (dir_offset);
			if (offset_itr == offset_index_map.end ()) continue;
			index = offset_itr->second;

			if (data_itr->day_time_field >= 0) {
				text = data_itr->file->Get_String (data_itr->day_time_field);
				text.Parse (date_time, " ");
			} else {
				date_time.assign (2, "");

				if (data_itr->day_field >= 0) {
					date_time [0] = data_itr->file->Get_String (data_itr->day_field);
				}
				if (data_itr->time_field >= 0) {
					date_time [1] = data_itr->file->Get_String (data_itr->time_field);
				}
			}

			if (select_flag)  {
				flag = false;
				for (str_itr = data_itr->select_days.begin (); str_itr != data_itr->select_days.end (); str_itr++) {
					if (str_itr->Equals (date_time [0])) {
						flag = true;
						break;
					}
				}
				if (!flag) continue;
			}

			//---- get or create a flow-time-period array for each count station ----

			day = (int) day_map.size ();

			str_id_stat = day_map.insert (Str_ID_Data (date_time [0], day));

			if (!str_id_stat.second) {
				day = str_id_stat.first->second;
			} else {
				day_array_ptr = new Vol_Spd_Period_Array ();
				day_array_ptr->Replicate (count_day_array);
				count_days.push_back (day_array_ptr);
			}
			day_array_ptr = count_days [day];

			//---- find the time period ----

			time = Dtime (date_time [1], data_itr->time_format);

			period_ptr = day_array_ptr->Period_Ptr (time);
			if (period_ptr == 0) continue;

			vol_spd_ptr = period_ptr->Data_Ptr (index);

			volume = data_itr->file->Get_Integer (data_itr->volume_field);
			if (volume == 0) continue;

			speed = data_itr->file->Get_Integer (data_itr->speed_field);

			if (speed > 0) {
				vol_spd_ptr->Add_Vol_Spd (volume * data_itr->factor, speed);
			} else {
				vol_spd_ptr->Add_Volume (volume * data_itr->factor);
			}
			count++;
		}
		End_Progress ();

		data_itr->file->Close ();
	}

	Print (2, "Number of Count Records Processed = ") << count;
}
