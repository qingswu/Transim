//*********************************************************
//	Sum_Grid.cpp - summarize the grid data
//*********************************************************

#include "GridData.hpp"

//---------------------------------------------------------
//	Sum_Grid
//---------------------------------------------------------

void GridData::Sum_Grid ()
{
	int select, index;
	double filter, value;

	Summary_Itr sum_itr;
	Range_Array_Itr range_itr;
	Doubles *doubles;
	Dbl_Itr data_itr;
	Data_Field_Itr field_itr;

	for (sum_itr = summary_array.begin (); sum_itr != summary_array.end (); sum_itr++) {
		Show_Progress ();

		select = out_file.Get_Integer (sum_itr->select_field);

		index = sum_itr->select_values.In_Index (select);
		if (index < 0) continue;

		sum_itr->grid_counts [index]++;
		doubles = &sum_itr->data_array [index];
		data_itr = doubles->begin ();

		for (field_itr = data_field_array.begin (); field_itr != data_field_array.end (); field_itr++, data_itr++) {
			filter = out_file.Get_Double (field_itr->filter_field);

			switch (field_itr->filter_test) {
				case 0:	//---- equals ----
					if (filter != field_itr->filter_value) continue;
					break;
				case 1:	//---- greater than ----
					if (filter <= field_itr->filter_value) continue;
					break;
				case 2:	//---- greather than or equal to ----
					if (filter < field_itr->filter_value) continue;
					break;
				case 3:	//---- not equal ----
					if (filter == field_itr->filter_value) continue;
					break;
				case 4:	//---- less than ----
					if (filter >= field_itr->filter_value) continue;
					break;
				case 5:	//---- less than or equal to ----
					if (filter > field_itr->filter_value) continue;
					break;
				default:
					break;
			}

			//---- process the second filter ----

			if (field_itr->filter2_field >= 0) {
				filter = out_file.Get_Double (field_itr->filter2_field);

				switch (field_itr->filter2_test) {
					case 0:	//---- equals ----
						if (filter != field_itr->filter2_value) continue;
						break;
					case 1:	//---- greater than ----
						if (filter <= field_itr->filter2_value) continue;
						break;
					case 2:	//---- greather than or equal to ----
						if (filter < field_itr->filter2_value) continue;
						break;
					case 3:	//---- not equal ----
						if (filter == field_itr->filter2_value) continue;
						break;
					case 4:	//---- less than ----
						if (filter >= field_itr->filter2_value) continue;
						break;
					case 5:	//---- less than or equal to ----
						if (filter > field_itr->filter2_value) continue;
						break;
					default:
						break;
				}
			}
			if (field_itr->data_value >= 0) {
				value = out_file.Get_Double (field_itr->data_value);
			} else {
				value = 1;
			}
			(*data_itr) += value;
		}
	}
}
