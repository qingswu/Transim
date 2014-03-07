//*********************************************************
//	Read_Factors.cpp - read growth factor file into memory
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Read_Factors
//---------------------------------------------------------

void MatrixData::Read_Factors (void)
{
	int n, org, des, period, num_periods, fac_period, fac_org, fac_des, fac_table;
	double factor;
	bool fac_period_flag;

	Dtime low, high;
	Matrix_Itr matrix_itr;
	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;

	//---- read the factor matrix into memory ----

	if (!factor_file->Read_Matrix ()) {
		Error ("Reading Factor Matrix");
	}

	num_periods = new_matrix->Num_Periods ();
	if (num_periods == 0) {
		num_periods = 1;
	}
	fac_period_flag = (factor_periods.Num_Periods () > 0);

	org_map = new_matrix->Org_Map ();
	des_map = new_matrix->Des_Map ();
	fac_table = 0;
	factor = 1.0;

	Show_Message ("Apply the Growth Factors -- Record");
	Set_Progress ();

	//---- process each period ----

	for (period=0; period < num_periods; period++) {
		if (fac_period_flag) {
			new_matrix->Period_Range (period, low, high);
			fac_period = factor_periods.Period (low, high);
			if (fac_period >= 0) {
				fac_table = factor_fields [fac_period];
			}
		} else {
			fac_period = fac_table = 0;
		}

		//---- process each origin zone ----

		for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
			org = org_itr->second;
			fac_org = org_itr->first;

			if (Zone_Equiv_Flag ()) {
				fac_org = zone_equiv.Zone_Group (fac_org);
			}

			fac_org = factor_file->Org_Index (fac_org);
			if (fac_org < 0) continue;

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				Show_Progress ();
				des = des_itr->second;
				fac_des = des_itr->first;

				if (Zone_Equiv_Flag ()) {
					fac_des = zone_equiv.Zone_Group (fac_des);
				}
				fac_des = factor_file->Des_Index (fac_des);
				if (fac_des < 0) continue;

				if (!fac_table_flag) {
					factor_file->Get_Cell_Index (0, fac_org, fac_des, fac_table, factor);

					if (factor == 1.0 || factor == 0.0) continue;
				}
				for (n=0; n < new_matrix->Tables (); n++) {
					if (fac_table_flag && n < (int) factor_fields.size ()) {
						fac_table = factor_fields [n];
						factor_file->Get_Cell_Index (0, fac_org, fac_des, fac_table, factor);

						if (factor == 1.0 || factor == 0.0) continue;
					}
					new_matrix->Factor_Cell_Index (period, org, des, n, factor);
				}
			}
		}
	}
	End_Progress ();
}
