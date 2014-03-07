//*********************************************************
//	Stat_Report.cpp - Field Statistic Report
//*********************************************************

#include "FileFormat.hpp"

//---------------------------------------------------------
//	Statistics_Report
//---------------------------------------------------------

void FileFormat::Statistics_Report (void)
{
	int i, j, num_fields, count;
	double minimum, maximum, total, average;
	String type;

	Db_Header *file;
	Field_Ptr field_ptr;
	Data_Itr data_itr;
	Stats_Array *stats_array;
	Field_Stats *stats_ptr;

	Show_Message ("Creating a Field Statitics Report -- Record");
	Set_Progress ();

	Header_Number (STATS_REPORT);

	//---- process each file ----

	for (data_itr = data_group.begin (); data_itr != data_group.end (); data_itr++) {

		for (j=0; j < 2; j++) {
			if (j == 0) {
				file = data_itr->file;
				stats_array = &data_itr->input;
			} else {
				file = data_itr->new_file;
				stats_array = &data_itr->output;
			}
			filename = file->Filename ();
			num_records = stats_array->begin ()->count;

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
					Print (1, String ("%-20.20s %-9.9s %8d %9.1lf %10.1lf %12.1lf %11.1lf") %
						field_ptr->Name () % type.To_Title () % count % minimum % maximum % total % average);
				} else if (field_ptr->Type () == DB_TIME) {
					Print (1, String ("%-20.20s %-9.9s %8d %9.9s %10.10s %12.0lf %11.11s") %
						field_ptr->Name () % type.To_Title () % count % 
						((Dtime) minimum).Time_String () % 
						((Dtime) maximum).Time_String () % (total / 10) % 
						((Dtime) average).Time_String ());
				} else {
					Print (1, String ("%-20.20s %-9.9s %8d %9.0lf %10.0lf %12.0lf %11.1lf") %
						field_ptr->Name () % type.To_Title () % count % minimum % maximum % total % average);
				}
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
	Print (1, "Field Statistics Report");
	Print (2, "File = ") << filename;
	Print (1, "Number of Records = ") << num_records;
	Print (2, "Field                Type         Count   Minimum    Maximum        Total     Average");
	Print (1);
}

/*********************************************|***********************************************

	Field Statistics Report

	File = ssssssssssssssssssssssssssssssssssssssssssssssssssssssss
	Number of Records = ddddddd

	Field                Type         Count   Minimum    Maximum        Total     Average

	ssssssssssssssssssss sssssssss dddddddd  dddddddd  ddddddddd  ddddddddddd  dddddddd.d

**********************************************|***********************************************/ 
