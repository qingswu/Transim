//*********************************************************
//	Read_Link_Data.cpp - read a link data file
//*********************************************************

#include "Data_Service.hpp"

//-----------------------------------------------------------
//	Read_Link_Data
//-----------------------------------------------------------

void Data_Service::Read_Link_Data (Link_Data_File &file, Volume_Array &data)
{
	int i, link, anode, bnode, an, bn, ab_dir, ba_dir, nmap, p1, p2, periods;
	Dtime low, high;
	Integers volume_map;
	Doubles volume;

	Link_Data *link_ptr;
	Int_Map_Itr map_itr;

	periods = sum_periods.Num_Periods ();

	if (data.size () == 0) {
		data.Initialize (&sum_periods, (int) (dir_array.size () + Num_Lane_Use_Flows ()));
	} else {
		data.Zero_Volumes ();
	}

	//---- read the link data file ----

	Show_Message (String ("Reading %s -- Record") % file.File_Type ());
	Set_Progress ();

	//---- build the volume map ----

	nmap = file.Num_Periods ();

	volume_map.assign (nmap, 0);

	for (i=0; i < nmap; i++) {
		file.Period_Range (i, low, high);

		p1 = sum_periods.Period (low);
		p2 = sum_periods.Period (high - 1);

		volume_map [i] = sum_periods.Period ((low + high) / 2);

		if (p1 != p2 || p1 != volume_map [i]) {
			Db_Field *field = file.Field (file.AB (i));
			Error (String ("Link Data Field (%s) does not fit the Time Period") % field->Name ());
		}
	}
	ab_dir = ba_dir = -1;

	//---- read the volume data ----

	while (file.Read ()) {
		Show_Progress ();

		link = file.Link ();
		if (link == 0) continue;

		map_itr = link_map.find (link);
		if (map_itr == link_map.end ()) {
			Warning (String ("Link %d was Not Found in the Link File") % link);
			continue;
		}
		link_ptr = &link_array [map_itr->second];

		anode = file.Anode ();
		bnode = file.Bnode ();

		if (anode != 0 && bnode != 0) {
			map_itr = node_map.find (anode);
			if (map_itr == node_map.end ()) {
				Warning (String ("Link %d Anode %d was Not Found in the Node File") % link % anode);
				continue;
			}
			an = map_itr->second;

			map_itr = node_map.find (bnode);
			if (map_itr == node_map.end ()) {
				Warning (String ("Link %d Bnode %d was Not Found in the Node File") % link % bnode);
				continue;
			}
			bn = map_itr->second;

			if (an == link_ptr->Anode () && bn == link_ptr->Bnode ()) {
				ab_dir = link_ptr->AB_Dir ();
				ba_dir = link_ptr->BA_Dir ();
			} else if (bn == link_ptr->Anode () && an == link_ptr->Bnode ()) {
				ab_dir = link_ptr->BA_Dir ();
				ba_dir = link_ptr->AB_Dir ();
			} else {
				Warning (String ("Anode %d or Bnode %d were Not Found on Link %d") % anode % bnode % link);
				continue;
			}
		} else {
			ab_dir = link_ptr->AB_Dir ();
			ba_dir = link_ptr->BA_Dir ();
		}

		//---- add the counts to the directional links ----

		if (ab_dir >= 0) {
			volume.assign (periods, 0.0);

			for (i=0; i < nmap; i++) {
				if (volume_map [i] >= 0) {
					volume [volume_map [i]] += file.Data_AB (i);
				}
			}
			for (i=0; i < periods; i++) {
				data.Volume (i, ab_dir, volume [i]);
			}
		}
		if (ba_dir >= 0) {
			volume.assign (periods, 0.0);

			for (i=0; i < nmap; i++) {
				if (volume_map [i] >= 0) {
					volume [volume_map [i]] += file.Data_BA (i);
				}
			}
			for (i=0; i < periods; i++) {
				data.Volume (i, ba_dir, volume [i]);
			}
		}
	}
	End_Progress ();
	Print (2, String ("Number of %s Records = %d") % file.File_Type () % Progress_Count ());

	file.Close ();
}
