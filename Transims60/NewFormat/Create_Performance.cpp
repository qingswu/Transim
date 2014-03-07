//*********************************************************
//	Create_Performance.cpp - Convert Link Delay to Performance
//*********************************************************

#include "NewFormat.hpp"

//---------------------------------------------------------
//	Create_Performance
//---------------------------------------------------------

void NewFormat::Create_Performance (void)
{
	int index, num_periods, dir;
	int i, increment;
	double length, density, factor;

	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Int_Map_Itr itr;
	Flow_Time_Array *link_delay_ptr;
	Flow_Time_Data *delay_data_ptr;
	Link_Perf_Array *link_perf_ptr;
	Link_Perf_Data *perf_data_ptr;

	increment = 1;

	if (link_perf_array.size () == 0) {
		if (Sum_Flow_Flag ()) {
			link_perf_array.Initialize (&sum_periods);
			increment = sum_periods.Increment ();
		} else {
			link_perf_array.Initialize (&time_periods);
			increment = time_periods.Increment ();
		}
	}
	turn_perf_array.Copy_Flow_Data (turn_delay_array);

	num_periods = link_delay_array.periods->Num_Periods ();

	//---- process each time period ----

	for (i=0; i < num_periods; i++) {

		link_delay_ptr = &link_delay_array [i];
		link_perf_ptr = &link_perf_array [i];

		//---- sort the links ----

		for (itr = link_map.begin (); itr != link_map.end (); itr++) {
			link_ptr = &link_array [itr->second];
			
			length = link_ptr->Length ();

			for (dir=0; dir < 2; dir++) {
				index = (dir) ? link_ptr->BA_Dir () : link_ptr->AB_Dir ();
				if (index < 0) continue;

				Show_Progress ();

				delay_data_ptr = &link_delay_ptr->at (index);
				if (delay_data_ptr->Flow () == 0) continue;
				
				dir_ptr = &dir_array [index];

				perf_data_ptr = &link_perf_ptr->at (index);

				perf_data_ptr->Flow (delay_data_ptr->Flow ());
				perf_data_ptr->Time (delay_data_ptr->Time ());

				length = UnRound (link_ptr->Length ());
				if (length < 0.1) length = 0.1;

				density = (double) delay_data_ptr->Flow () / increment;
				if (density < 0.01) density = 0.01;

				factor = dir_ptr->Lanes ();
				if (factor < 1) factor = 1;

				if (Metric_Flag ()) {
					factor = 1000.0 / (length * factor);
				} else {
					factor = MILETOFEET / (length * factor);
				}
				density *= factor;
				if (density > 0.0 && density < 0.01) density = 0.01;
				perf_data_ptr->Density (DTOI (density));
			}
		}
	}
}

