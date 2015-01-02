//*********************************************************
//	Update_Schedules.cpp - update transit schedules
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Update_Schedules
//---------------------------------------------------------

void Reschedule::Update_Schedules (void)
{
	int run, runs, offset, num_stops, index;
	double len_fac, diff;
	Dtime time, time0, time1, sum0, sum1, org_tod, new_tod;
	bool first;
	
	Line_Itr line_itr;
	Line_Stop_Itr stop_itr, first_itr, last_itr;
	Line_Run *run_ptr;
	Driver_Itr driver_itr;
	Stop_Data *stop_ptr;
	Dir_Data *dir_ptr;
	Link_Data *link_ptr;
	Perf_Period *perf0_ptr, *perf1_ptr;

	//---- process each transit route ----

	for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
		Show_Progress ();

		first_itr = line_itr->begin ();
		if (first_itr == line_itr->end ()) continue;

		last_itr = --line_itr->end ();

		runs = (int) first_itr->size ();

		//---- update the schedules for each run ----

		for (run=0; run < runs; run++) {
			org_tod = new_tod = first_itr->at (run).Schedule ();

			sum0 = sum1 = 0;
			first = true;
			offset = 0;
			num_stops = 0;
			stop_itr = line_itr->begin ();

			//---- find travel time changes for links along the route ----

			for (driver_itr = line_itr->driver_array.begin (); driver_itr != line_itr->driver_array.end (); driver_itr++) {
				dir_ptr = &dir_array [*driver_itr];
				link_ptr = &link_array [dir_ptr->Link ()];

				index = *driver_itr;

				if (dir_ptr->Use_Index () >= 0) {
					index = dir_ptr->Use_Index ();
				}

				perf0_ptr = perf_period_array.Period_Ptr (new_tod);
				perf1_ptr = update_array.Period_Ptr (new_tod);

				//---- skip links that are not included in both performance files ----

				if (perf0_ptr == 0 || perf1_ptr == 0) continue;

				time0 = perf0_ptr->Time (index);
				time1 = perf1_ptr->Time (index);

				for (; stop_itr != line_itr->end (); stop_itr++) {
					stop_ptr = &stop_array [stop_itr->Stop ()];

					if (stop_ptr->Link_Dir () != dir_ptr->Link_Dir ()) break;

					if (!first) {
						len_fac = (double) (stop_ptr->Offset () - offset) / link_ptr->Length ();

						sum0 += (int) (time0 * len_fac);
						sum1 += (int) (time1 * len_fac);

						run_ptr = &stop_itr->at (run);

						time = run_ptr->Schedule ();

						diff = time - org_tod;
						org_tod = time;

						if (sum0 > 0) {
							if (sum1 > sum0 * 4) {
								diff *= 4.0;
							} else {
								diff = diff * sum1 / sum0;
							}
						}
						new_tod = (int) (new_tod + diff);
						if (new_tod >= Model_End_Time ()) {
							new_tod = Model_End_Time () - 1;
						}
						run_ptr->Schedule (new_tod.Round_Seconds ());
						sum0 = sum1 = 0;
					} else {
						first = false;
					}
					offset = stop_ptr->Offset ();
					num_stops++;
				}
				if (first) continue;

				len_fac = (double) (link_ptr->Length () - offset) / link_ptr->Length ();
						
				sum0 += (int) (time0 * len_fac);
				sum1 += (int) (time1 * len_fac);

				if (stop_itr == last_itr) break;
			}
		}
	}
}
