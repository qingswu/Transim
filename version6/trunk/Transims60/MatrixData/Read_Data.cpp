//*********************************************************
//	Read_Data.cpp - read data files
//*********************************************************

#include "MatrixData.hpp"

//---------------------------------------------------------
//	Read_Data
//---------------------------------------------------------

void MatrixData::Read_Data (void)
{
	int i, org, des, period, num_periods;
	double value;
	Dtime low, high;
	String key;

	Data_Itr data_itr;
	Int_Itr int_itr;
	Field_Stats field_stats, *stats_ptr;
	Field_Ptr fld_ptr;

	field_stats.count = 0;
	field_stats.total = 0;
	field_stats.minimum = MAX_INTEGER;
	field_stats.maximum = -MAX_INTEGER;

	num_periods = new_matrix->Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	//---- process each file ----

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {

		Show_Message (String ("Reading %s -- Record") % data_itr->file->File_Type ());
		Set_Progress ();

		if (stats_flag) {
			data_itr->stats.assign (data_itr->file->Num_Fields (), field_stats);
		}

		while (data_itr->file->Read ()) {
			Show_Progress ();

			if (stats_flag) {
				for (i=0; i < data_itr->file->Num_Fields (); i++) {
					fld_ptr = data_itr->file->Field (i);

					stats_ptr = &data_itr->stats [i];

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

			//---- set matrix index ----

			org = data_itr->file->Get_Integer (data_itr->org_field);
			des = data_itr->file->Get_Integer (data_itr->des_field);
				
			if (select_org_zones && !org_zone_range.In_Range (org)) continue;
			if (select_des_zones && !des_zone_range.In_Range (des)) continue;
			if (percent_flag && random.Probability () > select_percent) continue;

			if (data_itr->period_flag) {
				period = data_itr->file->Get_Integer (data_itr->period_field);
			} else if (data_itr->time_flag) {
				low = data_itr->file->Get_Time (data_itr->low_field);
				high = data_itr->file->Get_Time (data_itr->high_field);
				period = new_matrix->Period (low, high);
				if (period < 0) continue;
			} else {
				period = data_itr->period;
			}

			//---- user processing script ----

			if (script_flag) {
				new_matrix->Origin (org);
				new_matrix->Destination (des);
				new_matrix->Period (period);

				for (i=0, int_itr = data_itr->table_fields.begin (); int_itr != data_itr->table_fields.end (); int_itr++, i++) {
					if (i >= new_matrix->Tables ()) break;
					fld_ptr = data_itr->file->Field (*int_itr);

					if (fld_ptr->Type () != DB_STRING && fld_ptr->Type () != DB_CHAR) {
						new_matrix->Table (i, data_itr->file->Get_Double (*int_itr));
					}
				}
				if (program.Execute (-1, data_itr->group) == 0) continue;

				period = new_matrix->Period ();
				if (period < 0 || period >= num_periods) continue;

				org = new_matrix->Origin ();
				if (Zone_Equiv_Flag ()) {
					org = zone_equiv.Zone_Group (org);
				}
				org = new_matrix->Add_Org (org);
				if (org < 0) continue;

				des = new_matrix->Destination ();
				if (Zone_Equiv_Flag ()) {
					des = zone_equiv.Zone_Group (des);
				}
				des = new_matrix->Add_Des (des);
				if (des < 0) continue;

				for (i=0; i < new_matrix->Tables (); i++) {
					value = new_matrix->Table (i) / factor;

					new_matrix->Add_Cell_Index (period, org, des, i, value);
				}

			} else {
			
				//---- add the matrix record to array ----

				if (period < 0 || period >= num_periods) continue;

				if (Zone_Equiv_Flag ()) {
					org = zone_equiv.Zone_Group (org);
				}
				org = new_matrix->Add_Org (org);
				if (org < 0) continue;
	
				if (Zone_Equiv_Flag ()) {
					des = zone_equiv.Zone_Group (des);
				}
				des = new_matrix->Add_Des (des);
				if (des < 0) continue;
				
				for (i=0, int_itr = data_itr->table_fields.begin (); int_itr != data_itr->table_fields.end (); int_itr++, i++) {
					if (i >= new_matrix->Tables ()) break;

					value = data_itr->file->Get_Double (*int_itr) / factor;
					new_matrix->Add_Cell_Index (period, org, des, i, value);
				}
			}
		}
		End_Progress ();

		data_itr->file->Close ();

		//---- write summary summary ----

		if (data_itr != data_group.begin ()) Print (1);
		Print (1, String ("Number of %s Records = %d") % data_itr->file->File_Type () % Progress_Count ());
	}
}
