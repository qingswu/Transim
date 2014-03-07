//*********************************************************
//	Write_Starts.cpp - Write a Start Time File
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Write_Start_Times
//---------------------------------------------------------

void MatrixData::Write_Start_Times (void)
{
	int i, j, period, num_periods, tab, num_tables;
	double total, value, row_tot;
	Doubles zeros;
	Dbls_Array org_tot;
	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;
	Str_Map_Itr map_itr;

	fstream *file = &start_file.File();

	Show_Message ("Writing Start Time File -- Record");
	Set_Progress ();

	num_periods = new_matrix->Num_Periods ();
	if (num_periods == 0) {
		num_periods = 1;
	}
	org_map = new_matrix->Org_Map ();
	des_map = new_matrix->Des_Map ();
	num_tables = new_matrix->Tables ();

	//---- write the header ----

	*file << "Origin";
	if (num_tables > 1) {
		*file << "\tTable";
	}
	for (period=0; period < num_periods; period++) {
		*file << "\t" << new_matrix->Range_Label (period);
	}
	*file << "\tTotal" << endl;

	zeros.assign (num_periods, 0);
	org_tot.assign (num_tables, zeros);

	for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
		i = org_itr->second;

		for (tab=0; tab < num_tables; tab++) {
			*file << org_itr->first;

			if (num_tables > 1) {
				Db_Field *ptr = new_matrix->Table_Field (tab);
				*file << "\t" << ptr->Name ();
			}
			row_tot = 0;

			for (period=0; period < num_periods; period++) {
				total = 0;

				for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
					Show_Progress ();
					j = des_itr->second;

					new_matrix->Get_Cell_Index (period, i, j, tab, value);
					total += value;
					row_tot += value;
					org_tot [tab] [period] += value;
				}
				*file << (String ("\t%.2lf") % total);
			}
			*file << (String ("\t%.2lf") % row_tot);

			if (equiv_flag) {
				*file << "\t" << zone_equiv.Group_Label (org_itr->first);
			} else if (label_flag) {
				map_itr = label_map.find (org_itr->first);
				if (map_itr != label_map.end ()) {
					*file << "\t" << map_itr->second;
				}
			}
			*file << endl;
		}
	}

	//---- totals ----

	for (tab=0; tab < num_tables; tab++) {
		*file << "Total";
		row_tot = 0;

		if (num_tables > 1) {
			Db_Field *ptr = new_matrix->Table_Field (tab);
			*file << "\t" << ptr->Name ();
		}

		for (period=0; period < num_periods; period++) {
			value = org_tot [tab] [period];
			*file << (String ("\t%.2lf") % value);
			row_tot += value;
		}
		*file << (String ("\t%.2lf") % row_tot) << endl;
	}
	End_Progress ();
	start_file.Close ();
}
