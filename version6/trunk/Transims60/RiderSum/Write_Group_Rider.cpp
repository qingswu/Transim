//*********************************************************
//	Write_Group_Rider - write the group rider file
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Write_Group_Rider
//---------------------------------------------------------

void RiderSum::Write_Group_Rider (void)
{
	int n, run, runs, num, length, period, num_periods, groups, routes, index, stop, to, link, capacity;
	double factor, sum_time, time, capfac;
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
	Veh_Type_Data *veh_type_ptr, *run_type_ptr;

	fstream &file = line_group_file.File ();
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Show_Message (String ("Writing % -- Record") % line_group_file.File_Type ());
	Set_Progress ();

	file << "Group\tPeriod\tRoutes\tStops\tDescription\n";
	file << "Stop\tLength\tTTime\tAlight\tBoard\tRiders\tRuns\tLoadFac\tCapacity\tCapFac\tStopName\n";

	//---- process each line group ----

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

			//---- process each line in the line group ----

			for (itr = group->begin (); itr != group->end (); itr++) {

				map_itr = line_map.find (*itr);
				if (map_itr == line_map.end ()) continue;

				Show_Progress ();

				line_ptr = &line_array [map_itr->second];

				//---- set the run flags ----

				if (!Run_Selection (line_ptr)) continue;

				//---- check the period flag ----

				if (period_flag [period] == 0) continue;

				time = 0.0;
				length = 0;
				flag = false;
				veh_type_ptr = &veh_type_array [line_ptr->Type ()];

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
							if (line_ptr->run_types.size () > 0) {
								run_type_ptr = &veh_type_array [line_ptr->Run_Type (run)];
								capacity = run_type_ptr->Capacity ();
							} else {
								capacity = veh_type_ptr->Capacity ();
							}
							link_ptr->runs++;
							link_ptr->capacity += capacity;
							time = next_itr->at (run).Schedule ().Seconds () - run_itr->Schedule ().Seconds ();
							sum_time += time;
							num++;
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
			file << n << "\t" << sum_periods.Range_Format (period) << "\t" << routes << "\t" << stop_index.size () << "\t" << label << "\n";

			for (data_itr = stop_group_array.begin (); data_itr != stop_group_array.end (); data_itr++) {

				map_itr = stop_map.find (data_itr->stop);
					
				stop_ptr = &stop_array [map_itr->second];
				
				if (data_itr->runs > 0) {
					factor = DTOI (data_itr->riders * 10.0 / data_itr->runs) / 10.0;
				} else {
					factor = 0;
				}
				if (data_itr->capacity > 0) {
					capfac = DTOI (data_itr->riders * 10.0 / data_itr->capacity) / 10.0;
				} else {
					capfac = 0;
				}
				file << data_itr->stop << "\t" << UnRound (data_itr->length) << "\t" << data_itr->time << "\t" <<
					data_itr->alight << "\t" << data_itr->board << "\t" << data_itr->riders << "\t" << 
					data_itr->runs << "\t" << factor << "\t" << data_itr->capacity << "\t" << capfac << "\t";

				if (Notes_Name_Flag ()) {
					file << stop_ptr->Name ();

					if (!stop_ptr->Notes ().empty ()) {
						file << " -- " << stop_ptr->Notes ();
					}
				}
				file << "\n";
			}
		}
	}
	End_Progress ();
	line_group_file.Close ();
}
