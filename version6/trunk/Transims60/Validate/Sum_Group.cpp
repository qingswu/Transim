//*********************************************************
//	Sum_Group.cpp - Summarize Group Statistics
//*********************************************************

#include "Validate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Sum_Group
//---------------------------------------------------------

void Validate::Sum_Group (int group, Link_Data *link_ptr, int dir)
{
	int i, j, num, use_index;
	double volume, count, capacity, error, vc;
	bool flag;

	Dir_Data *dir_ptr;
	Group_Data *data, data_rec;
	Group_Map_Itr map_itr;
	Group_Map_Stat map_stat;
	Vol_Period_Ptr vol_ptr;

	if (group == 0) return;
	
	//---- check for counts ----

	count = 0;
	num = sum_periods.Num_Periods ();
	if (num > (int) link_count_array.size ()) num = (int) link_count_array.size ();

	for (i=0; i < num; i++) {
		vol_ptr = link_count_array.Period_Ptr (i);
		count += vol_ptr->at (dir);
	}
	if (count == 0) return;

	//---- get the data record ----

	data = 0;
	memset (&data_rec, '\0', sizeof (data_rec));

	if (group < 0) {
		flag = true;
	} else {
		flag = false;

		map_stat = group_data.insert (Group_Map_Data (group, data_rec));

		data = &map_stat.first->second;
		data->index = group;
	}
	dir_ptr = &dir_array [dir];
	use_index = dir_ptr->Use_Index ();

	//---- process each period ----

	for (i=0; i < num; i++) {
		count = link_count_array.Volume (i, dir);
		if (use_index > 0) {
			count += link_count_array.Volume (i, use_index);
		}
		count *= factor;
		volume = link_volume_array.Volume (i, dir);
		if (use_index > 0) {
			volume += link_volume_array.Volume (i, use_index);
		}
		error = fabs (volume - count);

		if (flag) {
			data = 0;

			for (j=0; volume_level [j] > 0; j++) {
				if (count < volume_level [j]) {
					group = j;

					map_stat = group_data.insert (Group_Map_Data (group, data_rec));
					
					data = &map_stat.first->second;
					data->index = group;
					break;
				}
			}
			if (data == 0) continue;
		}
		
		capacity = dir_ptr->Capacity () * hours;
		if (capacity > 0.0) {
			vc = volume / capacity;
			if (vc > 99.99) vc = 99.99;
		} else {
			vc = 0.0;
		}

		if (method) {
			double length = UnRound (link_ptr->Length ()) / (Metric_Flag () ? 1000.0 : 5280.0);

			volume *= length;
			count *= length;
			error *= length;
			capacity *= length;
		}

		//---- sum to category ----

		data->number++;

		data->volume += volume;
		data->volume_sq += volume * volume;

		data->count += count;
		data->count_sq += count * count;

		data->error += error;
		data->error_sq += error * error;
		
		data->count_volume += count * volume;

		data->capacity += capacity;
		if (vc > data->max_vc) data->max_vc = vc;

		//---- add to total as well ----

		total.number++;

		total.volume += volume;
		total.volume_sq += volume * volume;

		total.count += count;
		total.count_sq += count * count;

		total.error += error;
		total.error_sq += error * error;
		
		total.count_volume += count * volume;

		total.capacity += capacity;
		if (vc > total.max_vc) total.max_vc = vc;
	}
}
