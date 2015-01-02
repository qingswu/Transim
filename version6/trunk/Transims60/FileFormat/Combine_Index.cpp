//*********************************************************
//	Combine_Index.cpp - merge data files by index
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Combine_Index
//---------------------------------------------------------

void FileFormat::Combine_Index (void)
{
	int i, n, in, out, num, num_in, num_out, index;
	String key;
	bool flag, stat;

	Int_Itr int_itr;
	Str_Itr str_itr;
	Data_Itr data_itr;
	Db_Sort_Array db_array;
	Db_Sort_Itr db_itr;

	//---- process a record from each file ----

	Show_Message ("Reading Data Files -- Record");
	Set_Progress ();

	num = num_in = num_out = 0;
	db_array.Replicate_Fields (&combine_file);

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {
		index = 0;

		while (data_itr->file->Read ()) {
			Show_Progress ();
			stat = true;
			num_in++;
			index++;

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

			//---- index field ----

			if (data_itr->index_field >= 0) {
				index = data_itr->file->Get_Integer (data_itr->index_field);
			}

			if (!db_array.Read_Record (index)) {
				db_array.Copy_Fields (combine_file);
				db_array.Write_Record (index);
			}

			//---- process each field ----

			for (i=0, int_itr = data_itr->field_types.begin (); int_itr != data_itr->field_types.end (); int_itr++, i++) {
				in = data_itr->in_fields [i];
				out = data_itr->out_fields [i];

				switch (*int_itr) {
					case DB_INTEGER:
					case DB_UNSIGNED:
					case DB_SHORT:
					default:
						db_array.Put_Field (out, data_itr->file->Get_Integer (in));
						break;
					case DB_DOUBLE:
					case DB_FIXED:
					case DB_FLOAT:
						db_array.Put_Field (out, data_itr->file->Get_Double (in));
						break;
					case DB_STRING:
					case DB_CHAR:
						db_array.Put_Field (out, data_itr->file->Get_String (in));
						break;
					case DB_TIME:
						db_array.Put_Field (out, data_itr->file->Get_Time (in));
						break;
				}
				num_out++;
			}
			db_array.Write_Record (index);
		}
	}
	End_Progress ();

	//---- write the combine file records ----

	Show_Message (String ("Writing %s -- Record") % combine_file.File_Type ());
	Set_Progress ();

	for (db_itr = db_array.begin (); db_itr != db_array.end (); db_itr++) {
		Show_Progress ();
		db_array.Read_Record (db_itr->first);
		combine_file.Copy_Fields (db_array);
		combine_file.Write ();
		num++;
	}
	End_Progress ();
	combine_file.Close ();

	//---- write summary summary ----

	Write (2, "Number of Input Data Records  = ") << num_in;
	Write (1, "Number of Output Data Fields  = ") << num_out;
	Write (1, "Number of Output Data Records = ") << num;
}
