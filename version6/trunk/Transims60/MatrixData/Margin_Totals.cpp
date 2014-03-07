//*********************************************************
//	Margin_Totals.cpp - Row / Column Totals Report
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Margin_Total_Report
//---------------------------------------------------------

void MatrixData::Margin_Total_Report (void)
{
	int i, j, period, num_periods, zone, max_zone, tab, num_tables;
	double total, dvalue;
	bool flag;
	Doubles org_sum, des_sum, org_tot, des_tot, org_total, des_total;
	Int_Map *org_map, *des_map;
	Int_Map_Itr org_itr, des_itr;
	String period_format;
	Str_Map_Itr map_itr;

	Show_Message ("Creating a Margin Totals Report -- Record");
	Set_Progress ();

	Header_Number (MARGIN_REPORT);

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

	if (!Break_Check (num_periods * max_zone + 7)) {
		Print (1);
		Margin_Total_Header ();
	}
	org_total.assign (num_tables, 0);
	des_total.assign (num_tables, 0);

	for (period=0; period < num_periods; period++) {
		if (period_flag) {
			period_format = new_matrix->Range_Format (period);
		}
		if (period > 0) {
			Print (1);
		}
		org_tot.assign (num_tables, 0);
		des_tot.assign (num_tables, 0);

		for (zone=1; zone <= max_zone; zone++) {
			Show_Progress ();

			i = new_matrix->Org_Index (zone);
			j = new_matrix->Des_Index (zone);

			if (i < 0 && j < 0) continue;

			org_sum.assign (num_tables, 0);
			flag = false;

			if (i >= 0) {
				for (tab=0; tab < num_tables; tab++) {
					total = 0;

					for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
						new_matrix->Get_Cell_Index (period, i, des_itr->second, tab, dvalue);
						total += dvalue;
					}
					if (total > 0) {
						flag = true;
						org_sum [tab] = total;
					}
				}
			}

			des_sum.assign (num_tables, 0);

			if (j >= 0) {
				for (tab=0; tab < num_tables; tab++) {
					total = 0;

					for (org_itr = org_map->begin (); org_itr != org_map->end (); org_itr++) {
						new_matrix->Get_Cell_Index (period, org_itr->second, j, tab, dvalue);
						total += dvalue;
					}
					if (total > 0) {
						flag = true;
						des_sum [tab] = total;
					}
				}
			}
			if (!flag) continue;

			if (period_flag) {
				Print (1, String ("%14.14s %5d") % period_format % zone);
			} else {
				Print (1, String ("%5d") % zone);
			}
			for (tab=0; tab < num_tables; tab++) {
				Print (0, String ("  %8.0lf %8.0lf") % org_sum [tab] % des_sum [tab]);

				org_tot [tab] += org_sum [tab];
				des_tot [tab] += des_sum [tab];
			}
			if (Zone_Equiv_Flag ()) {
				Print (0, String ("  %s") % zone_equiv.Group_Label (zone));
			} else if (label_flag) {
				map_itr = label_map.find (zone);
				if (map_itr != label_map.end ()) {
					Print (0, String ("  %s") % map_itr->second);
				}
			}
		}
		if (period_flag) {
			Print (2, String ("%14.14s Total") % period_format);
		} else {
			Print (2, "Total");
		}
		for (tab=0; tab < num_tables; tab++) {
			Print (0, String ("  %8.0lf %8.0lf") % org_tot [tab] % des_tot [tab]);
			org_total [tab] += org_tot [tab];
			des_total [tab] += des_tot [tab];
		}
	}
	if (period_flag) {
		Print (2, String ("%14.14s Total") % "Daily");

		for (tab=0; tab < num_tables; tab++) {
			Print (0, String ("  %8.0lf %8.0lf") % org_total [tab] % des_total [tab]);
		}
	}
	End_Progress ();
	Header_Number (0);
}

//---------------------------------------------------------
//	Margin_Totals_Header
//---------------------------------------------------------

void MatrixData::Margin_Total_Header (void)
{
	Print (1, "Margin Totals Report");

	if (period_flag) {
		Print (2, "                    ");
	} else {
		Print (2, "     ");
	}	
	for (int i=0; i < new_matrix->Tables (); i++) {
		Db_Field *ptr = new_matrix->Table_Field (i);

		if (ptr->Name ().length () <= 17) {
			String header = ptr->Name ();
			Print (0, String ("  %17.17s") % header.Header (17));
		} else {
			Print (0, String ("  ---- Table %d ----") % i);	
		}
	}
	if (period_flag) {
		Print (1, "       Period  Group");
	} else {
		Print (1, "Group");
	}

	for (int i=0; i < new_matrix->Tables (); i++) {
		Print (0, "       Org      Des");
	}
	Print (1);
}

/*********************************************|***********************************************

	Margin Totals Report

	                      ---- Table 1-----  ---- table 2-----  ---- table 3-----  ---- table 4-----
	       Period  Group       Org      Des       Org      Des       Org      Des       Org      Des

	ssssssssssssss ddddd  dddddddd dddddddd  dddddddd dddddddd  dddddddd dddddddd  dddddddd dddddddd

	       ---- table 1-----  ---- table 2-----  ---- table 3-----  ---- table 4-----
	 Zone       Org      Des       Org      Des       Org      Des       Org      Des

	ddddd  dddddddd dddddddd  dddddddd dddddddd  dddddddd dddddddd  dddddddd dddddddd

**********************************************|***********************************************/ 
