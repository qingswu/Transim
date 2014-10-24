//*********************************************************
//	Combine_Data.cpp - Combine Flow and Time Data
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Combine_Data
//---------------------------------------------------------

void CountSum::Combine_Data (Processing_Methods method, int day_num)
{
	int period, index, day, count, num;
	double volume, speed, total;

	Count_Days_Itr count_itr; 
	Vol_Spd_Period_Itr period_itr;
	Vol_Spd_Period *period_ptr, *vol_spd_period_ptr;
	Vol_Spd_Data *vol_spd_ptr, *count_ptr, *result_ptr;

	Offset_Index_Itr offset_itr, index_itr;
	Int2_Key dir_offset;

	if (method == MINIMUM) {
		Show_Message ("Minimum Volume Data -- Record");
	} else if (method == MAXIMUM) {
		Show_Message ("Maximum Volume Data -- Record");
	} else if (day_num < 0) {
		Show_Message ("Combining Volume Data -- Record");
	} else {
		Show_Message (String ("Volume Data for Day %s -- Record") % (day_num + 1));
	}
	Set_Progress ();

	count_day_array.Zero_Data ();
	vol_spd_array.Zero_Data ();

	//---- process counts for each count day by time period ----

	for (period=0, period_itr = count_day_array.begin (); period_itr != count_day_array.end (); period_itr++, period++) {

		index = -1;
		vol_spd_period_ptr = vol_spd_array.Period_Ptr (period);

		for (offset_itr = offset_index_map.begin (); offset_itr != offset_index_map.end (); offset_itr++) {
			dir_offset = offset_itr->first;

			if (dir_offset.first == index) continue;
			
			index = dir_offset.first;

			result_ptr = period_itr->Data_Ptr (offset_itr->second);
			count = 0;

			for (day=0, count_itr = count_days.begin (); count_itr != count_days.end (); count_itr++, day++) {
				Show_Progress ();

				if (day_num >= 0 && day != day_num) continue;
				
				period_ptr = (*count_itr)->Period_Ptr (period);

				volume = speed = 0;
				num = 0;

				for (index_itr = offset_itr; index_itr != offset_index_map.end (); index_itr++) {
					dir_offset = index_itr->first;

					if (dir_offset.first != index) break;

					count_ptr = period_ptr->Data_Ptr (index_itr->second);

					if (count_ptr->Volume () > 0) {
						if (volume > 0) {
							total = volume + count_ptr->Volume ();
							speed = (speed * volume + count_ptr->Speed () * count_ptr->Volume ()) / total; 
							volume = total;
						} else {
							volume = count_ptr->Volume ();
							speed = count_ptr->Speed ();
						}
						num++;
					}
				}
				if (num == 0) continue;

				if (num > 1) {
					volume /= num;
				}

				if (method == AVERAGE) {
					if (result_ptr->Volume () > 0) {
						result_ptr->Volume ((result_ptr->Volume () * count + volume) / (count + 1));
						result_ptr->Speed ((result_ptr->Speed () * count + speed) / (count + 1));
					} else {
						result_ptr->Volume (volume);
						result_ptr->Speed (speed);
					}
					count++;
				} else if (method == MINIMUM) {
					if (result_ptr->Volume () == 0 || result_ptr->Volume () > volume) {
						result_ptr->Volume (volume);
						result_ptr->Speed (speed);
					}
				} else if (method == MAXIMUM) {
					if (result_ptr->Volume () == 0 || result_ptr->Volume () < volume) {
						result_ptr->Volume (volume);
						result_ptr->Speed (speed);
					}
				}
			}

			//----- store the result by dir index -----
				
			vol_spd_ptr = vol_spd_period_ptr->Data_Ptr (index);

			vol_spd_ptr->Volume (result_ptr->Volume ());
			vol_spd_ptr->Speed (result_ptr->Speed ());
		}
	}
	End_Progress ();
}
