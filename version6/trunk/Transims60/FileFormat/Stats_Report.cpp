//*********************************************************
//	Stat_Report.cpp - Field Statistic Report
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Statistics_Report
//---------------------------------------------------------

void FileFormat::Statistics_Report (void)
{
	int i, j, j2, num_fields, count;
	double minimum, maximum, total, average, time_factor;
	String type;
	Units_Type time_format;

	Db_Header *file;
	Field_Ptr field_ptr;
	Data_Itr data_itr;
	Stats_Array *stats_array;
	Field_Stats *stats_ptr;

	Show_Message ("Creating a Field Statitics Report -- Record");
	Set_Progress ();

	Header_Number (STATS_REPORT);

	time_format = Time_Format ();

	if (time_format == MINUTES) {
		time_factor = 600.0;
	} else if (time_format == SECONDS) {
		time_factor = 10.0;
	} else {
		time_factor = 36000.0;
	}

	//---- process each file ----

	if (merge_flag) {
		j2 = 1;
	} else {
		j2 = 2;
	}

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {

		for (j=0; j < j2; j++) {
			if (j == 0) {
				file = data_itr->file;
				stats_array = &data_itr->input;
			} else {
				if (data_itr->new_file == 0) continue;
				file = data_itr->new_file;
				stats_array = &data_itr->output;
			}
			filename = file->Filename ();
			num_records = stats_array->begin ()->count;
			group_num = data_itr->group;

			num_fields = file->Num_Fields ();

			if (!Break_Check (num_fields + 7)) {
				Print (1);
				Statistics_Header ();
			}

			for (i=0; i < num_fields; i++) {
				Show_Progress ();

				field_ptr = file->Field (i);
				type = Field_Code (field_ptr->Type ());

				stats_ptr = &stats_array->at (i);

				count = stats_ptr->count;
				minimum = stats_ptr->minimum;
				maximum = stats_ptr->maximum;
				total = stats_ptr->total;

				if (count > 0) {
					average = total / count;
				} else {
					average = 0;
				}
				if (field_ptr->Type () == DB_DOUBLE || field_ptr->Type () == DB_FIXED) {
					Print (1, String ("%-20.20s %-9.9s %8d %11.1lf %12.1lf %13.1lf %11.1lf") %
						field_ptr->Name () % type.To_Title () % count % minimum % maximum % total % average);
				} else if (field_ptr->Type () == DB_TIME) {
					Print (1, String ("%-20.20s %-9.9s %8d %11.11s %12.12s %13.0lf %11.11s") %
						field_ptr->Name () % type.To_Title () % count % 
						((Dtime) minimum).Time_String () % 
						((Dtime) maximum).Time_String () % (total / time_factor) % 
						((Dtime) average).Time_String ());
				} else {
					Print (1, String ("%-20.20s %-9.9s %8d %11.0lf %12.0lf %13.0lf %11.1lf") %
						field_ptr->Name () % type.To_Title () % count % minimum % maximum % total % average);
				}
			}
		}
	}

	if (merge_flag) {
		file = &merge_file;
		stats_array = &output;

		filename = file->Filename ();
		num_records = stats_array->begin ()->count;
		group_num = 0;

		num_fields = file->Num_Fields ();

		if (!Break_Check (num_fields + 7)) {
			Print (1);
			Statistics_Header ();
		}

		for (i=0; i < num_fields; i++) {
			Show_Progress ();

			field_ptr = file->Field (i);
			type = Field_Code (field_ptr->Type ());

			stats_ptr = &stats_array->at (i);

			count = stats_ptr->count;
			minimum = stats_ptr->minimum;
			maximum = stats_ptr->maximum;
			total = stats_ptr->total;

			if (count > 0) {
				average = total / count;
			} else {
				average = 0;
			}
			if (field_ptr->Type () == DB_DOUBLE || field_ptr->Type () == DB_FIXED) {
				Print (1, String ("%-20.20s %-9.9s %8d %11.1lf %12.1lf %13.1lf %11.1lf") %
					field_ptr->Name () % type.To_Title () % count % minimum % maximum % total % average);
			} else if (field_ptr->Type () == DB_TIME) {
				Print (1, String ("%-20.20s %-9.9s %8d %11.11s %12.12s %13.0lf %11.11s") %
					field_ptr->Name () % type.To_Title () % count % 
					((Dtime) minimum).Time_String () % 
					((Dtime) maximum).Time_String () % (total / time_factor) % 
					((Dtime) average).Time_String ());
			} else {
				Print (1, String ("%-20.20s %-9.9s %8d %11.0lf %12.0lf %13.0lf %11.1lf") %
					field_ptr->Name () % type.To_Title () % count % minimum % maximum % total % average);
			}
		}
	}

	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Statistics_Header
//---------------------------------------------------------

void FileFormat::Statistics_Header (void)
{
	Print (1, "Field Statistics Report ") << group_num;
	Print (2, "File = ") << filename;
	Print (1, "Number of Records = ") << num_records;
	Print (2, "Field                Type         Count    Minimum      Maximum         Total     Average");
	Print (1);
}

/*********************************************|***********************************************

	Field Statistics Report

	File = ssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	Number of Records = ddddddd

	Field                Type         Count    Minimum      Maximum         Total     Average

	ssssssssssssssssssss sssssssss dddddddd  dddddddddd  ddddddddddd  dddddddddddd  dddddddd.d

**********************************************|***********************************************/ 
