//*********************************************************
//	Read_Performance.cpp - read the performance file
//*********************************************************

#include "Data_Service.hpp"

//------------------------------------------------------
//	Read_Performance
//---------------------------------------------------------

void Data_Service::Read_Performance (Performance_File &file, Perf_Period_Array &perf_period_array)
{
	int i, num, start, end, link, dir, count, index, size;
	double factor, p1, p2, share, speed, occ_fac;
	Dtime time;
	bool sum_flag;

	Time_Periods *period_ptr;
	Dir_Data *dir_ptr;
	Performance_Data perf_rec;
	Perf_Data *perf_ptr;
	Perf_Period *perf_period_ptr;
	Perf_Itr perf_itr;
	Perf_Period_Itr perf_period_itr;

	//---- store the performance data ----

	Show_Message (String ("Reading %s -- Record") % file.File_Type ());
	Set_Progress ();
	
	Initialize_Performance (file, perf_period_array);
	count = 0;

	period_ptr = perf_period_array.periods;

	while (file.Read ()) {
		Show_Progress ();

		perf_rec.Clear ();

		if (!Get_Performance_Data (file, perf_rec)) continue;

		//---- get the time period ----

		if (period_ptr->Period_Range (perf_rec.Start (), perf_rec.End (), start, end)) {

			p1 = period_ptr->Period_Offset (start, perf_rec.Start ());
			p2 = period_ptr->Period_Offset (end, perf_rec.End ());
			if (p1 == p2) continue;

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

				perf_period_ptr = perf_period_array.Period_Ptr (i);

				dir_ptr = &dir_array [perf_rec.Dir_Index ()];

				if (perf_rec.Type () == 0 || !Lane_Use_Flows ()) {
					index = perf_rec.Dir_Index ();
					sum_flag = (perf_rec.Type () == 1);
				} else {
					index = dir_ptr->Use_Index ();
					sum_flag = false;
				}
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
					perf_ptr->Add_Ratio_Dist (perf_rec.Veh_Dist ());
					perf_ptr->Add_Ratio_Time (perf_rec.Veh_Time ());
				}
				count++;
			}
		}
	}
	End_Progress ();
	file.Close ();

	Break_Check (3);
	Print (2, String ("Number of %s Records = %d") % file.File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file.File_ID () % count);
	}
	Print (1, "Number of Link Direction Records = ") << count;

	if (count > 0) System_Data_True (PERFORMANCE);

	//---- calculate the travel times ----

	link = dir = start = end = 0;
	size = (int) dir_array.size ();

	for (perf_period_itr = perf_period_array.begin (); perf_period_itr != perf_period_array.end (); perf_period_itr++) {
		start++;
		bool flag = false;

		for (num=0, perf_itr = perf_period_itr->begin (); perf_itr != perf_period_itr->end (); perf_itr++, num++) {
			link++;

			if (num < size) {
				index = num;
			} else {
				index = lane_use_flow_index [num - size];
			}
			dir_ptr = &dir_array [index];

			if (perf_itr->Veh_Time () > 0) {
				dir++;
				flag = true;
				speed = perf_itr->Veh_Dist () / perf_itr->Veh_Time ();
				if (speed < 0.1) speed = 0.1;

				time = link_array [dir_ptr->Link ()].Length () / speed;
			} else if (perf_itr->Volume () > 0) {
				dir++;
				flag = true;
				time = perf_itr->Time ();
			} else {
				time = dir_ptr->Time0 ();
			}
			if (time < 1) time = 1;
			perf_itr->Time (time);
			
			if (Clear_Flow_Flag ()) perf_itr->Clear_Flows ();

			if (perf_itr->Count () > 0) {
				perf_itr->Queue (perf_itr->Queue () / perf_itr->Count ());
				//if (!Ratio_Flag ()) perf_itr->Occupancy (1);
			}
		}
		if (flag) end++;
	}
	Break_Check (6);
	Print (2, "Number of Summary Time Periods = ") << perf_period_array.periods->Num_Periods ();
	Print (1, String ("Percent of Link Periods with Travel Time Data = %.1lf%%") % 
				((link) ? dir * 100.0 / link : 0.0) % FINISH);
	Print (1, String ("Percent of Time Periods with Performance Data = %.1lf%%") % 
				((start) ? end * 100.0 / start : 0.0) % FINISH);

}

//---------------------------------------------------------
//	Initialize_Performance
//---------------------------------------------------------

void Data_Service::Initialize_Performance (Performance_File &file, Perf_Period_Array &perf_period_array)
{
	Required_File_Check (file, LINK);
	
	if (System_Data_Reserve (PERFORMANCE) == 0) return;

	if (perf_period_array.size () == 0) {
		if (Sum_Flow_Flag ()) {
			perf_period_array.Initialize (&sum_periods);
		} else {
			perf_period_array.Initialize (&time_periods);
		}
	}
}

//---------------------------------------------------------
//	Get_Performance_Data
//---------------------------------------------------------

