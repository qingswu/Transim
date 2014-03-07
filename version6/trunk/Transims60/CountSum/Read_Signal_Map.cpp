//*********************************************************
//	Read_Signal_Map.cpp - read signal id map file
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Read_Signal_Map
//---------------------------------------------------------

void CountSum::Read_Signal_Map (void)
{
	int id, node, ext_node;
	Signal_Point signal_rec;
    Signal_Pt_Stat signal_stat;
	Int_Map_Stat map_stat;
	Int_Map_Itr map_itr;

	//---- read signal id map----

	Show_Message (String ("Reading %s -- Record") % signal_map_file.File_Type ());
	Set_Progress ();

	while (signal_map_file.Read ()) {
		Show_Progress ();

		//---- get the signal number ----

		id = signal_map_file.Get_Integer (map_id_field);
		if (id == 0) continue;

		ext_node = signal_map_file.Get_Integer (node_field);

		map_itr = node_map.find (ext_node);
		if (map_itr == node_map.end ()) {
			Warning (String ("Node %d was Not Found in the Node File") % ext_node);
			continue;
		}
		node = map_itr->second;

		if (arc_signal_flag) {
			map_stat = id_map.insert (Int_Map_Data (id, node));

			if (!map_stat.second) {
				Warning ("Duplicate Signal ID Map = ") << id;
			}
		} else {
			signal_rec.node = node;

			signal_stat = signal_pt_map.insert (Signal_Pt_Data (id, signal_rec));

			if (!signal_stat.second) {
				Warning ("Duplicate Signal Number = ") << id;
			}
			if (signal_node_flag) {
				signal_node_file.Put_Field (0, id);
				signal_node_file.Put_Field (1, ext_node);
				signal_node_file.Write ();
			}
		}
	}
	End_Progress ();

	signal_map_file.Close ();

	if (!arc_signal_flag && signal_node_flag) {
		signal_node_file.Close ();
	}

	Print (2, String ("Number of %s Records = %d") % signal_map_file.File_Type () % Progress_Count ());
}
