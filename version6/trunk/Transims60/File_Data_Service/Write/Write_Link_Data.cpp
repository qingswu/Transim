//*********************************************************
//	Write_Link_Data.cpp - Write the Link Data File
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Link_Data
//---------------------------------------------------------

void Data_Service::Write_Link_Data (Link_Data_File &link_data_file, Volume_Array &data, bool fill_flag)
{
	int period, num_periods, count;
	double volume;
	bool flag;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Vol_Period_Ptr period_ptr;

	Show_Message (String ("Writing %s -- Record") % link_data_file.File_Type ());
	Set_Progress ();

	num_periods = link_data_file.Num_Periods ();
	count = 0;

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		link_data_file.Link (link_ptr->Link ());

		node_ptr = &node_array [link_ptr->Anode ()];
		link_data_file.Anode (node_ptr->Node ());

		node_ptr = &node_array [link_ptr->Bnode ()];
		link_data_file.Bnode (node_ptr->Node ());

		flag = fill_flag;

		for (period=0; period < num_periods; period++) {
			period_ptr = data.Period_Ptr (period);

			if (link_ptr->AB_Dir () >= 0) {
				volume = period_ptr->at (link_ptr->AB_Dir ());
				link_data_file.Data_AB (period, volume);

				if (volume != 0) flag = true;
			} else {
				link_data_file.Data_AB (period, 0);
			}
			if (link_ptr->BA_Dir () >= 0) {
				volume = period_ptr->at (link_ptr->BA_Dir ());
				link_data_file.Data_BA (period, volume);

				if (volume != 0) flag = true;
			} else {
				link_data_file.Data_BA (period, 0);
			}
		}
		if (flag) {
			link_data_file.Write ();
			count++;
		}
	}
	End_Progress ();

	link_data_file.Close ();

	Print (2, String ("%s Records = %d") % link_data_file.File_Type () % count);
}


//---------------------------------------------------------
//	Write_Link_Data
//---------------------------------------------------------

void Data_Service::Write_Link_Data (Link_Data_File &link_data_file, Vol_Spd_Period_Array &data, bool vol_flag, bool fill_flag)
{
	int period, num_periods, count;
	double value;
	bool flag;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Vol_Spd_Period *period_ptr;
	Vol_Spd_Data *vol_spd_ptr;

	Show_Message (String ("Writing %s -- Record") % link_data_file.File_Type ());
	Set_Progress ();

	num_periods = link_data_file.Num_Periods ();
	count = 0;

	for (map_itr = link_map.begin (); map_itr != link_map.end (); map_itr++) {
		Show_Progress ();

		link_ptr = &link_array [map_itr->second];

		link_data_file.Link (link_ptr->Link ());

		node_ptr = &node_array [link_ptr->Anode ()];
		link_data_file.Anode (node_ptr->Node ());

		node_ptr = &node_array [link_ptr->Bnode ()];
		link_data_file.Bnode (node_ptr->Node ());

		flag = fill_flag;

		for (period=0; period < num_periods; period++) {
			period_ptr = data.Period_Ptr (period);

			if (link_ptr->AB_Dir () >= 0) {
				vol_spd_ptr = period_ptr->Data_Ptr (link_ptr->AB_Dir ());
				value = (vol_flag) ? vol_spd_ptr->Volume () : vol_spd_ptr->Speed ();

				link_data_file.Data_AB (period, value);
				if (value != 0) flag = true;
			} else {
				link_data_file.Data_AB (period, 0);
			}
			if (link_ptr->BA_Dir () >= 0) {
				vol_spd_ptr = period_ptr->Data_Ptr (link_ptr->BA_Dir ());
				value = (vol_flag) ? vol_spd_ptr->Volume () : vol_spd_ptr->Speed ();

				link_data_file.Data_BA (period, value);
				if (value != 0) flag = true;
			} else {
				link_data_file.Data_BA (period, 0);
			}
		}
		if (flag) {
			link_data_file.Write ();
			count++;
		}
	}
	End_Progress ();

	link_data_file.Close ();

	Print (2, String ("%s Records = %d") % link_data_file.File_Type () % count);
}
