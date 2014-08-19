//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "CountSum.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void CountSum::Execute (void)
{
	String filename;
	Data_Itr data_itr;
	Str_ID_Itr day_itr;

	Data_Service::Execute ();	

	if (sum_periods.Num_Periods () > 0) {
		perf_period_array.Initialize (&sum_periods);
	} else {
		perf_period_array.Initialize (&time_periods);
	}
	perf_period_array.Set_Time0 ();

	Node_List ();

	if (signal_map_flag) {
		Read_Signal_Map ();
	}
	if (arc_signal_flag) {
		Read_Signal_Points ();
	}
	if (signal_detect_flag) {
		Read_Signal_Detectors ();
	}
	if (link_map_flag) {
		Read_Link_Map ();
	}
	if (arc_station_flag) {
		Link_Extents ();

		Read_Link_Points ();
	}
	if (new_map_flag) {
		Write_Link_Map ();
	}

	//---- initialize the count day array ----

	if (sum_periods.Num_Periods () > 0) {
		count_day_array.Initialize (&sum_periods, (int) offset_index_map.size ());
	} else {
		count_day_array.Initialize (&time_periods, (int) offset_index_map.size ());
	}

	Read_Data ();

	//---- average all days ----

	Combine_Data ();

	if (System_File_Flag (NEW_PERFORMANCE)) {
		Write_Performance (count_day_array);
	}
	if (link_data_flag) {
		Write_Link_Data (link_data_file, count_day_array);
	}

	//---- output each day ----

	if (day_flag) {
		for (day_itr = day_map.begin (); day_itr != day_map.end (); day_itr++) {

			Combine_Data (AVERAGE, day_itr->second);

			if (System_File_Flag (NEW_PERFORMANCE)) {
				Performance_File *file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);

				filename ("%s_%s.%s") % perf_name % day_itr->first % perf_ext;
				file->File_Type (String ("New Performance File %s") % day_itr->first);

				file->Create (filename);

				Write_Performance (count_day_array);
			}
			if (link_data_flag) {
				filename ("%s_%s.%s") % link_data_name % day_itr->first % link_data_ext;
				link_data_file.File_Type (String ("New Link Data File %s") % day_itr->first);

				link_data_file.Create (filename);
				link_data_file.Write_Header ();

				Write_Link_Data (link_data_file, count_day_array);
			}
		}
	}

	//---- output min/max days ---

	if (min_max_flag) {

		//---- minimum counts ----

		Combine_Data (MINIMUM);

		if (System_File_Flag (NEW_PERFORMANCE)) {
			Performance_File *file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);

			filename ("%s_MIN.%s") % perf_name % perf_ext;
			file->File_Type ("New Minimum Performance File");

			file->Create (filename);

			Write_Performance (count_day_array);
		}
		if (link_data_flag) {
			filename ("%s_MIN.%s") % link_data_name % link_data_ext;
			link_data_file.File_Type ("New Minimum Link Data File");

			link_data_file.Create (filename);
			link_data_file.Write_Header ();

			Write_Link_Data (link_data_file, count_day_array);
		}

		//---- maximum counts ----

		Combine_Data (MAXIMUM);

		if (System_File_Flag (NEW_PERFORMANCE)) {
			Performance_File *file = (Performance_File *) System_File_Handle (NEW_PERFORMANCE);

			filename ("%s_MAX.%s") % perf_name % perf_ext;
			file->File_Type ("New Maximum Performance File");

			file->Create (filename);

			Write_Performance (count_day_array);
		}
		if (link_data_flag) {
			filename ("%s_MAX.%s") % link_data_name % link_data_ext;
			link_data_file.File_Type ("New Maximum Link Data File");

			link_data_file.Create (filename);
			link_data_file.Write_Header ();

			Write_Link_Data (link_data_file, count_day_array);
		}

	}
	Exit_Stat (DONE);
}
