//*********************************************************
//	Write_Summary.cpp - Write the New Summary File
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Write_Summary
//---------------------------------------------------------

void GridData::Write_Summary ()
{
	int i, j, count;

	Summary_Itr sum_itr;
	Range_Array_Itr range_itr;
	Dbls_Itr dbls_itr;
	Dbl_Itr data_itr;

	count = 0;

	Show_Message (2, String ("Writing %s -- Record") % summary_file.File_Type ());
	Set_Progress ();

	for (sum_itr = summary_array.begin (); sum_itr != summary_array.end (); sum_itr++) {
		Show_Progress ();

		summary_file.Put_Field (0, sum_itr->group);
		summary_file.Put_Field (1, sum_itr->field_name);

		dbls_itr = sum_itr->data_array.begin ();

		for (i=0, range_itr = sum_itr->select_values.begin (); range_itr != sum_itr->select_values.end (); range_itr++, dbls_itr++, i++) {
			summary_file.Put_Field (2, range_itr->Low ());
			summary_file.Put_Field (3, range_itr->High ());
			summary_file.Put_Field (4, sum_itr->grid_counts [i]);

			for (j=data_offset, data_itr = dbls_itr->begin (); data_itr != dbls_itr->end (); data_itr++, j++) {
				summary_file.Put_Field (j, *data_itr);
			}
			if (!summary_file.Write ()) {
				Error (String ("Writing %s") % summary_file.File_Type ());
			}
			count++;
		}
	}
	End_Progress ();

	summary_file.Close ();

	Print (1);
	Write (1, "Number of Summary Records Written = ") << count;
}
