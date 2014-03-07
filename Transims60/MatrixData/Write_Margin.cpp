//*********************************************************
//	Write_Margin.cpp - Write a Margin Total File
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Write_Margin_Totals
//---------------------------------------------------------

void MatrixData::Write_Margin_Totals (void)
{
	int i, j, period, num_periods, zone, max_zone, tab, num_tables;
	double total, value;
	Doubles org_sum, des_sum, org_tot, des_tot;
	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;
	Str_Map_Itr map_itr;

	fstream *file = &margin_file.File();

	Show_Message ("Writing Margin Total File -- Record");
	Set_Progress ();

	num_periods = new_matrix->Num_Periods ();
	if (num_periods == 0) {
		num_periods = 1;
	}
	org_map = new_matrix->Org_Map ();
	des_map = new_matrix->Des_Map ();

	org_itr = --org_map->end ();
	des_itr = --des_map->end ();

	max_zone = MAX (org_itr->first, des_itr->first);
	num_tables = new_matrix->Tables ();

	for (period=0; period < num_periods; period++) {
		if (period_flag) {
			if (new_matrix->Merge_Flag ()) {
				*file << "Period=\t" << new_matrix->Range_String () << endl;
			} else {
				*file << "Period=\t" << new_matrix->Range_Format (period) << endl;
			}
		}
		org_tot.assign (num_tables, 0);
		des_tot.assign (num_tables, 0);

		for (tab=0; tab < num_tables; tab++) {
			Db_Field *ptr = new_matrix->Table_Field (tab);
			*file << "\t" << ptr->Name () << "\t" << ptr->Name ();
		}
		*file << endl << "Group";

		for (tab=0; tab < num_tables; tab++) {
			*file << "\t" << "Org" << "\t" << "Des";
		}
		*file << endl;

		for (zone=1; zone <= max_zone; zone++) {
			Show_Progress ();

			i = new_matrix->Org_Index (zone);
			j = new_matrix->Des_Index (zone);

			if (i < 0 && j < 0) continue;

			org_sum.assign (num_tables, 0);

			if (i >= 0) {
				for (tab=0; tab < num_tables; tab++) {
					total = 0;

					for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
						new_matrix->Get_Cell_Index (period, i, des_itr->second, tab, value);
						total += value;
					}
					if (total > 0) {
						org_sum [tab] = total;
					}
				}
			}

			des_sum.assign (num_tables, 0);

			if (j >= 0) {
				for (tab=0; tab < num_tables; tab++) {
					total = 0;

					for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
						new_matrix->Get_Cell_Index (period, org_itr->second, j, tab, value);
						total += value;
					}
					if (total > 0) {
						des_sum [tab] = total;
					}
				}
			}
			*file << zone;

			for (tab=0; tab < num_tables; tab++) {
				*file << "\t" << org_sum [tab] << "\t" << des_sum [tab];

				org_tot [tab] += org_sum [tab];
				des_tot [tab] += des_sum [tab];
			}
			if (equiv_flag) {
				*file << "\t" << zone_equiv.Group_Label (zone);
			} else if (label_flag) {
				map_itr = label_map.find (zone);
				if (map_itr != label_map.end ()) {
					*file << "\t" << map_itr->second;
				}
			}
			*file << endl;
		}
		*file << "Total";

		for (tab=0; tab < num_tables; tab++) {
			*file << "\t" << org_tot [tab] << "\t" << des_tot [tab];
		}
		*file << endl;
	}
	End_Progress ();
	margin_file.Close ();
}
