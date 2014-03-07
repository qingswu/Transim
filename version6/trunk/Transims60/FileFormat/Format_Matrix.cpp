//*********************************************************
//	Format_Matrix.cpp - convert matrix files
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Format_Matrix
//---------------------------------------------------------

void FileFormat::Format_Matrix (void)
{
	int n, j, org, des, tables;
	double dvalue;
	Int_Map *des_map;
	Int_Map_Itr des_itr;

	Matrix_Itr matrix_itr;

	for (matrix_itr = matrix_group.begin (); matrix_itr != matrix_group.end (); matrix_itr++) {

		Show_Message (String ("Reading %s -- Zone") % matrix_itr->matrix->File_Type ());
		Set_Progress ();

		tables = matrix_itr->new_matrix->Tables ();

		//---- process each origin zone ----

		for (org=1; matrix_itr->matrix->Read_Record (org); org++) {
			if (matrix_itr->matrix->Org_Index (org) < 0) continue;

			Show_Progress ();

			des_map = matrix_itr->matrix->Des_Map ();

			for (des_itr = des_map->begin (); des_itr != des_map->end (); des_itr++) {
				des = des_itr->first;
				j = des_itr->second;

				for (n=0; n < tables; n++) {
					matrix_itr->matrix->Get_Cell_Index (j, matrix_itr->table_map [n], dvalue);
					matrix_itr->new_matrix->Set_Cell (des, n, dvalue);
				}
				if (script_flag) {
					matrix_itr->matrix->Set_Fields (org, des);
					matrix_itr->new_matrix->Set_Fields (org, des);

					if (program.Execute (-1, matrix_itr->group)) {
						matrix_itr->new_matrix->Get_Fields ();
					} else {
						for (n=0; n < tables; n++) {
							matrix_itr->new_matrix->Set_Cell (des, n, 0.0);
						}
					}
				}
			}
			if (!matrix_itr->new_matrix->Write_Record (org)) {
				Error ("Writing Tables");
			}
		}
		End_Progress ();

		matrix_itr->matrix->Close ();
		matrix_itr->new_matrix->Close ();
	}
}
