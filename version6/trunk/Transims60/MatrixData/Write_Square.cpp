//*********************************************************
//	Write_Square.cpp - Write a Square Table File
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Write_Square_Table
//---------------------------------------------------------

void MatrixData::Write_Square_Table (void)
{
	int i, j, period, num_periods, org, des, tab, num_tables;
	double total, value;
	Doubles des_tot;
	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;
	String period_format;
	Str_Map_Itr map_itr;

	fstream *file = &square_file.File();

	Show_Message ("Writing Square Table File -- Record");
	Set_Progress ();

	num_periods = new_matrix->Num_Periods ();
	if (num_periods == 0) {
		num_periods = 1;
	}
	org_map = new_matrix->Org_Map ();
	des_map = new_matrix->Des_Map ();
	num_tables = new_matrix->Tables ();

	for (tab=0; tab < num_tables; tab++) {
		*file << "Table=\t" << new_matrix->Table_Field (tab)->Name () << endl;

		for (period=0; period < num_periods; period++) {
			if (period_flag) {
				if (new_matrix->Merge_Flag ()) {
					*file << "Period=\t" << new_matrix->Range_String () << endl;
				} else {
					*file << "Period=\t" << new_matrix->Range_Format (period) << endl;
				}
			}
			if (Notes_Name_Flag ()) {
				*file << "\t";

				for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
					des = des_itr->first;
					*file << "\t";

					if (Zone_Equiv_Flag ()) {
						*file << zone_equiv.Group_Label (des);
					} else if (label_flag) {
						map_itr = label_map.find (des);
						if (map_itr != label_map.end ()) {
							*file << map_itr->second;
						}
					}
				}
				*file << endl << "\t";
			}
			*file << "Group";

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				*file << (String ("\t%d") % des_itr->first);
			}
			*file << "\tTotal" << endl;

			des_tot.assign (des_map->size (), 0);

			for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
				Show_Progress ();

				org = org_itr->first;
				i = org_itr->second;
				total = 0;

				if (Notes_Name_Flag ()) {
					if (Zone_Equiv_Flag ()) {
						*file << zone_equiv.Group_Label (org);
					} else if (label_flag) {
						map_itr = label_map.find (org);
						if (map_itr != label_map.end ()) {
							*file << map_itr->second;
						}
					}
					*file << "\t";
				}
				*file << org_itr->first;

				for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
					j = des_itr->second;

					new_matrix->Get_Cell_Index (period, i, j, tab, value);

					*file << "\t" << value;
					total += value;
					des_tot [j] += value;
				}
				*file << "\t" << total;

				if (Zone_Equiv_Flag ()) {
					*file << "\t" << zone_equiv.Group_Label (org);
				} else if (label_flag) {
					map_itr = label_map.find (org);
					if (map_itr != label_map.end ()) {
						*file << "\t" << map_itr->second;
					}
				}
				*file << endl;
			}
			if (Notes_Name_Flag ()) {
				*file << "\t";
			}
			*file << "Total";
			total = 0;

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				j = des_itr->second;

				value = des_tot [j];
				*file << "\t" << value;
				total += value;
			}
			*file << "\t" << total << endl;
		}
	}
	End_Progress ();
	square_file.Close ();
}
