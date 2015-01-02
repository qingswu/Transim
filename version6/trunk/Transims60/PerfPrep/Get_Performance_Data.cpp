//*********************************************************
//	Get_Performance_Data.cpp - read the performance file
//*********************************************************

#include "PerfPrep.hpp"

#include "Shape_Tools.hpp"

//---------------------------------------------------------
//	Get_Performance_Data
//---------------------------------------------------------

bool PerfPrep::Get_Performance_Data (Performance_File &file, Performance_Data &perf_rec)
{
	bool flag = Data_Service::Get_Performance_Data (file, perf_rec);

	if (!flag && deleted_flag) {
		if (Report_Flag (DELETED_RECORDS)) {
			if (first_delete) {
				first_delete = false;
				Header_Number (DELETED_RECORDS);
				New_Page ();
			}
			Deleted_Report (file);
		}
		if (del_file_flag) {
			deleted_file.Copy_Fields (file);
			deleted_file.Write ();
		}
	}
	if (flag) {
		Dir_Data *dir_ptr = &dir_array [perf_rec.Dir_Index ()];
		Link_Data *link_ptr = &link_array [dir_ptr->Link ()];
		Node_Data *node_ptr;
		Data_Range_Itr range_itr;

		if (select_links) {
			for (range_itr = link_ranges.begin (); range_itr != link_ranges.end (); range_itr++) {
				if (range_itr->In_Range (link_ptr->Link ())) break;
			}
			if (range_itr == link_ranges.end ()) return (false);
		}
		if (select_nodes) {
			node_ptr = &node_array [link_ptr->Anode ()];

			for (range_itr = node_ranges.begin (); range_itr != node_ranges.end (); range_itr++) {
				if (range_itr->In_Range (node_ptr->Node ())) break;
			}
			if (range_itr == link_ranges.end ()) return (false);
		}
		if (select_facilities) {
			if (!select_facility [link_ptr->Type ()]) {
				return (false);
			}
		}
		if (select_subareas) {
			node_ptr = &node_array [link_ptr->Anode ()];
			if (!subarea_range.In_Range (node_ptr->Subarea ())) {
				return (false);
			}
		}
		if (select_polygon) {
			node_ptr = &node_array [link_ptr->Anode ()];

			if (!In_Polygon (polygon_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
				node_ptr = &node_array [link_ptr->Bnode ()];

				if (!In_Polygon (polygon_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
					return (false);
				}
			}
		}
	}
	return (flag);
}
