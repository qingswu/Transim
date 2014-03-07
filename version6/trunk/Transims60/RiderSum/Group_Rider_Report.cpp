//*********************************************************
//	Group_Rider_Report - write the transit route profile
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Group_Rider_Report
//---------------------------------------------------------

void RiderSum::Group_Rider_Report (void)
{
	int n, run, runs, num, length, tot_len, period, num_periods, routes, groups, index, stop, to, link;
	int max_riders, max_board, max_alight, max_runs, total;
	double factor, max_fac, sum_time, tot_time, time;
	double vmt, vht, pmt, pht;
	Dtime low, high;
	String label;
	bool flag;

	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr, to_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr, next_itr;
	Line_Run_Itr run_itr;
	Int_Map stop_index;
	Int2_Map link_index;
	Int2_Map_Itr map2_itr;
	Int2_Key int2_key;
	Int2_Map_Stat map2_stat;
	Int_Map_Stat map_stat;
	Stop_Group_Data data, *data_ptr, *link_ptr;
	Stop_Group_Itr data_itr;

	Show_Message ("Line Group Profile -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (GROUP_RIDERS);

	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	num = (int) (line_map.size () * stop_map.size ());

	if (!Break_Check (num + 5)) {
		Print (1);
		Group_Rider_Header ();
	}

	groups = line_equiv.Max_Group ();
	routes = 0;
	memset (&data, '\0', sizeof (data));
	
	for (n=1; n <= groups; n++) {

		group = line_equiv.Group_List (n);
		if (group == 0) continue;

		label = line_equiv.Group_Label (n);

		//---- process each time period ----

		for (period = 0; period < num_periods; period++) {

			//---- initialize stop data ----

			stop_index.clear ();
			link_index.clear ();
			stop_group_array.clear ();
			routes = 0;
			vmt = vht = pmt = pht = 0.0;

			//---- process each line in the line group ----

			for (itr = group->begin (); itr != group->end (); itr++) {

				map_itr = line_map.find (*itr);
				if (map_itr == line_map.end ()) continue;

				Show_Progress ();

				line_ptr = &line_array [map_itr->second];

				//---- set the run flags ----

				if (!Run_Selection (line_ptr)) continue;

				if (period_flag [period] == 0) continue;

				time = 0.0;
				total = length = 0;
				flag = false;

				sum_periods.Period_Range (period, low, high);

				for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {

					stop_ptr = &stop_array [stop_itr->Stop ()];
					stop = stop_ptr->Stop ();

					map_itr = stop_index.find (stop);

					next_itr = stop_itr + 1;

					if (next_itr != line_ptr->end ()) {
						stop_ptr = &stop_array [next_itr->Stop ()];

						to = stop_ptr->Stop ();
					} else {
						to = 0;
					}

					//---- create the link ----

					int2_key = Int2_Key (stop, to);

					map2_itr = link_index.find (int2_key);

					if (map2_itr == link_index.end ()) {
						if (to == 0 && map_itr != stop_index.end ()) {
							link = map_itr->second;
						} else {
							link = (int) stop_group_array.size ();

							link_index.insert (Int2_Map_Data (int2_key, link));

							data.stop = stop;
							data.to = to;
							stop_group_array.push_back (data);

							//---- check for a merge ----

							if (to != 0) {
								to_itr = stop_index.find (to);

								if (to_itr != stop_index.end ()) {
									index = (int) stop_group_array.size ();

									int2_key = Int2_Key (to, 0);

									map2_stat = link_index.insert (Int2_Map_Data (int2_key, index));

									if (map2_stat.second) {
										data.stop = to;
										data.to = 0;
										stop_group_array.push_back (data);
									}
								}
							}
						}
					} else {
						link = map2_itr->second;
					}
					link_ptr = &stop_group_array [link];

					//---- crete the stop index ----

					if (map_itr == stop_index.end ()) {
						map_stat = stop_index.insert (Int_Map_Data (stop, link));
						index = link;
					} else {
						index = map_itr->second;
					}
					data_ptr = &stop_group_array [index];

					if (next_itr != line_ptr->end ()) {
						length = next_itr->Length () - stop_itr->Length ();
					} else {
						length = 0;
					}
					sum_time = 0.0;
					num = runs = 0;

					for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
						if (run_flag [run] == 0) continue;
						if (run_period [run] != period) continue;

						data_ptr->board += run_itr->Board ();
						data_ptr->alight += run_itr->Alight ();
						link_ptr->riders += run_itr->Load ();
						runs++;
						flag = true;

						if (next_itr != line_ptr->end ()) {
							link_ptr->runs++;
							time = next_itr->at (run).Schedule ().Seconds () - run_itr->Schedule ().Seconds ();
							sum_time += time;
							num++;

							vmt += length;
							vht += time;
							pmt += length * run_itr->Load ();
							pht += time * run_itr->Load ();
						}
					}
					if (runs == 0) continue;

					if (link_ptr->length == 0.0) {
						link_ptr->length = length;
					}
					if (num > 0) {
						time = sum_time / num;
					} else {
						time = 0;
					}
					if (link_ptr->time > 0) {
						link_ptr->time = (link_ptr->time + time) / 2.0;
					} else {
						link_ptr->time = time;
					}
				}
				if (flag) routes++;
			}

			//---- print the report ----

			if (!Break_Check ((int) stop_index.size () + 15)) {
				Print (1);
				Group_Rider_Header ();
			}
			Print (1, "Group   Time Period  Routes  Description");

			Print (2, String ("%5d  %12.12s  %6d  %s") % n %	sum_periods.Range_Format (period) %
				routes % label);

			Print (2, "    Stop  Length   TTime   Alight    Board   Riders  Runs  LoadFac");
			Print (1);

			tot_time = 0.0;
			max_alight = max_board = max_riders = max_runs = total = tot_len = 0;
			max_fac = 0.0;

			for (data_itr = stop_group_array.begin (); data_itr != stop_group_array.end (); data_itr++) {

				map_itr = stop_map.find (data_itr->stop);
					
				stop_ptr = &stop_array [map_itr->second];
				
				if (data_itr->runs > 0) {
					factor = DTOI (data_itr->riders * 10.0 / data_itr->runs) / 10.0;
				} else {
					factor = 0.0;
				}
				Print (1, String ("%8d %7.0lf %7.0lf %8d %8d %8d %5d %8.1lf") % data_itr->stop  % 
					UnRound (data_itr->length) % data_itr->time % data_itr->alight % data_itr->board % 
					data_itr->riders % data_itr->runs % factor);

				if (Notes_Name_Flag ()) {
					if (!stop_ptr->Name ().empty ()) {
						Print (0, String ("  %s") % stop_ptr->Name ());
					}
					if (!stop_ptr->Notes ().empty ()) {
						Print (0, " -- ") << stop_ptr->Notes ();
					}
				}
				if (data_itr->alight > max_alight) max_alight = data_itr->alight;
				if (data_itr->board > max_board) max_board = data_itr->board;
				if (data_itr->riders > max_riders) max_riders = data_itr->riders;
				if (data_itr->runs > max_runs) max_runs = data_itr->runs;
				if (factor > max_fac) max_fac = factor;

				tot_len += data_itr->length;
				tot_time += data_itr->time;
				total += data_itr->board;
			}
			if (max_runs == 0) continue;

			Print (2, String (" Maximum                 %8ld %8ld %8ld %5ld %8.1lf") %
				max_alight % max_board % max_riders % max_runs % max_fac);

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
//	Group_Rider_Header
//---------------------------------------------------------

void RiderSum::Group_Rider_Header (void)
{
	Print (1, "Line Group Profile");
	Print (1);
}

/*********************************************|***********************************************

	Line Group Profile

	Group   Time Period  Routes  Description 

	ddddd  ssssssssssss  dddddd  sssssssssssssssssssssssssssss

	    Stop  Length   TTime   Alight    Board   Riders  Runs  LoadFac

	dddddddd ddddddd ddddddd dddddddd dddddddd dddddddd ddddd dddddd.d  ssssssssssssssssss

	 Maximum                 dddddddd dddddddd dddddddd ddddd dddddd.d
	 
    Total Boardings = dddddddd  
	Route Length = ffff.f miles,  fffff.f minutes  Average Speed = ffff.f mph
	Vehicle Miles = dddddd.d  Vehicle Hours = dddddd.d
	Passenger Miles = ddddddd.d   Passenger Hours = dddddd.d
	Passengers per Vehicle Mile = fffff.f  Passengers per Vehicle Hour = ffff.f
	Average Trip Length = ffffff.f miles,  ffffff.f minutes
	            
**********************************************|***********************************************/ 
