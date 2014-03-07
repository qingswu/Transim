//*********************************************************
//	Line_Rider_Report - write the transit route profile
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Line_Rider_Report
//---------------------------------------------------------

void RiderSum::Line_Rider_Report (void)
{
	int riders, board, alight, run, runs, num, length, tot_len, period, num_periods;
	int max_riders, max_board, max_alight, max_runs, total, capacity, max_cap;
	double factor, max_fac, sum_time, tot_time, time, capfac, max_capfac;
	double vmt, vht, pmt, pht;
	Dtime low, high;

	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr, next_itr;
	Line_Run_Itr run_itr;
	Veh_Type_Data *veh_type_ptr, *run_type_ptr;

	Show_Message ("Line Rider Profile -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (LINE_RIDERS);

	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	num = (int) (line_map.size () * stop_map.size ());

	if (!Break_Check (num + 5)) {
		Print (1);
		Line_Rider_Header ();
	}

	//---- process each route ----

	for (map_itr = line_map.begin (); map_itr != line_map.end (); map_itr++) {
		Show_Progress ();

		if (select_routes && !route_range.In_Range (map_itr->first)) continue;

		line_ptr = &line_array [map_itr->second];

		if (select_modes && !select_mode [line_ptr->Mode ()]) continue;

		//---- check the link criteria ----

		if (!Link_Selection (line_ptr)) continue;

		//---- set the run flags ----

		if (!Run_Selection (line_ptr)) continue;

		//---- save the route ridership data ----
		
		veh_type_ptr = &veh_type_array [line_ptr->Type ()];

		for (period = 0; period < num_periods; period++) {
			if (period_flag [period] == 0) continue;

			if (!Break_Check ((int) line_ptr->size () + 15)) {
				Print (1);
				Line_Rider_Header ();
			}
			veh_type_ptr = &veh_type_array [line_ptr->Type ()];

			Print (1, "   Route    Mode     Type   Time Period  Name");

			Print (2, String ("%8d  %10.10s %4d  %12.12s") % line_ptr->Route () %
				Transit_Code ((Transit_Type) line_ptr->Mode ()) % veh_type_ptr->Type () % 
				sum_periods.Range_Format (period));

			if (!line_ptr->Name ().empty ()) {
				Print (0, String ("  %s") % line_ptr->Name ());
			}
			if (Notes_Name_Flag ()) {
				if (!line_ptr->Notes ().empty ()) {
					Print (0, String (" -- %s") % line_ptr->Notes ());
				}
			}
			Print (2, "    Stop  Length   TTime   Alight    Board   Riders  Runs  LoadFac Capacity CapFac");
			Print (1);

			time = tot_time = 0.0;
			max_alight = max_board = max_riders = max_runs = total = length = tot_len = max_cap = 0;
			max_fac = vmt = vht = pmt = pht = max_capfac = 0.0;

			sum_periods.Period_Range (period, low, high);

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
				riders = board = alight = runs = capacity = 0;

				stop_ptr = &stop_array [stop_itr->Stop ()];

				next_itr = stop_itr + 1;

				if (next_itr != line_ptr->end ()) {
					length = next_itr->Length () - stop_itr->Length ();
				} else {
					length = 0;
				}
				sum_time = 0.0;
				num = 0;

				for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
					if (run_flag [run] == 0) continue;
					if (run_period [run] != period) continue;

					board += run_itr->Board ();
					alight += run_itr->Alight ();
					riders += run_itr->Load ();
					runs++;

					if (line_ptr->run_types.size () > 0) {
						run_type_ptr = &veh_type_array [line_ptr->Run_Type (run)];
						capacity += run_type_ptr->Capacity ();
					} else {
						capacity += veh_type_ptr->Capacity ();
					}
					if (next_itr != line_ptr->end ()) {
						time = next_itr->at (run).Schedule ().Seconds () - run_itr->Schedule ().Seconds ();

						vmt += length;
						vht += time;
						pmt += length * run_itr->Load ();
						pht += time * run_itr->Load ();

						sum_time += time;
						num++;
					}
				}
				if (runs == 0) continue;
				if (capacity == 0) capacity = runs;

				factor = (double) riders / runs;
				capfac = DTOI (riders * 10.0 / capacity) / 10.0;

				if (next_itr == line_ptr->end ()) runs = 0;

				if (num > 0) {
					time = sum_time / num;
				} else {
					time = 0;
				}
				Print (1, String ("%8d %7.0lf %7.0lf %8d %8d %8d %5d %8.1lf %6d %8.1lf") % stop_ptr->Stop () % 
					UnRound (length) % time % alight % board % riders % runs % factor % capacity % capfac);

				if (Notes_Name_Flag ()) {
					if (!stop_ptr->Name ().empty ()) {
						Print (0, String ("  %s") % stop_ptr->Name ());
					}
					if (!stop_ptr->Notes ().empty ()) {
						Print (0, " -- ") << stop_ptr->Notes ();
					}
				}
				if (alight > max_alight) max_alight = alight;
				if (board > max_board) max_board = board;
				if (riders > max_riders) max_riders = riders;
				if (runs > max_runs) max_runs = runs;
				if (factor > max_fac) max_fac = factor;
				if (capacity > max_cap) max_cap = capacity;
				if (capfac > max_capfac) max_capfac = capfac;

				tot_len += length;
				tot_time += time;
				total += board;
			}
			if (max_runs == 0) continue;

			Print (2, String (" Maximum                 %8ld %8ld %8ld %5ld %8.1lf %6lf %8.1lf") %
				max_alight % max_board % max_riders % max_runs % max_fac % max_cap % max_capfac);

			Print (2, "Total Boardings = ") << total;

			if (total == 0 || tot_time == 0) continue;

			factor = UnRound (tot_len);
			vmt = UnRound (vmt) / 5280.0;
			vht = vht / 3600.0;
			pmt = UnRound (pmt) / 5280.0;
			pht = pht / 3600.0;


			Print (1, String ("Route Length = %.1lf miles, %.1lf minutes  Average Speed = %.1lf mph") %
				External_Units (factor, MILES) % (tot_time / 60.0) % External_Units ((factor / tot_time), MPH));
			Print (1, String ("Vehicle Miles = %.1lf  Vehicle Hours = %.1lf") % vmt % vht);
			Print (1, String ("Passenger Miles = %.1lf  Passenger Hours = %.1lf") %	pmt % pht);
			Print (1, String ("Passengers per Vehicle Mile = %.1lf  Passengers per Vehicle Hour = %.1lf") %	
				(pmt / vht) % (pht / vht));

			vmt = pmt / total;
			vht = pht * 60.0 / total;

			Print (1, String ("Average Trip Length = %.1lf miles, %.1lf minutes") % vmt % vht);
		}
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Line_Rider_Header
//---------------------------------------------------------

void RiderSum::Line_Rider_Header (void)
{
	Print (1, "Line Rider Profile");
	Print (1);
}

/*********************************************|***********************************************

	Line Rider Profile

	   Route    Mode    Type  Time Period  Name 

	dddddddd ssssssssss dddd ssssssssssss  sssssssssss -- ssssssssssssssssss

	    Stop  Length   TTime   Alight    Board   Riders  Runs  LoadFac Capacity CapFac

	dddddddd ddddddd ddddddd dddddddd dddddddd dddddddd ddddd dddddd.d dddddd dddddd.d  ssssssssssssssssss

	 Maximum                 dddddddd dddddddd dddddddd ddddd dddddd.d dddddd dddddd.d  
	 
    Total Boardings = dddddddd  
	Route Length = ffff.f miles,  fffff.f minutes  Average Speed = ffff.f mph
	Vehicle Miles = dddddd.d  Vehicle Hours = dddddd.d
	Passenger Miles = ddddddd.d   Passenger Hours = dddddd.d
	Passengers per Vehicle Mile = fffff.f  Passengers per Vehicle Hour = ffff.f
	Average Trip Length = ffffff.f miles,  ffffff.f minutes
	            
**********************************************|***********************************************/ 
