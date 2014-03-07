//*********************************************************
//	Transfer_Link.cpp - insert platform links
//*********************************************************

#include "RoutePrep.hpp"

//---------------------------------------------------------
//	Transfer_Link
//---------------------------------------------------------

void RoutePrep::Transfer_Link (void)
{
	int link_index, dir_index, length, best_len, best;
	double dx, dy, speed;

	Ints_Itr st1_itr, st2_itr;
	Int_Itr a_itr, b_itr;
	Node_Data *a_ptr, *b_ptr;
	Link_Data link_rec;
	Dir_Data dir_rec;

	if (station_nodes.size () < 2) return;

	link_rec.Use (Use_Code ("WALK"));
	link_rec.Type (WALKWAY);
	link_rec.Divided (1);

	link_rec.Name ("Transfer Link");

	dir_rec.Lanes (1);
	dir_rec.Speed (Internal_Units (3.0, MPH));
	dir_rec.Capacity (1000);
	speed = UnRound (dir_rec.Speed ());

	for (st1_itr = station_nodes.begin (); st1_itr != station_nodes.end (); st1_itr++) {
		st2_itr = st1_itr + 1;
		if (st2_itr == station_nodes.end ()) break;

		for (a_itr = st1_itr->begin (); a_itr != st1_itr->end (); a_itr++) {
			a_ptr = &node_array [*a_itr];

			best = -1; 
			best_len = MAX_INTEGER;

			for (b_itr = st2_itr->begin (); b_itr != st2_itr->end (); b_itr++) {
				b_ptr = &node_array [*b_itr];

				dx = a_ptr->X () - b_ptr->X ();
				dy = a_ptr->Y () - b_ptr->Y ();

				length = DTOI (sqrt (dx * dx + dy * dy));

				if (length < best_len) {
					best = *b_itr;
					best_len = length;
				}
			}
			if (best < 0) continue;

			link_index = (int) link_array.size ();
			dir_index = (int) dir_array.size ();
	
			link_rec.Anode (*a_itr);
			link_rec.Bnode (best);
			link_rec.Length (best_len);
			dir_rec.Time0 (UnRound (best_len) / speed);

			link_rec.AB_Dir (dir_index);

			dir_rec.Link (link_index);
			dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir_index));
			dir_array.push_back (dir_rec);

			link_rec.BA_Dir (++dir_index);

			dir_rec.Dir (1);

			dir_map.insert (Int_Map_Data (dir_rec.Link_Dir (), dir_index));
			dir_array.push_back (dir_rec);

			link_rec.Link (new_link++);
			link_map.insert (Int_Map_Data (link_rec.Link (), link_index));
			link_array.push_back (link_rec);
		}
	}
}
