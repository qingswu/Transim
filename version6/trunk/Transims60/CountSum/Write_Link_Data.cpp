//*********************************************************
//	Write_Link_Data.cpp - Write the Link Data File
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Write_Link_Data
//---------------------------------------------------------

void CountSum::Write_Link_Data (Link_Data_File &link_data_file, Flow_Time_Period_Array &link_delay_array)
{
	int period, num_periods, count;
	bool flag;

	Int_Map_Itr map_itr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Flow_Time_Array *array_ptr;
	Flow_Time_Data *flow_ptr;

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

		flag = false;

		for (period=0; period < num_periods; period++) {
			array_ptr = &link_delay_array [period];

			if (link_ptr->AB_Dir () >= 0) {
				flow_ptr = &array_ptr->at (link_ptr->AB_Dir ());
				link_data_file.Data_AB (period, flow_ptr->Flow ());

				if (flow_ptr->Flow () != 0) flag = true;
			} else {
				link_data_file.Data_AB (period, 0);
			}
			if (link_ptr->BA_Dir () >= 0) {
				flow_ptr = &array_ptr->at (link_ptr->BA_Dir ());
				link_data_file.Data_BA (period, flow_ptr->Flow ());

				if (flow_ptr->Flow () != 0) flag = true;
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
