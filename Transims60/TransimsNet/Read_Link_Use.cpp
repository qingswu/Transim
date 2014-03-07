//*********************************************************
//	Read_Link_Use.cpp - read the link use file
//*********************************************************

#include "TransimsNet.hpp"

//---------------------------------------------------------
//	Read_Link_Uses
//---------------------------------------------------------

void TransimsNet::Read_Link_Uses (void)
{
	int i, j, link, dir, index, lanes, low, high, num, num_values, count;
	Dtime start, end;
	bool lane_flag;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;
	Link_Use_Map_Stat map_stat;
	Link_Dir_Data link_dir;
	Link_Use_Data link_use_data;
	Link_Use_Array link_use_array, *link_use_ptr;
	Time_Periods period;

	//---- read link use file----

	Show_Message (String ("Reading %s -- Record") % link_use_file.File_Type ());
	Set_Progress ();

	num_values = link_use_file.Num_Values ();
	count = 0;

	while (link_use_file.Read_Record ()) {
		Show_Progress ();

		//---- get the link number ----

		link = link_use_file.Link ();
		if (link == 0) continue;

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Warning (String ("Link Use Link %d was Not Found") % link);
			continue;
		}
		link_ptr = &link_array [map_itr->second];

		dir = link_use_file.Dir ();

		if (dir == 0) {
			index = link_ptr->AB_Dir ();
		} else {
			index = link_ptr->BA_Dir ();
		}
		if (index < 0) {
			Warning (String ("Link Use Link %d Direction %s was Not Found") % link % ((dir == 0) ? "AB" : "BA"));
			continue;
		}
		map_stat = link_use_map.insert (Link_Use_Map_Data (index, link_use_array));
		if (!map_stat.second) {
			Warning ("Duplicate Link Use Record for Link = ") << link;
			continue;
		}
		link_use_ptr = &map_stat.first->second;

		for (i=0; i < num_values; i++) {
			link_use_data.Type (link_use_file.Type (i));
			link_use_data.Use (link_use_file.Use (i));

			//---- lane number ----

			lanes = link_use_file.Lanes (i);
			lane_flag = (lanes == 0);

			if (link_use_file.Version () <= 40 && lanes > 0) {
				low = high = lanes - 1;
			} else {
				Convert_Lane_Range (index, lanes, low, high);
			}
			link_use_data.Low_Lane (low);
			link_use_data.High_Lane (high);

			link_use_data.Toll (link_use_file.Toll (i));
			link_use_data.Speed (link_use_file.Speed (i));

			period.clear ();
			period.Add_Ranges (link_use_file.Period (i));

			num = period.Num_Periods ();

			if (link_use_data.Type () == 0 && Use_Permission (link_use_data.Use (), ANY) && lane_flag && 
				link_use_data.Toll () == 0 && link_use_data.Speed () == 0 && num == 0) continue;

			if (num == 0) {
				link_use_data.Start (Model_Start_Time ());
				link_use_data.End (Model_End_Time ());

				link_use_ptr->push_back (link_use_data);
				count++;
			} else {
				for (j=0; j < num; j++) {
					period.Period_Range (j, start, end);

					link_use_data.Start (start);
					link_use_data.End (end);

					link_use_ptr->push_back (link_use_data);
					count++;
				}
			}
		}
	}
	End_Progress ();
	link_use_file.Close ();

	Print (2, "Number of ") << link_use_file.File_Type () << " Records = " << count;
}