bool Data_Service::Get_Performance_Data (Performance_File &file, Performance_Data &perf_rec)
{
	int dir_index, link, dir;
	static int node; 
	static Dtime period;

	Link_Data *link_ptr;

	//---- check/convert the link number and direction ----
		
	link = file.Link ();
	dir = file.Dir ();

	link_ptr = Set_Link_Direction (file, link, dir, (file.Version () <= 40));

	if (link_ptr == 0) return (false);

	if (dir) {
		dir_index = link_ptr->BA_Dir ();
		node = link_ptr->Anode ();
	} else {
		dir_index = link_ptr->AB_Dir ();
		node = link_ptr->Bnode ();
	}
	if (dir_index < 0) {
		Warning (String ("%s %d Link %d Direction %s was Not Found") % file.File_ID () % Progress_Count () % 
			link_ptr->Link () % ((dir) ? "BA" : "AB"));
		node = 0;
		return (false);
	}
	perf_rec.Dir_Index (dir_index);
	perf_rec.Type (file.Type ());

	//---- store the data ----

	perf_rec.Persons (file.Persons ());
	perf_rec.Volume (file.Volume ());
	perf_rec.Enter (file.Enter ());
	perf_rec.Exit (file.Exit ());

	perf_rec.End (file.End ());
	perf_rec.Flow (file.Flow ());

	if (file.Version () <= 40 && file.Dbase_Format () == VERSION3) {
		if (period == 0) {
			period = perf_rec.End ();
		}
		perf_rec.Start (perf_rec.End () - period);
		if (perf_rec.Flow () > 0) {
			perf_rec.Time (UnRound (file.Time ()) / perf_rec.Flow ());
		} else {
			perf_rec.Time (file.Time ());
		}
	} else {
		perf_rec.Start (file.Start ());
		perf_rec.Time (file.Time ());
	}
	perf_rec.Speed (file.Speed ());
	perf_rec.Time_Ratio (file.Time_Ratio ());
	perf_rec.Delay (file.Delay ());
	perf_rec.Density (file.Density ());
	perf_rec.Max_Density (file.Max_Density ());
	perf_rec.Queue (file.Queue ());
	perf_rec.Max_Queue (file.Max_Queue ());
	perf_rec.Failure (file.Failure ());
	perf_rec.Veh_Dist (file.Veh_Dist ());
	perf_rec.Veh_Time (file.Veh_Time ());
	perf_rec.Veh_Delay (file.Veh_Delay ());

	return (true);
}

//------------------------------------------------------
//	Read_Performance
//---------------------------------------------------------

void Data_Service::Read_Performance (Performance_File &file, Volume_Array &data)
{
	int i, num, start, end, count, index;
	double factor, p1, p2, share;
	bool sum_flag;

	Time_Periods *period_ptr;
	Dir_Data *dir_ptr;
	Performance_Data perf_rec;
	Dbls_Ptr vol_ptr;
	double *volume;

	//---- store the performance data ----

	Show_Message (String ("Reading %s -- Record") % file.File_Type ());
	Set_Progress ();
	
	if (data.size () == 0) {
		if (Sum_Flow_Flag ()) {
			data.Initialize (&sum_periods, (int) (dir_array.size () + Num_Lane_Use_Flows ()));
		} else {
			data.Initialize (&time_periods, (int) (dir_array.size () + Num_Lane_Use_Flows ()));
		}
	}
	count = 0;

	period_ptr = data.periods;

	while (file.Read ()) {
		Show_Progress ();

		perf_rec.Clear ();

		if (!Get_Performance_Data (file, perf_rec)) continue;

		//---- get the time period ----

		if (period_ptr->Period_Range (perf_rec.Start (), perf_rec.End (), start, end)) {

			p1 = period_ptr->Period_Offset (start, perf_rec.Start ());
			p2 = period_ptr->Period_Offset (end, perf_rec.End ());
			if (p1 == p2) continue;

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

				vol_ptr = data.Period_Ptr (i);

				dir_ptr = &dir_array [perf_rec.Dir_Index ()];

				if (perf_rec.Type () == 0 || !Lane_Use_Flows ()) {
					index = perf_rec.Dir_Index ();
					sum_flag = (perf_rec.Type () == 1);
				} else {
					index = dir_ptr->Use_Index ();
					sum_flag = false;
				}
				volume = &vol_ptr->at (index);

				if (!sum_flag && num == 0 && *volume > 0) {
					*volume += perf_rec.Enter ();
				} else {
					*volume += (perf_rec.Volume () * share);
				}
				count++;
			}
		}
	}
	End_Progress ();
	file.Close ();

	Break_Check (3);
	Print (2, String ("Number of %s Records = %d") % file.File_Type () % Progress_Count ());

	if (count && count != Progress_Count ()) {
		Print (1, String ("Number of %s Data Records = %d") % file.File_ID () % count);
	}
	Print (1, "Number of Link Direction Records = ") << count;
}
