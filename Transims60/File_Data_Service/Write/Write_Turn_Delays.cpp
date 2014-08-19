//*********************************************************
//	Write_Turn_Delays.cpp - write a new turn time file
//*********************************************************

#include "Data_Service.hpp"

//---------------------------------------------------------
//	Write_Turn_Delays
//---------------------------------------------------------

void Data_Service::Write_Turn_Delays (bool fill_flag)
{
	Turn_Delay_File *file = (Turn_Delay_File *) System_File_Handle (NEW_TURN_DELAY);

	Write_Turn_Delays (*file, turn_period_array, fill_flag);
}

void Data_Service::Write_Turn_Delays (Turn_Delay_File &file, Turn_Period_Array &array, bool fill_flag)
{
	int i, index, count;
	Dtime low, high, time;

	Int2_Map_Itr itr;
	Connect_Data *connect_ptr;
	Turn_Data *turn_ptr;
	Turn_Period_Itr turn_period_itr;
	Turn_Delay_Data turn_rec;

	Show_Message (String ("Writing %s -- Record") % file.File_Type ());
	Set_Progress ();

	count = 0;

	//---- process each time period ----

	for (i=0, turn_period_itr = array.begin (); turn_period_itr != array.end (); turn_period_itr++, i++) {
		
		array.periods->Period_Range (i, low, high);

		turn_rec.Start (low);
		turn_rec.End (high);

		//---- connection sort ----

		for (index=0, itr = connect_map.begin (); itr != connect_map.end (); itr++, index++) {
			connect_ptr = &connect_array [itr->second];

			Show_Progress ();

			turn_ptr = turn_period_itr->Data_Ptr (index);
			if (!fill_flag && turn_ptr->Turn () == 0) continue;

			turn_rec.Dir_Index (connect_ptr->Dir_Index ());
			turn_rec.To_Index (connect_ptr->To_Index ());
			turn_rec.Turn (turn_ptr->Turn ());
			turn_rec.Time (turn_ptr->Time ());

			count += Put_Turn_Delay_Data (file, turn_rec);
		}
	}
	Show_Progress (count);
	End_Progress ();
	file.Close ();
	
	Print (2, String ("%s Records = %d") % file.File_Type () % count);
}

//---------------------------------------------------------
//	Put_Turn_Delay_Data
//---------------------------------------------------------

int Data_Service::Put_Turn_Delay_Data (Turn_Delay_File &file, Turn_Delay_Data &data)
{
	Node_Data *node_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;

	dir_ptr = &dir_array [data.Dir_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	file.Link (link_ptr->Link ());

	dir_ptr = &dir_array [data.To_Index ()];
	link_ptr = &link_array [dir_ptr->Link ()];

	file.To_Link (link_ptr->Link ());

	if (dir_ptr->Dir ()) {
		node_ptr = &node_array [link_ptr->Bnode ()];
	} else {
		node_ptr = &node_array [link_ptr->Anode ()];
	}
	file.Node (node_ptr->Node ());

	file.Start (data.Start ());
	file.End (data.End ());

	file.Turn (data.Turn ());
	file.Time (data.Time ());

	if (!file.Write (false)) {
		Error (String ("Writing %s") % file.File_Type ());
	}
	return (1);
}

