//*********************************************************
//	Get_Performance_Data.cpp - read the performance file
//*********************************************************

#include "LinkSum.hpp"

//---------------------------------------------------------
//	Get_Performance_Data
//---------------------------------------------------------

bool LinkSum::Get_Performance_Data (Performance_File &file, Performance_Data &perf_rec)
{
	if (compare_perf_flag) {
		int link, dir;

		Dir_Data *dir_ptr;
		Link_Data *link_ptr;
		Int_Map_Itr map_itr;

		link = file.Link ();

		map_itr = compare_link_map.find (link);

		if (map_itr != compare_link_map.end ()) {
			dir = map_itr->second;

			map_itr = link_map.find (link);

			if (map_itr != link_map.end ()) {
				if (Data_Service::Get_Performance_Data (file, perf_rec)) {
					int i, num, start, end, index;
					double factor, p1, p2, share, occ_fac;
					Dtime time;
					bool sum_flag;

					Time_Periods *period_ptr;
					Dir_Data *dir_ptr;
					Perf_Data *perf_ptr;
					Perf_Period *perf_period_ptr;
					Perf_Itr perf_itr;
					Perf_Period_Itr perf_period_itr;

					//---- get the time period ----

					period_ptr = compare_perf_array.periods;

					if (period_ptr->Period_Range (perf_rec.Start (), perf_rec.End (), start, end)) {

						p1 = period_ptr->Period_Offset (start, perf_rec.Start ());
						p2 = period_ptr->Period_Offset (end, perf_rec.End ());

						if (p1 != p2) {

							//---- allocate the data to multiple time periods ----

							num = end - start;
							factor = 1.0 / (end - start + p2 - p1);

							for (i = start; i <= end; i++) {
								if (i == start && i == end) {
									share = 1.0;
								} else if (i == start) {
									share = (1.0 - p1) * factor;
								} else if (i == end) {
									share = p2 * factor;
								} else {
									share = factor;
								}
		
								//---- process the link record ----

								perf_period_ptr = compare_perf_array.Period_Ptr (i);

								dir_ptr = &dir_array [perf_rec.Dir_Index ()];

								if (perf_rec.Type () == 0 || !Lane_Use_Flows ()) {
									index = perf_rec.Dir_Index ();
									sum_flag = (perf_rec.Type () == 1);
								} else {
									index = dir_ptr->Use_Index ();
									sum_flag = false;
								}
								if (index < 0) continue;

								perf_ptr = perf_period_ptr->Data_Ptr (index);

								if (!sum_flag && num == 0 && perf_ptr->Volume () > 0) {
									if (perf_rec.Volume () > 0) {
										occ_fac = perf_rec.Persons () / perf_rec.Volume ();
									} else if (perf_ptr->Volume () > 0) {
										occ_fac = perf_ptr->Persons () / perf_ptr->Volume ();
									} else {
										occ_fac = 1.0;
									}
									perf_ptr->Add_Persons (perf_rec.Enter () * occ_fac);
									perf_ptr->Add_Volume (perf_rec.Enter ());
									perf_ptr->Sum_Max_Queue (perf_rec.Max_Queue ());
								} else {
									perf_ptr->Add_Persons (perf_rec.Persons () * share);
									perf_ptr->Add_Volume (perf_rec.Volume () * share);
									perf_ptr->Add_Max_Queue (perf_rec.Max_Queue ());
								}
								perf_ptr->Add_Count ();
								perf_ptr->Add_Enter (perf_rec.Enter () * share);
								perf_ptr->Add_Exit (perf_rec.Exit () * share);
								perf_ptr->Sum_Max_Volume (perf_rec.Volume ());
								perf_ptr->Add_Queue (perf_rec.Queue () * share);
								perf_ptr->Add_Failure (DTOI (perf_rec.Failure () * share));
								perf_ptr->Add_Veh_Dist (Round (perf_rec.Veh_Dist () * share));
								perf_ptr->Add_Veh_Time (perf_rec.Veh_Time () * share);

								if (Ratio_Flag () && Round (perf_rec.Time_Ratio ()) >= Congested_Ratio ()) {
									perf_ptr->Add_Ratio ();
									perf_ptr->Add_Ratio_Dist (Round (perf_rec.Veh_Dist () * share));
									perf_ptr->Add_Ratio_Time (perf_rec.Veh_Time () * share);
								}
							}
						}
					}
				}
			}
			dir_ptr = &dir_array [dir];
			link_ptr = &link_array [dir_ptr->Link ()];

			file.Link (link_ptr->Link ());

			if (dir_ptr->Dir () == 1) {
				dir = file.Dir ();
				file.Dir (1 - dir);
			}
		}
	}
	return (Data_Service::Get_Performance_Data (file, perf_rec));
}

