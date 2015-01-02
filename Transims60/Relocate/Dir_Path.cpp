//*********************************************************
//	Dir_Path.cpp - build a path between two links
//*********************************************************

#include "Relocate.hpp"

#include "Dir_Path_Data.hpp"

//---------------------------------------------------------
//	Dir_Path
//---------------------------------------------------------

bool Relocate::Dir_Path (int dir1, int dir2, Use_Type use, Integers &path_array)
{
	int dir, best_cum, cum_a, cum_b, dir_index, index;

	Dir_Path_Data *path_ptr, *first_ptr, *last_ptr, path_root;
	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Connect_Data *connect_ptr;
	
	Dir_Path_Array dir_path_array;

	path_array.clear ();
	if (dir1 < 0 || dir2 < 0) return (false);

	dir_path_array.assign (dir_array.size (), path_root);

	first_ptr = &path_root;
	first_ptr->Next_List (dir1);
	last_ptr = &dir_path_array [dir1];

	//---- link leaving the previous link ----

	dir_ptr = &dir_array [dir1];

	//---- process each link leaving the node ----

	for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {

		connect_ptr = &connect_array [index];

		dir_index = connect_ptr->To_Index ();

		dir_ptr = &dir_array [dir_index];
		link_ptr = &link_array [dir_ptr->Link ()];

		if (link_ptr->Divided () == 0 || !Use_Permission (link_ptr->Use (), use)) continue;

		cum_b = link_ptr->Length ();

		path_ptr = &dir_path_array [dir_index];

		if (path_ptr->Next_List () == -1 && last_ptr != path_ptr) {
			last_ptr->Next_List (dir_index);
			last_ptr = path_ptr;
		}
		path_ptr->Distance (cum_b);
		path_ptr->From_Dir (dir1);

	}
	if (last_ptr == &dir_path_array [dir1]) return (false);

	//---- build a path to the destination node ----

	best_cum = MAX_INTEGER;

	for (;;) {
		dir = first_ptr->Next_List ();
		if (dir < 0) break;

		first_ptr->Next_List (-2);

		first_ptr = &dir_path_array [dir];

		//---- check the cumulative impedance ----

		cum_a = first_ptr->Distance ();
		if (cum_a >= best_cum) continue;

		//---- identify the approach link ----

		dir_ptr = &dir_array [dir];

		//---- process each link leaving the link ----

		for (index = dir_ptr->First_Connect (); index >= 0; index = connect_ptr->Next_Index ()) {
			connect_ptr = &connect_array [index];

			dir_index = connect_ptr->To_Index ();

			dir_ptr = &dir_array [dir_index];
			link_ptr = &link_array [dir_ptr->Link ()];

			if (link_ptr->Divided () == 0 || !Use_Permission (link_ptr->Use (), use)) continue;

			if (dir_index == dir) continue;

			//---- check the cumulative distance ----

			cum_b = cum_a + link_ptr->Length ();
			if (cum_b > best_cum) continue;

			path_ptr = &dir_path_array [dir_index];

			if (cum_b >= path_ptr->Distance () && path_ptr->Distance () > 0) continue;

			if (dir_index == dir2) {
				best_cum = cum_b;
			}

			//---- add to the tree ----

			if (path_ptr->Next_List () == -2) {
				path_ptr->Next_List (first_ptr->Next_List ());
				first_ptr->Next_List (dir_index);
			} else if (path_ptr->Next_List () == -1 && last_ptr != path_ptr) {
				last_ptr->Next_List (dir_index);
				last_ptr = path_ptr;
			}
			path_ptr->Distance (cum_b);
			path_ptr->From_Dir (dir);
		}
	}

	//---- trace the path ----

	while (dir2 != dir1 && dir2 >= 0) {
		path_array.push_back (dir2);

		path_ptr = &dir_path_array [dir2];

		dir2 = path_ptr->From_Dir ();
	}
	return (dir2 == dir1);
}
