//*********************************************************
//	Sum_Riders - sum the transit route between stops
//*********************************************************

#include "ArcRider.hpp"

//---------------------------------------------------------
//	Sum_Riders
//---------------------------------------------------------

void ArcRider::Sum_Riders (void)
{
	int i, *board, *alight, *start, *end, *cap_start, *cap_end, stop, index, load, runs, capacity, cap, run;
	bool flag;

	Link_Data *link_ptr;
	Dir_Data *dir_ptr;
	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Driver_Itr driver_itr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr line_stop_itr;
	Line_Run_Itr run_itr;
	Veh_Type_Data *veh_type_ptr, *run_type_ptr;

	//---- process each route ----

	Show_Message ("Summing Ridership Data -- Record");
	Set_Progress ();

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		if (select_routes && !route_range.In_Range (map_itr->first)) continue;

		line_ptr = &line_array [map_itr->second];

		if (select_transit_modes && !select_transit [line_ptr->Mode ()]) continue;

		if (Line_Equiv_Flag ()) {
			Int_Set *group;
			Int_Set_Itr itr;
			flag = false;

			for (i = line_equiv.First_Group (); i > 0 && !flag; i = line_equiv.Next_Group ()) {
				group = line_equiv.Group_List (i);
				if (group == 0) continue;

				for (itr = group->begin (); itr != group->end () && !flag; itr++) {
					if (*itr == line_ptr->Route ()) flag = true;
				}
			}
			if (!flag) continue;
		}

		//---- check the link criteria ----

		if (select_links) {
			int num, n1, n2;
			Data_Range_Itr range_itr;

			for (range_itr = link_ranges.begin (); range_itr != link_ranges.end (); range_itr++) {
				num = range_itr->Max_Count ();
				n1 = n2 = 0;

				for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {

					//---- check the link sequence ----

					n2 = range_itr->Range_Count (*driver_itr);
					if (!n2) continue;

					if (n2 != n1 + 1) {
						n2 = 0;
						break;
					}
					if (n2 == num) break;
					n1 = n2;
				}
				if (n2 != num) continue;
			}
		}

		//---- save the route ridership by link ----

		stop = 1;
		line_stop_itr = line_ptr->begin();
		if (line_stop_itr == line_ptr->end ()) continue;
		
		veh_type_ptr = &veh_type_array [line_ptr->Type ()];

		index = line_stop_itr->Stop ();

		stop_ptr = &stop_array [index];

		board = &board_data [index];
		alight = &alight_data [index];
		start = &start_runs [index];
		cap_start = &start_cap [index];
		end = &end_runs [index];
		cap_end = &end_cap [index];
		load = runs = capacity = 0;

		for (driver_itr = line_ptr->driver_array.begin (); driver_itr != line_ptr->driver_array.end (); driver_itr++) {
			dir_ptr = &dir_array [*driver_itr];

			link_ptr = &link_array [dir_ptr->Link ()];

			if (rider_flag) {
				load_data [*driver_itr] += load;
				enter_runs [*driver_itr] += runs;
				enter_cap [*driver_itr] += capacity;
			}
			while (stop_ptr->Link_Dir () == dir_ptr->Link_Dir ()) {

				load = runs = capacity = 0;

				for (run=0, run_itr = line_stop_itr->begin (); run_itr != line_stop_itr->end (); run_itr++, run++) {
					if (select_time_of_day && !time_range.In_Range (run_itr->Schedule ())) continue;

					load += run_itr->Load ();
					*board += run_itr->Board ();
					*alight += run_itr->Alight ();
					runs += 1;

					if (line_ptr->run_types.size () > 0) {
						run_type_ptr = &veh_type_array [line_ptr->Run_Type (run)];
						cap = run_type_ptr->Capacity ();
					} else {
						cap = veh_type_ptr->Capacity ();
					}
					capacity += cap;

					if (stop == 1) {
						*start += 1;
						*cap_start += cap;
					}
					if (stop == (int) line_stop_itr->size ()) {
						*end += 1;
						*cap_end += cap;
					}
				}
				stop++;
				if (++line_stop_itr == line_ptr->end ()) break;

				index = line_stop_itr->Stop ();

				stop_ptr = &stop_array [index];

				board = &board_data [index];
				alight = &alight_data [index];
				start = &start_runs [index];
				end = &end_runs [index];
				cap_start = &start_cap [index];
				cap_end = &end_cap [index];
			}
			if (line_stop_itr == line_ptr->end ()) break;
		}
	}
	End_Progress ();
}
