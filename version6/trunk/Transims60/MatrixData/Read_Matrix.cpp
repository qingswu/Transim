//*********************************************************
//	Read_Matrix.cpp - read matrix files into memory
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Read_Matrix
//---------------------------------------------------------

void MatrixData::Read_Matrix (void)
{
	int i, j, in_j, out_i, out_j, n, org, des, period, num_periods, out_period;
	double value, total, select, v, problem, prob, increment;
	bool flag, alt_flag;

	Dtime low, high;
	Matrix_Itr matrix_itr;
	Int_Map *des_map;
	Int_Map_Itr des_itr;
	Dbl_Itr prob_itr;

	i = j = out_i = out_j = 0;
	alt_flag = false;

	for (matrix_itr = matrix_group.begin (); matrix_itr != matrix_group.end (); matrix_itr++) {

		Show_Message (String ("Reading %s -- Record") % matrix_itr->matrix->File_Type ());
		Set_Progress ();

		num_periods = matrix_itr->matrix->Num_Periods ();
		if (num_periods == 0) {
			num_periods = 1;
		}

		//---- process each period ----

		for (period=0; period < num_periods; period++) {
			if (period_flag) {
				matrix_itr->matrix->Period_Range (period, low, high);
				out_period = new_matrix->Period (low, high);
			} else {
				out_period = 0;
			}
			if (out_period < 0) continue;

			//---- process each origin zone ----

			for (org=1; matrix_itr->matrix->Read_Row (org, period); org++) {

				if (matrix_itr->matrix->Org_Index (org) < 0) continue;

				if (select_org_zones && !org_zone_range.In_Range (org)) continue;

				i = (equiv_flag) ? zone_equiv.Zone_Group (org) : org;

				if (matrix_itr->transpose) {
					out_j = i = new_matrix->Add_Des (i);
				} else {
					out_i = i = new_matrix->Add_Org (i);
				}
				if (i < 0) continue;

				des_map = matrix_itr->matrix->Des_Map ();

				//---- select destinations ----

				if (method == SELECT_METHOD) {
					problem = 1.0;

					if (missing_flag) {
						if (random_missing.Probability() <= share_missing) {
							if (!problem_flag) continue;

							prob = random_problem.Probability ();
							n = (int) problem_distribution.size ();

							if (n > 1) {
								increment = 1.0 / (n - 1);
							} else {
								increment = 1.0;
							}
							total = problem = 0.0;

							for (prob_itr = problem_distribution.begin (); prob_itr != problem_distribution.end (); prob_itr++) {
								total += *prob_itr;
								if (prob <= total) break;
								problem += increment;
							}
							if (problem >= 1.0) continue;
							problem = 1.0 - problem;
						}
					}

					for (n=0; n < matrix_itr->matrix->Tables (); n++) {
						for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
							des = des_itr->first;
							in_j = des_itr->second;

							if (select_des_zones && !des_zone_range.In_Range (des)) continue;

							matrix_itr->matrix->Get_Cell_Index (in_j, n, value);
							value *= problem;
							total = 0.0;

							for (v = value; v > 0.0; v -= 1.0) {
								if (v >= 1.0) {
									select = 1.0;
								} else {
									select = v;
								}
								if (random.Probability () <= select * select_percent) {
									total += select;
								}
							}
							matrix_itr->matrix->Set_Cell_Index (in_j, n, total);
						}
					}
				}

				//---- process each destination ----
				
				for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {

					flag = false;
						
					des = des_itr->first;
					in_j = des_itr->second;

					if (select_des_zones && !des_zone_range.In_Range (des)) continue;

					j = (equiv_flag) ? zone_equiv.Zone_Group (des) : des;
					if (j < 0) continue;

					if (matrix_itr->transpose) {
						out_i = j = new_matrix->Add_Org (j);
					} else {
						out_j = j = new_matrix->Add_Des (j);
					}
					if (j < 0) continue;

					//---- user processing script ----

					if (script_flag) {
						matrix_itr->record->Origin (org);
						matrix_itr->record->Destination (des);
						matrix_itr->record->Period (period);

						for (n=0; n < matrix_itr->matrix->Tables (); n++) {
							matrix_itr->matrix->Get_Cell_Index (in_j, n, value);
							value *= matrix_itr->factor;
							matrix_itr->record->Table (n, value);
						}
						new_matrix->Reset_Record ();
						new_matrix->Copy_Fields (*matrix_itr->record);

						if (program.Execute (-1, matrix_itr->group) == 0) continue;

						//---- add the matrix record to array ----
			
						for (n=0; n < new_matrix->Tables (); n++) {
							value = new_matrix->Table (n) / factor;

							if (value != 0) {
								if (alt_flag) {
									alt_matrix->Add_Cell_Index (out_period, out_i, out_j, n, value);
								} else {
									new_matrix->Add_Cell_Index (out_period, out_i, out_j, n, value);
								}
								flag = true;
							}
						}

					} else {
			
						//---- add the matrix record to array ----
			
						for (n=0; n < matrix_itr->matrix->Tables (); n++) {
							if (n >= new_matrix->Tables ()) break;

							matrix_itr->matrix->Get_Cell_Index (in_j, n, value);

							value *= matrix_itr->factor / factor;

							if (value != 0) {
								if (alt_flag) {
									alt_matrix->Add_Cell_Index (out_period, out_i, out_j, n, value);
								} else {
									new_matrix->Add_Cell_Index (out_period, out_i, out_j, n, value);
								}
								flag = true;
							}
						}
					}
					if (flag) {
						Show_Progress ();
					}
				}
			}
		}
		End_Progress ();

		matrix_itr->matrix->Close ();

		alt_flag = (method == RATIO_METHOD);
	}
}
