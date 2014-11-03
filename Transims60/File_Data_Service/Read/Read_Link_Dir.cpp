//*********************************************************
//	Read_Link_Dir.cpp - read a link direction file
//*********************************************************

#include "Data_Service.hpp"

//-----------------------------------------------------------
//	Read_Link_Direction
//-----------------------------------------------------------

void Data_Service::Read_Link_Direction (Link_Direction_File &file, Volume_Array &data, bool base_flag)
{
	int i, link, dir, nmap, p1, p2, periods;
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
			Error ("Link Direction Periods do not fit into Summary Periods");
		}
	}

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

		dir = file.Dir ();

		if (dir) {
			dir = link_ptr->BA_Dir ();
		} else {
			dir = link_ptr->AB_Dir ();
		}

		//---- add the counts to the directional links ----

		if (dir >= 0) {
			volume.assign (periods, 0.0);

			for (i=0; i < nmap; i++) {
				if (volume_map [i] >= 0) {
					volume [volume_map [i]] += (base_flag) ? file.Data2 (i) : file.Data (i);
				}
			}
			for (i=0; i < periods; i++) {
				data.Volume (i, dir, volume [i]);
			}
		}
	}
	End_Progress ();
	Print (2, String ("Number of %s Records = %d") % file.File_Type () % Progress_Count ());

	file.Close ();
}
