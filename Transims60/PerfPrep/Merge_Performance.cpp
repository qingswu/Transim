//*********************************************************
//	Merge_Performance.cpp - merge performance data
//*********************************************************

#include "PerfPrep.hpp"

#include "Shape_Tools.hpp"
#include <math.h>

//---------------------------------------------------------
//	Merge_Performance
//---------------------------------------------------------

void PerfPrep::Merge_Performance (void)
{
	int period, index;
	double volume, merge_vol, dx, dy, x0, y0, min_x, min_y, max_x, max_y;
	bool factor_flag, subarea_flag;

	Perf_Itr perf_itr;
	Perf_Data *perf_ptr, *perf2_ptr;
	Perf_Period_Itr period_itr;
	Perf_Period *period_ptr, *period2_ptr;
	//Dir_Data *dir_ptr;
	Node_Data *node_ptr;
	Int_Map_Itr map_itr;

	//---- merge performance files ----

	Show_Message ("Merging Performance Data -- Record");
	Set_Progress ();

	if (method == SIMPLE_LINK_AVG) {
		factor = 1.0;
		factor_flag = false;
	} else {
		factor_flag = (factor == 0.0);
	}
	
	//---- polygon range ----

	if (select_polygon) {
		min_x = min_y = MAX_INTEGER;
		max_x = max_y = -MAX_INTEGER;

		for (Points_Itr itr = polygon_file.begin (); itr != polygon_file.end (); itr++) {
			if (itr->x < min_x) min_x = itr->x;
			if (itr->y < min_y) min_y = itr->y;
			if (itr->x > max_x) max_x = itr->x;
			if (itr->y > max_y) max_y = itr->y;
		}
	} else {
		min_x = min_y = max_x = max_y = 0.0;
	}

	//---- process each time period ----

	for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {

		if (merge_perf_array.periods->Num_Periods () <= period) continue;

		period_ptr = &merge_perf_array [period];

		if (base_flag) {
			if (base_perf_array.periods->Num_Periods () <= period) continue;

			period2_ptr = &base_perf_array [period];
		} else {
			period2_ptr = 0;
		}

		//---- process each link direction ----

		for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
			Show_Progress ();

			volume = perf_itr->Volume ();

			if (method == REPLACE_LINKS && volume > 0.0) continue;

			if ((int) period_ptr->size () <= index) continue;

			perf_ptr = period_ptr->Data_Ptr (index);

			if (base_flag) {
				double value;

				if ((int) period2_ptr->size () <= index) continue;

				perf2_ptr = period2_ptr->Data_Ptr (index);
			
				Dir_Data *dir_ptr = &dir_array [index];

				value = fabs (perf_itr->Volume () - perf2_ptr->Volume ());
				if (value >= 10 && dir_ptr->First_Lane_Use () < 0) {
					subarea_flag = true;

					Link_Data *link_ptr;

					link_ptr = &link_array [dir_ptr->Link ()];

					node_ptr = &node_array [link_ptr->Anode ()];

					if (select_polygon) {
						if (!In_Polygon (polygon_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
							node_ptr = &node_array [link_ptr->Bnode ()];

							if (!In_Polygon (polygon_file, UnRound (node_ptr->X ()), UnRound (node_ptr->Y ()))) {
								subarea_flag = false;
							}
						}
					}
					if (!subarea_flag) {
						x0 = UnRound (node_ptr->X ());
						y0 = UnRound (node_ptr->Y ());

						if (x0 < min_x) {
							dx = min_x - x0;
						} else if (x0 > max_x) {
							dx = x0 - max_x;
						} else {
							dx = x0 - (max_x + min_x) / 2;
						}
						if (y0 < min_y) {
							dy = min_y - y0;
						} else if (y0 > max_y) {
							dy = y0 - max_y;
						} else {
							dy = y0 - (max_y + min_y) / 2;
						}							
						dx = sqrt (dx * dx + dy * dy) / 5280.0;

						if (dx <= 1.0) subarea_flag = true;
					}
					if (subarea_flag && link_ptr->Type () != FREEWAY) {
						value = MAX ((perf_ptr->Persons () + perf_itr->Persons () - perf2_ptr->Persons ()), 0);
						perf_ptr->Persons (value);

						value = MAX ((perf_ptr->Volume () + perf_itr->Volume () - perf2_ptr->Volume ()), 0);

						perf_ptr->Volume (value);

						value = MAX ((perf_ptr->Enter () + perf_itr->Enter () - perf2_ptr->Enter ()), 0);
						perf_ptr->Enter (value);

						value = MAX ((perf_ptr->Exit () + perf_itr->Exit () - perf2_ptr->Exit ()), 0);
						perf_ptr->Exit (value);

						value = MAX ((perf_ptr->Veh_Dist () + perf_itr->Veh_Dist () - perf2_ptr->Veh_Dist ()), 0);
						perf_ptr->Veh_Dist (value);

						value = MAX ((perf_ptr->Veh_Time () + perf_itr->Veh_Time () - perf2_ptr->Veh_Time ()), 0);
						perf_ptr->Veh_Time (value);

						value = MAX ((perf_ptr->Time () + perf_itr->Time () - perf2_ptr->Time ()), 0);
						perf_ptr->Time ((int) value);
					}
				}
				perf_itr->Set_Flows (perf_ptr);
				perf_itr->Time (perf_ptr->Time ());

			} else if (method == ADD_FLOWS) {
				perf_itr->Add_Flows (perf_ptr);
				perf_itr->Time (perf_ptr->Time ());
			} else if (method == REPLACE_LINKS || (method == REPLACE_AVERAGE && volume == 0.0)) {
				perf_itr->Set_Flows (perf_ptr);
				perf_itr->Time (perf_ptr->Time ());
			} else if (factor_flag) {
				merge_vol = perf_ptr->Volume ();

				if (volume > 0) {
					factor = merge_vol / volume;
				} else {
					factor = merge_vol;
				}
				perf_itr->Weight_Flows (perf_ptr, factor);
				perf_itr->Weight_Time (perf_ptr->Time (), factor);
			} else {
				perf_itr->Average_Flows (perf_ptr);
				perf_itr->Average_Time (perf_ptr->Time ());
			}
		}
	}
	End_Progress ();

	//---- apply max travel time ratio ----

	//if (!ratio_flag) return;

	//Show_Message ("Checking Travel Time Ratio -- Record");
	//Set_Progress ();

	//for (period=0, period_itr = perf_period_array.begin (); period_itr != perf_period_array.end (); period_itr++, period++) {

	//	//---- process each link direction ----

	//	for (index=0, perf_itr = period_itr->begin (); perf_itr != period_itr->end (); perf_itr++, index++) {
	//		Show_Progress ();

	//		if (index < (int) dir_array.size ()) {
	//			rec = index;
	//		} else {
	//			rec = lane_use_flow_index [index - (int) dir_array.size ()];
	//		}
	//		dir_ptr = &dir_array [rec];
	//		
	//		if (dir_ptr->Time0 () <= 0) continue;

	//		ratio = (double) perf_itr->Time () / dir_ptr->Time0 ();
	//		if (ratio <= time_ratio) continue;

	//		ratio = time_ratio / ratio;

	//		perf_itr->Time (DTOI (perf_itr->Time () * ratio));
	//	}
	//}
	//End_Progress ();
}

