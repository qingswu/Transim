//*********************************************************
//	Create_Link_Map.cpp - create link data structures
//*********************************************************

#include "LinkData.hpp"

//---------------------------------------------------------
//	Create_Link_Map
//---------------------------------------------------------

void LinkData::Create_Link_Map (void)
{
	int link, index, anode, bnode;
	bool flag;
	String record;
	Strings parse;
	Str_Itr str_itr;
	Int2_Map_Stat map_stat;

	Int_Map_Itr itr;
	Int_Itr int_itr;
	Link_Node_Itr link_itr;

	Data data_rec;

	//---- initialize the data records ----

	if (data_flag) {
		data_rec.link = 0;
		data_rec.num_ab = 0;
		data_rec.num_ba = 0;

		data_rec.volume_ab.assign (num_fields, 0.0);
		data_rec.volume_ba.assign (num_fields, 0.0);
		if (speed_flag) {
			data_rec.speed_ab.assign (num_fields, 0.0);
			data_rec.speed_ba.assign (num_fields, 0.0);
		}
	}

	//---- process each link ----
	
	Show_Message ("Creating Link Data -- Record");
	Set_Progress ();

	for (index=0, link_itr = link_node_array.begin (); link_itr != link_node_array.end (); link_itr++, index++) {
		Show_Progress ();

		link = anode = bnode = 0;
		flag = false;

		data_rec.link = index;

		for (anode=0, int_itr = link_itr->nodes.begin (); int_itr != link_itr->nodes.end (); int_itr++) {
			if (anode == 0) {
				anode = *int_itr;
			} else {
				bnode = *int_itr;
				if (bnode == 0) continue;

				map_stat = data_map.insert (Int2_Map_Data (Int2_Key (anode, bnode), (int) data_array.size ()));

				if (!map_stat.second) {
					Warning (String ("Duplicate AB Nodes (%d-%d)") % anode % bnode);
				} else {
					flag = true;
				}
				anode = bnode;
				nab++;
			}
		}
		if (flag) {
			data_array.push_back (data_rec);
		}
	}
	End_Progress ();
	nequiv = Progress_Count ();

	link_node.Close ();
}
