//*********************************************************
//	Read_Points.cpp - read signal points
//*********************************************************

#include "CountSum.hpp"

#include <math.h>

//---------------------------------------------------------
//	Read_Signal_Points
//---------------------------------------------------------

void CountSum::Read_Signal_Points (void)
{
	int id, node, best_node, ext_node;
	double dx, dy, best_dist, dist, max_dist2;
	bool select_flag, flag;
	XYZ_Point *pts;
	Str_Itr str_itr;
	Node_Itr node_itr, best_itr;
	Signal_Point signal_rec;
	Signal_Pt_Itr signal_itr;
    Signal_Pt_Stat signal_stat;
	Int_Map_Itr map_itr;

	//---- read signal points----

	Show_Message (String ("Reading %s -- Record") % arc_signals.File_Type ());
	Set_Progress ();

	select_flag = (signal_types.size () > 0);
	max_dist2 = max_dist * max_dist;

	while (arc_signals.Read_Record ()) {
		Show_Progress ();

		//---- get the signal number ----

		id = arc_signals.Get_Integer (signal_field);
		if (id == 0) continue;

		if (signal_pt_map.find (id) != signal_pt_map.end ()) continue;

		signal_rec.type = arc_signals.Get_String (type_field).Trim ();

		if (select_flag) {
			flag = false;
			for (str_itr = signal_types.begin (); str_itr != signal_types.end (); str_itr++) {
				if (str_itr->Equals (signal_rec.type)) {
					flag = true;
					break;
				}
			}
			if (!flag) continue;
		}
		best_node = ext_node = -1;

		//---- check the ID map ----

		if (signal_map_flag) {
			map_itr = id_map.find (id);

			if (map_itr != id_map.end ()) {
				best_node = map_itr->second;
				ext_node = node_array [best_node].Node ();
			}
		}

		//---- find the closest node ----

		if (best_node == -1) {
			pts = arc_signals.Get_Points ();

			flag = false;
			best_dist = max_dist2;

			for (node=0, node_itr = best_itr = node_array.begin (); node_itr != node_array.end (); node_itr++, node++) {
				dx = pts->x - UnRound (node_itr->X ());
				dy = pts->y - UnRound (node_itr->Y ());

				dist = dx * dx + dy * dy;
				if (dist < best_dist) {
					best_node = node;
					best_itr = node_itr;
					ext_node = node_itr->Node ();
					flag = true;
					if (dist > 0) {
						best_dist = dist;
					} else {
						break;
					}
				}
			}
			if (flag) {
				if (best_itr->Control () > 0) {
					Warning (String ("Multiple Signals assigned to Node %d") % ext_node);
				}
				best_itr->Control (id);
			} else {
				Warning (String ("Signal %d was Not Matched to a Node") % id);
				continue;
			}
		}

		signal_rec.node = best_node;
		signal_rec.primary = arc_signals.Get_String (primary_field).Trim ();
		signal_rec.cross = arc_signals.Get_String (cross_field).Trim ();

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
	End_Progress ();

	arc_signals.Close ();

	if (Progress_Count () != (int) signal_pt_map.size ()) {
		Show_Message (1, "Number of Located Signal Records = ") << signal_pt_map.size ();
		Show_Message (1);
	}
	Print (2, "Number of ") << arc_signals.File_Type () << " Records = " << signal_pt_map.size ();

	if (signal_node_flag) {
		signal_node_file.Close ();
	}
}
