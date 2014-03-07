//*********************************************************
//	Normalize.cpp - Normalize Origin Zones
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Normalize_Origins
//---------------------------------------------------------

void TourData::Normalize_Origins ()
{
	int org, des;
	double target, base, diff;
	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;
	Table_Itr table_itr;

	if (!target_matrix->Read_Matrix ()) {
		Error (String ("Reading %d") % target_matrix->File_Type ());
	}
	if (!base_matrix->Read_Matrix ()) {
		Error (String ("Reading %s") % base_matrix->File_Type ());
	}
	Show_Message ("Normalizing Origins -- Record");
	Set_Progress ();

	//---- process each origin table ----

	org_map = base_matrix->Org_Map ();

	for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
		org = target_matrix->Org_Index (org_itr->first);
		if (org < 0) continue;

		for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
			Show_Progress ();

			base = base_matrix->Row_Sum_Index (0, org_itr->second, table_itr->base);

			target = target_matrix->Row_Sum_Index (0, org, table_itr->target);

			if (target > 0) {
				target_matrix->Factor_Row_Index (0, org, table_itr->target, (base / target));
			}
		}
	}
	End_Progress ();

	//---- check for significant differences ----

	if (min_difference > 0.0 || min_percent_diff > 0.0) {
		des_map = base_matrix->Des_Map ();

		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
			org = target_matrix->Org_Index (org_itr->first);
			if (org < 0) continue;

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				des = target_matrix->Des_Index (des_itr->first);
				if (des < 0) continue;
		
				for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {

					base_matrix->Get_Cell_Index (0, org_itr->second, des_itr->second, table_itr->base, base);
					target_matrix->Get_Cell_Index (0, org, des, table_itr->target, target);

					if (base > 0 && target > 0) {
						diff = fabs (target - base);
						if (diff < min_difference || (diff / base) < min_percent_diff) {
							target_matrix->Set_Cell_Index (0, org, des, table_itr->target, 0);
						}
					} else if (target > 0) {
						target_matrix->Set_Cell_Index (0, org, des, table_itr->target, 0);
					}
				}
			}
		}
	}
}
