//*********************************************************
//	Load_Transit_Data - load transit data to links
//*********************************************************

#include "PerfPrep.hpp"

//---------------------------------------------------------
//	Load_Transit_Data
//---------------------------------------------------------

void PerfPrep::Load_Transit_Data (void)
{
	int offset, run, runs, num_stops, index;
	Dtime time, dwell, ttime;
	double volume, persons, load, len_fac, max_len_fac;
	bool first;

	Line_Itr line_itr;
	Line_Stop_Itr stop_itr, first_itr, last_itr;
	Driver_Itr driver_itr;
	Stop_Data *stop_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Veh_Type_Data *veh_type_ptr, *run_type_ptr;

	Show_Message (String ("Loading Transit Data -- Record"));
	Set_Progress ();

	merge_perf_array.Zero_Flows ();

	veh_type_ptr = 0;
	volume = persons = 1.0;
	dwell.Seconds (5); 

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		Show_Progress ();

		first_itr = line_itr->begin ();
		if (first_itr == line_itr->end ()) continue;

		last_itr = --line_itr->end ();
		
		//dwell = (veh_type_ptr->Min_Dwell () + veh_type_ptr->Max_Dwell ()) / 2;
		//if (dwell < Dtime (2, SECONDS)) dwell.Seconds (2);

		if (transit_pce_flag) {
			veh_type_ptr = &veh_type_array [line_itr->Type ()];
			volume = UnRound (veh_type_ptr->PCE ());
		} else if (transit_veh_flag) {
			volume = 1.0;
		} else {
			volume = 0.0;
		}
		load = 1.0;
		runs = (int) first_itr->size ();

		for (run=0; run < runs; run++) {
			time = first_itr->at (run).Schedule ();

			if (transit_pce_flag) {
				if (line_itr->run_types.size () > 0) {
					run_type_ptr = &veh_type_array [line_itr->Run_Type (run)];
					volume = UnRound (run_type_ptr->PCE ());
				} else {
					volume = UnRound (veh_type_ptr->PCE ());
				}
			}
			first = true;
			offset = 0;
			max_len_fac = persons = 0.0;
			num_stops = 0;
			stop_itr = line_itr->begin ();

			for (driver_itr = line_itr->driver_array.begin (); driver_itr != line_itr->driver_array.end (); driver_itr++) {
				dir_ptr = &dir_array [*driver_itr];
				link_ptr = &link_array [dir_ptr->Link ()];

				for (; stop_itr != line_itr->end (); stop_itr++) {
					stop_ptr = &stop_array [stop_itr->Stop ()];

					if (stop_ptr->Link_Dir () != dir_ptr->Link_Dir ()) break;

					if (!first) {
						len_fac = (double) (stop_ptr->Offset () - offset) / link_ptr->Length ();

						if (len_fac >= max_len_fac) {
							max_len_fac = len_fac;
							persons = load;
						}
					} else {
						first = false;
					}
					if (transit_person_flag) {
						load = stop_itr->at (run).Load ();
					}
					offset = stop_ptr->Offset ();
					num_stops++;
				}
				if (first) continue;

				len_fac = (double) (link_ptr->Length () - offset) / link_ptr->Length ();

				if (len_fac >= max_len_fac) {
					persons = load;
				}
				index = *driver_itr;

				if (dir_ptr->Use_Index () >= 0) {
					index = dir_ptr->Use_Index ();
				}
				ttime = merge_perf_array.Flow_Time (index, time, 1.0, link_ptr->Length (), volume, persons) + num_stops * dwell;
				if (ttime < 0) break;
				time += ttime;

				if (stop_itr == last_itr) break;

				max_len_fac = 0.0;
				offset = 0;
				persons = load;
				num_stops = 0;
			}
		}
	}
	End_Progress ();
}

