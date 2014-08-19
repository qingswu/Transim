//*********************************************************
//	Write_Turn_Data.cpp - Write the Turn Data File
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Write_Turn_Data
//---------------------------------------------------------

void LinkSum::Write_Turn_Data (void)
{
	int nrec, node, index, period, nperiod;
	Dtime low, high;
	bool select_flag;

	Ints_Map node_sort;
	Ints_Map_Itr sort_itr;
	Integers ints_rec;
	Int_Itr int_itr;
	Int_Map_Itr itr;
	Dir_Itr dir_itr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Node_Data *node_ptr;
	Connect_Data *connect_ptr;
	Turn_Period *period_ptr;
	Turn_Data *turn_ptr;
	Turn_Delay_File *turn_file;
	Turn_Delay_Data turn_data;

	turn_file = (Turn_Delay_File *) System_File_Handle (NEW_TURN_DELAY);

	Show_Message (String ("Writing %s -- Record") % turn_file->File_Type ());
	Set_Progress ();

	//---- construct the node link liste ----

	select_flag = (turn_range.size () > 0);
	nperiod = time_periods.Num_Periods ();

	for (itr = node_map.begin (); itr != node_map.end (); itr++) {
		if (select_flag && !turn_range.In_Range (itr->first)) continue;

		node_sort.insert (Ints_Map_Data (itr->first, ints_rec));
	}

	//---- assign links to nodes ----

	for (index=0, dir_itr = dir_array.begin (); dir_itr != dir_array.end (); dir_itr++, index++) {
		link_ptr = &link_array [dir_itr->Link ()];

		node = (dir_itr->Dir () == 0) ? link_ptr->Bnode () : link_ptr->Anode ();

		node_ptr = &node_array [node];

		sort_itr = node_sort.find (node_ptr->Node ());
		if (sort_itr == node_sort.end ()) continue;

		sort_itr->second.push_back (index);
	}

	//---- process each node ----

	nrec = 0;

	for (sort_itr = node_sort.begin (); sort_itr != node_sort.end (); sort_itr++) {
		Show_Progress ();

		for (int_itr = sort_itr->second.begin (); int_itr != sort_itr->second.end (); int_itr++) {

			turn_data.Dir_Index (*int_itr);

			dir_ptr = &dir_array [*int_itr];

			for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
				connect_ptr = &connect_array [index];

				turn_data.To_Index (connect_ptr->To_Index ());

				for (period=0; period < nperiod; period++) {
					period_ptr = &turn_period_array [period];
					if (index >= (int) period_ptr->size ()) continue;

					turn_ptr = period_ptr->Data_Ptr (index);

					turn_data.Start (low);
					turn_data.End (high);
					turn_data.Time (turn_ptr->Time ());
					turn_data.Turn (turn_ptr->Turn ());

					Put_Turn_Delay_Data (*turn_file, turn_data);
					nrec++;
				}
			}
		}
	}
	End_Progress ();

	Print (2, "Number of New Turn Delay Records = ") << nrec;
}
