//*********************************************************
//	Get_Performance_Data.cpp - read the performance file
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Get_Performance_Data
//---------------------------------------------------------

bool PerfPrep::Get_Performance_Data (Performance_File &file, Performance_Data &perf_rec)
{
	bool flag = Data_Service::Get_Performance_Data (file, perf_rec);

	if (!flag && deleted_flag) {
		if (Report_Flag (DELETED_RECORDS)) {
			if (first_delete) {
				first_delete = false;
				Header_Number (DELETED_RECORDS);
				New_Page ();
			}
			Deleted_Report (file);
		}
		if (del_file_flag) {
			deleted_file.Copy_Fields (file);
			deleted_file.Write ();
		}
	}
	return (flag);
}
