//*********************************************************
//	Write_Ends.cpp - Write a End Time File
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Write_End_Times
//---------------------------------------------------------

void MatrixData::Write_End_Times (void)
{
	int i, j, period, num_periods, tab, num_tables;
	double total, value, row_tot;
	Doubles zeros;
	Dbls_Array des_tot;
	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;
	Str_Map_Itr map_itr;

	fstream *file = &end_file.File();

	Show_Message ("Writing End Time File -- Record");
	Set_Progress ();

	num_periods = new_matrix->Num_Periods ();
	if (num_periods == 0) {
		num_periods = 1;
	}
	org_map = new_matrix->Org_Map ();
	des_map = new_matrix->Des_Map ();
	num_tables = new_matrix->Tables ();

	//---- write the header ----

	*file << "Destination";
	if (num_tables > 1) {
		*file << "\tTable";
	}
	for (period=0; period < num_periods; period++) {
		*file << "\t" << new_matrix->Range_Label (period);
	}
	*file << "\tTotal" << endl;

	zeros.assign (num_periods, 0);
	des_tot.assign (num_tables, zeros);

	for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
		j = des_itr->second;

		for (tab=0; tab < num_tables; tab++) {
			*file << des_itr->first;

			if (num_tables > 1) {
				Db_Field *ptr = new_matrix->Table_Field (tab);
				*file << "\t" << ptr->Name ();
			}
			row_tot = 0;

			for (period=0; period < num_periods; period++) {
				total = 0;

				for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
					Show_Progress ();
					i = org_itr->second;

					new_matrix->Get_Cell_Index (period, i, j, tab, value);
					total += value;
					row_tot += value;
					des_tot [tab] [period] += value;
				}
				*file << (String ("\t%.2lf") % total);
			}
			*file << (String ("\t%.2lf") % row_tot);

			if (equiv_flag) {
				*file << "\t" << zone_equiv.Group_Label (des_itr->first);
			} else if (label_flag) {
				map_itr = label_map.find (des_itr->first);
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
			value = des_tot [tab] [period];
			*file << (String ("\t%.2lf") % value);
			row_tot += value;
		}
		*file << (String ("\t%.2lf") % row_tot) << endl;
	}
	End_Progress ();
	end_file.Close ();
}
