//*********************************************************
//	Iteration_Setup.cpp - configure the iteration data
//*********************************************************

#include "Converge_Service.hpp"

//---------------------------------------------------------
//	Iteration_Setup
//---------------------------------------------------------

void Converge_Service::Iteration_Setup (void)
{
	if (iteration == 1) {
		int i, num;

		if (link_gap_flag) {
			link_gap_file.File () << "ITERATION";
			num = dat->sum_periods.Num_Periods ();
			for (i=0; i < num; i++) {
				link_gap_file.File () << "\t" << dat->sum_periods.Range_Label (i);
			}
			link_gap_file.File () << "\tTOTAL" << endl;
			link_gap_file.File () << iteration;
		}
		if (trip_gap_flag) {
			trip_gap_file.File () << "ITERATION";
			num = dat->sum_periods.Num_Periods ();
			for (i=0; i < num; i++) {
				trip_gap_file.File () << "\t" << dat->sum_periods.Range_Label (i);
			}
			trip_gap_file.File () << "\tTOTAL" << endl;
			trip_gap_file.File () << iteration;
		}
	} else if (iteration > 1) {
		first_iteration = false;

		if (link_gap_flag) {
			link_gap_file.File () << iteration;
		}
		if (trip_gap_flag) {
			trip_gap_file.File () << iteration;
		}
	}
	if (save_plan_flag && save_plan_file.Part_Flag ()) {
		if (save_iter_range.In_Range (iteration)) {
			save_plan_file.Open (iteration);
		}
	}

	if (!first_iteration) {
		Use_Link_Delays (true);
	}
	if (rider_flag || (System_File_Flag (RIDERSHIP) && Cap_Penalty_Flag ())) {
		line_array.Clear_Ridership ();
	}
	potential = select_records;
	total_percent = 1.0;

	if (total_records > 0 && max_percent_flag) {
		percent_selected = ((double) select_records / total_records);
		total_percent = percent_selected;

		if (percent_selected > max_percent_select) {
			percent_selected = max_percent_select / percent_selected;
			if (select_weight > 0) {
				percent_selected = percent_selected * select_records / select_weight;
			}
		} else {
			percent_selected = 1.0;
		}
	} else {
		percent_selected = 1.0;
	}
	total_records = select_records = select_weight = 0;

	//---- preload transit vehicles ----

	if (preload_flag) {
		Preload_Transit ();
	}
}
