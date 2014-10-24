//*********************************************************
//	Get_Phasing_Data.cpp - read the phasing plan file
//*********************************************************

#include "TransimsNet.hpp"
#include "TypeDefs.hpp"

//---------------------------------------------------------
//	Get_Phasing_Data
//---------------------------------------------------------

bool TransimsNet::Get_Phasing_Data (Phasing_File &file, Phasing_Record &phasing_rec)
{
	int link, dir, node, dir_index, to_index, num, detector, signal;
	String svalue;
	
	Int_Map_Itr map_itr;
	Int2_Map_Itr map2_itr;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;

	Phasing_Data *phasing_ptr = &(phasing_rec.phasing_data);

	//---- process the header record ----
	
	if (!file.Nested ()) {
		signal = file.Signal ();
		if (signal == 0) return (false);

		map_itr = signal_map.find (signal);
		if (map_itr == signal_map.end ()) {
			Warning (String ("Phasing Signal %d was Not Found") % signal);
			return (false);
		}
		phasing_rec.Signal (map_itr->second);

		phasing_ptr->Phasing (file.Phasing ());
		phasing_ptr->Phase (file.Phase ());

		//---- process the detectors ----

		svalue = file.Detectors ();

		if (!svalue.empty ()) {
			Strings detect;
			Str_Itr itr;

			num = svalue.Parse (detect, "|+/");

			for (itr = detect.begin (); itr != detect.end (); itr++) {
				detector = itr->Integer ();
				if (detector <= 0) break;

				map_itr = detector_map.find (detector);
				if (map_itr == detector_map.end ()) {
					Warning (String ("Signal %d Detector %d was Not Found") % file.Signal () % detector);
					break;
				}
				phasing_ptr->detectors.push_back (map_itr->second);
			}
		}
		if (file.Version () > 40) return (phasing_rec.Signal () >= 0);
	}
	Movement_Data move_rec;

	move_rec.Movement (file.Movement ());

	//---- convert the link direction ----
	
	link = file.Link ();
	dir = file.Dir ();

	link_ptr = Set_Link_Direction (file, link, dir);

	if (link_ptr == 0) return (false);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
		node = link_ptr->Anode ();
	} else {
		dir_index = link_ptr->AB_Dir ();
		node = link_ptr->Bnode ();
	}
	if (dir_index < 0) {
		Warning (String ("Phasing Plan %d Link %d Direction %s was Not Found") % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		return (false);
	}

	//---- convert the to-link number ----

	link = file.To_Link ();

	map_itr = link_map.find (link);
	if (map_itr == link_map.end ()) {
		Warning (String ("Phasing Plan %d Link %d was Not Found") % Progress_Count () % link);
		return (false);
	}
	link = map_itr->second;

	link_ptr = &link_array [link];

	if (link_ptr->Anode () == node) {
		to_index = link_ptr->AB_Dir ();
	} else if (link_ptr->Bnode () == node) {
		to_index = link_ptr->BA_Dir ();
	} else {
		Node_Data *ptr = &node_array [node];
		Warning (String ("Phasing Plan %d Node %d is Not on Link %d") % Progress_Count () % ptr->Node () % link_ptr->Link ());
		return (false);
	}

	//---- check the link connection ----

	map2_itr = connect_map.find (Int2_Key (dir_index, to_index));

	if (map2_itr == connect_map.end ()) {
		to_index = link_ptr->Link ();

		dir_ptr = &dir_array [dir_index];
		link_ptr = &link_array [dir_ptr->Link ()];
		dir_index = link_ptr->Link ();

		if (file.Version () > 40) {
			Warning (String ("No Link Connection between %d and %d") % dir_index % to_index);
		}
		return (false);
	}

	//---- save the data ----

	move_rec.Connection (map2_itr->second);

	move_rec.Protection (file.Protection ());

	if (file.Version () <= 40 && move_rec.Movement () == 0) {
		Connect_Data *connect_ptr;
		const char *direction;

		dir_ptr = &dir_array [dir_index];
		direction = compass.Point_Text (dir_ptr->In_Bearing (), 4);

		if (*direction == 'N') {
			dir = NB_TH;
		} else if (*direction == 'S') {
			dir = SB_TH;
		} else if (*direction == 'E') {
			dir = EB_TH;
		} else {
			dir = WB_TH;
		}
		connect_ptr = &connect_array [map2_itr->second];
		num = connect_ptr->Type ();
		
		if (num == RIGHT || num == R_SPLIT) {
			dir += NB_RT - NB_TH;
		} else if (num == LEFT || num == L_SPLIT) {
			dir += NB_LT - NB_TH;
		} else if (num == UTURN) {
			dir += NB_UT - NB_TH;
		}
		move_rec.Movement (dir);
	}
	phasing_ptr->push_back (move_rec);

	return (phasing_rec.Signal () >= 0);
}
