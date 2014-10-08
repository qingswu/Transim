//*********************************************************
//	Select_Links.cpp - flag links in the selection set
//*********************************************************

#include "ArcPerf.hpp"

//---------------------------------------------------------
//	Select_Links
//---------------------------------------------------------

void ArcPerf::Select_Links (void)
{
	int count = 0;
	Node_Data *node_ptr;
	Link_Itr link_itr;

	Show_Message ("Selecting Links -- Record");
	Set_Progress ();

	for (link_itr = link_array.begin (); link_itr != link_array.end (); link_itr++) {
		Show_Progress ();

		if (!select_facility [link_itr->Type ()]) {
			link_itr->Use (0);
			continue;
		}
		if (select_subareas) {
			node_ptr = &node_array [link_itr->Anode ()];
			if (!subarea_range.In_Range (node_ptr->Subarea ())) {
				link_itr->Use (0);
				continue;
			}
		}
		if (select_polygon) {
			node_ptr = &node_array [link_itr->Anode ()];

			if (!In_Polygon (polygon_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
				node_ptr = &node_array [link_itr->Bnode ()];

				if (!In_Polygon (polygon_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
					link_itr->Use (0);
					continue;
				}
			}
		}
		if (select_links) {
			bool flag = false;
			Data_Range_Itr range_itr;

			for (range_itr = link_ranges.begin (); range_itr != link_ranges.end (); range_itr++) {
				if (range_itr->In_Range (link_itr->Link ())) {
					flag = true;
					break;
				}
			}
			if (!flag) {
				link_itr->Use (0);
				continue;
			}
		}
		count++;
	}
	Print (2, "Number of Links Selected = ") << count;
}
