//*********************************************************
//	Merge_Data.cpp - merge standard data files
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Merge_Data
//---------------------------------------------------------

void FileFormat::Merge_Data (void)
{
	int i, n, num_in, num_out, index;
	double value;
	bool flag;
	String key;
	Str_Itr str_itr;
	Int_Itr int_itr;

	Data_Itr data_itr;
	Str_ID_Itr id_itr;
	Field_Stats field_stats, *stats_ptr;
	Field_Ptr fld_ptr;
	Int_Set index_set;
	Int_Set_Itr set_itr;

	field_stats.count = 0;
	field_stats.total = 0;
	field_stats.minimum = MAX_INTEGER;
	field_stats.maximum = -MAX_INTEGER;

	//---- process each file ----

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {

		Show_Message (String ("Reading %s -- Record") % data_itr->file->File_Type ());
		Set_Progress ();

		num_in = num_out = 0;

		if (stats_flag) {
			data_itr->input.assign (data_itr->file->Num_Fields (), field_stats);
		}

		while (data_itr->file->Read (false)) {
			Show_Progress ();
			num_in++;

			//---- select records ----

			if (data_itr->select_flag) {
				flag = false;
				if (data_itr->int_flag) {
					n = data_itr->file->Get_Integer (data_itr->sel_field);

					for (int_itr = data_itr->sel_values.begin (); int_itr != data_itr->sel_values.end (); int_itr++) {
						if (n == *int_itr) {
							flag = true;
							break;
						}
					}
				} else {
					key = data_itr->file->Get_String (data_itr->sel_field);

					for (str_itr = data_itr->sel_strings.begin (); str_itr != data_itr->sel_strings.end (); str_itr++) {
						if (key.Equals (*str_itr)) {
							flag = true;
							break;
						}
					}
				}
				if (!flag) continue;
			}

			//---- store the indexed record ----

			index = data_itr->file->Get_Integer (data_itr->index_field);

			data_itr->index->Copy_Fields (*(data_itr->file));
			data_itr->index->Write_Record (index);
			num_out++;

			index_set.insert (index);

			//---- sum field statistics ----

			if (stats_flag) {
				for (i=0; i < data_itr->file->Num_Fields (); i++) {
					fld_ptr = data_itr->file->Field (i);
					if (fld_ptr->Nested ()) continue;

					stats_ptr = &data_itr->input [i];

					if (fld_ptr->Type () != DB_STRING && fld_ptr->Type () != DB_CHAR) {
						stats_ptr->count++;
						value = data_itr->file->Get_Double (i);

						if (value < stats_ptr->minimum) {
							stats_ptr->minimum = value;
						}
						if (value > stats_ptr->maximum) {
							stats_ptr->maximum = value;
						}
						stats_ptr->total += value;
					} else {
						stats_ptr->count++;
						stats_ptr->minimum = 0;
						stats_ptr->maximum = 0;
					}
				}
			}
		}
		End_Progress ();

		Write (2, "Number of Input Data Records  = ") << num_in;
		Write (1, "Number of Selected Records    = ") << num_out;
	}

	//---- write the merged file records ----

	Show_Message (String ("Writing %s -- Record") % combine_file.File_Type ());
	Set_Progress ();

	num_out = 0;
	output.assign (merge_file.Num_Fields (), field_stats);

	for (set_itr = index_set.begin (); set_itr != index_set.end (); set_itr++) {
		Show_Progress ();

		for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
			data_itr->index->Read_Record (*set_itr);
		}

		//---- write summary summary ----

		if (!script_flag || program.Execute (*set_itr) != 0) {
			num_out++;

			merge_file.Write (false);

			if (stats_flag) {
				for (i=0; i < merge_file.Num_Fields (); i++) {
					fld_ptr = merge_file.Field (i);
					if (fld_ptr->Nested ()) continue;

					stats_ptr = &output [i];

					if (fld_ptr->Type () != DB_STRING && fld_ptr->Type () != DB_CHAR) {
						stats_ptr->count++;
						value = merge_file.Get_Double (i);

						if (value < stats_ptr->minimum) {
							stats_ptr->minimum = value;
						}
						if (value > stats_ptr->maximum) {
							stats_ptr->maximum = value;
						}
						stats_ptr->total += value;
					} else {
						stats_ptr->count++;
						stats_ptr->minimum = 0;
						stats_ptr->maximum = 0;
					}
				}
			}
		}
	}
	End_Progress ();
	merge_file.Close ();

	Write (2, "Number of Output Data Records = ") << num_out;
}
