//*********************************************************
//	Write_Tour.cpp - Write the New Tour File
//*********************************************************

#include "TourData.hpp"

//---------------------------------------------------------
//	Write_Tour
//---------------------------------------------------------

void TourData::Write_Tour ()
{
	int num_in, num_out, num_moved, table;
	int purpose, type, income, org, des, point;
	Table_Itr table_itr;

	num_in = num_out = num_moved = 0;

	Show_Message (String ("Writing %s -- Record") % new_file.File_Type ());
	Set_Progress ();

	while (tour_file.Read ()) {
		Show_Progress ();

		num_in++;
		new_file.Copy_Fields (tour_file);

		org = tour_file.Get_Integer (org_field);
		des = tour_file.Get_Integer (des_field);
		table = 0;

		if (org > 0 && org <= max_zone && des > 0 && des <= max_zone) {
			purpose = tour_file.Get_Integer (purpose_field);
			type = tour_file.Get_Integer (type_field);
			income = tour_file.Get_Integer (income_field);

			for (table_itr = table_groups.begin (); table_itr != table_groups.end (); table_itr++) {
				if (table_itr->purpose.In_Range (purpose) && table_itr->type.In_Range (type) &&
					table_itr->income.In_Range (income)) {

					table = table_itr->group - 1;

					if (Process (org, des, point, table_itr)) {
						num_moved++;
						new_file.Put_Field (des_field, des);
						new_file.Put_Field (point_field, point);
					}
					if (attr_file_flag) {
						table_itr->attr [des]++;
					}
					break;
				}
			}
		}

		//---- save the output fields ----

		if (!new_file.Write ()) {
			Error (String ("Writing %s Record %d") % new_file.File_Type () % num_in);
		}
		num_out++;

		//---- summarize the results ----

		if (result_flag) {
			result_matrix->Add_Cell (0, zone_group [org], zone_group [des], table, 1.0);
		}
	}
	End_Progress ();

	tour_file.Close ();
	new_file.Close ();

	Print (1);
	Write (1, "Number of Tour Records Read = ") << num_in;
	Write (1, "Number of Tour Records Written = ") << num_out;
	Write (1, "Number of Tour Records Moved = ") << num_moved;
}

