//*********************************************************
//	Matrix_Ratios.cpp - read matrices and calculate ratios
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Matrix_Ratios
//---------------------------------------------------------

void MatrixData::Matrix_Ratios (void)
{
	int i, j, n, period, num_periods;
	double value, base;

	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;

	org_map = new_matrix->Org_Map ();
	des_map = new_matrix->Des_Map ();

	num_periods = new_matrix->Num_Periods ();
	if (num_periods < 1) num_periods = 1;

	Show_Message (String ("Building %s -- Record") % new_matrix->File_Type ());
	Set_Progress ();

	for (period=0; period < num_periods; period++) {
		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
			i = org_itr->second;

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				j = des_itr->second;

				Show_Progress ();

				for (n=0; n < new_matrix->Tables (); n++) {
					new_matrix->Get_Cell_Index (period, i, j, n, base);
					alt_matrix->Get_Cell_Index (period, i, j, n, value);

					if (value != 0 && base != 0) {
						value /= base;
						if (value < min_ratio) {
							value = min_ratio;
						} else if (value > max_ratio) {
							value = max_ratio;
						}
						new_matrix->Set_Cell_Index (period, i, j, n, value);
					} else {
						new_matrix->Set_Cell_Index (period, i, j, n, 0.0);
					}
				}
			}
		}
	}
	End_Progress ();
}
