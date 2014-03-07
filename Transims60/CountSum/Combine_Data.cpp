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
	double flow;
	Dtime time;

	Count_Days_Itr count_itr; 
	Flow_Time_Period_Itr period_itr;
	Flow_Time_Array *period_ptr;
	Flow_Time_Data *flow_ptr, *count_ptr;

	Offset_Index_Itr offset_itr, index_itr;
	Int2_Key dir_offset;

	if (method == MINIMUM) {
		Show_Message ("Minimum Flow Data -- Record");
	} else if (method == MAXIMUM) {
		Show_Message ("Maximum Flow Data -- Record");
	} else if (day_num < 0) {
		Show_Message ("Combining Flow Data -- Record");
	} else {
		Show_Message (String ("Flow Data for Day %s -- Record") % (day_num + 1));
	}
	Set_Progress ();

	//---- initialize the flow and time data ----

	link_delay_array.Set_Time0 ();
	link_delay_array.Zero_Flows ();

	//---- process counts for each count day by time period ----

	for (period=0, period_itr = link_delay_array.begin (); period_itr != link_delay_array.end (); period_itr++, period++) {

		index = -1;

		for (offset_itr = offset_index_map.begin (); offset_itr != offset_index_map.end (); offset_itr++) {
			dir_offset = offset_itr->first;

			if (dir_offset.first == index) continue;
			
			index = dir_offset.first;

			flow_ptr = period_itr->Data_Ptr (index);
			count = 0;

			for (day=0, count_itr = count_days.begin (); count_itr != count_days.end (); count_itr++, day++) {
				Show_Progress ();

				if (day_num >= 0 && day != day_num) continue;
				
				period_ptr = &(*count_itr)->at (period);

				flow = 0;
				time = 0;
				num = 0;

				for (index_itr = offset_itr; index_itr != offset_index_map.end (); index_itr++) {
					dir_offset = index_itr->first;

					if (dir_offset.first != index) break;

					count_ptr = period_ptr->Data_Ptr (index_itr->second);

					if (count_ptr->Flow () > 0) {
						flow += count_ptr->Flow ();
						time += count_ptr->Time ();
						num++;
					}
				}
				if (num == 0) continue;

				if (num > 1) {
					flow /= num;
					time = time / num;
				}

				if (method == AVERAGE) {
					flow_ptr->Average_Flow_Time (flow, time, count);
					count++;
				} else if (method == MINIMUM) {
					if (flow_ptr->Flow () == 0 || flow_ptr->Flow () > flow) {
						flow_ptr->Flow (flow);
						flow_ptr->Time (time);
					}
				} else if (method == MAXIMUM) {
					if (flow_ptr->Flow () == 0 || flow_ptr->Flow () < flow) {
						flow_ptr->Flow (flow);
						flow_ptr->Time (time);
					}
				}
			}
		}
	}
	End_Progress ();
}
