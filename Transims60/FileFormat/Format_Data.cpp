//*********************************************************
//	Format_Data.cpp - convert standard data files
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Format_Data
//---------------------------------------------------------

void FileFormat::Format_Data (void)
{
	int i, n, num, num_in, num_out;
	double value;
	bool flag;
	String key;
	Str_Itr str_itr;
	Int_Itr int_itr;

	Data_Itr data_itr;
	Str_ID_Itr id_itr;
	Field_Stats field_stats, *stats_ptr;
	Field_Ptr fld_ptr;

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
			data_itr->output.assign (data_itr->new_file->Num_Fields (), field_stats);
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

			//---- sort or copy fields ----

			if (data_itr->sort_flag) {
				data_itr->dbase->Copy_Fields (*data_itr->file);
			} else {
				data_itr->new_file->Copy_Fields (*data_itr->file, false);
			}

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

			if (!script_flag || program.Execute (-1, data_itr->group) != 0) {
				num_out++;

				if (data_itr->sort_flag) {
					data_itr->dbase->Write_Record ();

					data_itr->sort_string->Copy_Fields (*data_itr->dbase);
					key = data_itr->sort_string->Record_String ();

					data_itr->sort_id.insert (Str_ID_Data (key, num_out));
				} else {
					data_itr->new_file->Write (false);

					if (stats_flag) {
						for (i=0; i < data_itr->new_file->Num_Fields (); i++) {
							fld_ptr = data_itr->new_file->Field (i);
							if (fld_ptr->Nested ()) continue;

							stats_ptr = &data_itr->output [i];

							if (fld_ptr->Type () != DB_STRING && fld_ptr->Type () != DB_CHAR) {
								stats_ptr->count++;
								value = data_itr->new_file->Get_Double (i);

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

			if (data_itr->file->Nest_Flag ()) {
				num = data_itr->file->Num_Nest ();

				for (n=0; n < num; n++) {
					Show_Progress ();
					num_in++;

					if (!data_itr->file->Read (true)) {
						Error ("Reading Nested Record");
					}
					if (stats_flag) {
						for (i=0; i < data_itr->file->Num_Fields (); i++) {
							fld_ptr = data_itr->file->Field (i);
							if (!fld_ptr->Nested ()) continue;

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
					data_itr->new_file->Copy_Fields (*data_itr->file, true);

					if (!script_flag || program.Execute (-1, data_itr->group) != 0) {
						data_itr->new_file->Write (true);
						num_out++;

						if (stats_flag) {
							for (i=0; i < data_itr->new_file->Num_Fields (); i++) {
								fld_ptr = data_itr->new_file->Field (i);
								if (!fld_ptr->Nested ()) continue;

								stats_ptr = &data_itr->output [i];

								if (fld_ptr->Type () != DB_STRING && fld_ptr->Type () != DB_CHAR) {
									stats_ptr->count++;
									value = data_itr->new_file->Get_Double (i);

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
			}
		}
		End_Progress ();

		//---- write the sorted records ----

		if (data_itr->sort_flag) {
			for (id_itr = data_itr->sort_id.begin (); id_itr != data_itr->sort_id.end (); id_itr++) {
				data_itr->dbase->Read_Record (id_itr->second);
				data_itr->new_file->Copy_Fields (*data_itr->dbase);
				data_itr->new_file->Write (false);

				if (stats_flag) {
					for (i=0; i < data_itr->new_file->Num_Fields (); i++) {
						fld_ptr = data_itr->new_file->Field (i);
						stats_ptr = &data_itr->output [i];

						if (fld_ptr->Type () != DB_STRING && fld_ptr->Type () != DB_CHAR) {
							stats_ptr->count++;
							value = data_itr->new_file->Get_Double (i);

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
			data_itr->sort_id.clear ();
			data_itr->dbase->clear ();
		}
		data_itr->file->Close ();
		data_itr->new_file->Close ();

		//---- write summary summary ----

		if (data_itr != data_group.begin ()) Print (1);
		Break_Check (2);
		Print (1, String ("Number of %s Records = %d") % data_itr->file->File_Type () % num_in);
		Write (1, String ("Number of %s Records = %d") % data_itr->new_file->File_Type () % num_out);
	}
}
