//*********************************************************
//	Stop_Route_Report - write the stop route summary
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Stop_Route_Report
//---------------------------------------------------------

void RiderSum::Stop_Route_Report (void)
{
	int stop, riders, run, runs, tot_riders, tot_runs, num, period, num_periods, type, capacity, tot_capacity;
	double factor, cap_fac;
	Dtime low, high;
	string time_range;
	bool type_flag;

	Int_Map_Itr map_itr, line_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Veh_Type_Data *line_type_ptr, *run_type_ptr;

	Show_Message ("Stop Route Summary -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (STOP_ROUTE);

	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	num = (int) (line_map.size () * stop_map.size ());

	if (!Break_Check (num + 5)) {
		Print (1);
		Stop_Route_Header ();
	}

	//---- process each stop ----

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		stop = map_itr->first;

		if (select_stops && !stop_range.In_Range (stop)) continue;

		stop = map_itr->second;
		stop_ptr = &stop_array [stop];

		Print (1, "Stop: ") << stop_ptr->Stop ();
		
		if (Notes_Name_Flag ()) {
			if (!stop_ptr->Name ().empty ()) {
				Print (0, String ("  %s") % stop_ptr->Name ());
			}
			if (!stop_ptr->Notes ().empty ()) {
				Print (0, " -- ") << stop_ptr->Notes ();
			}
		}

		//---- process each time period ----

		for (period = 0; period < num_periods; period++) {
			sum_periods.Period_Range (period, low, high);

			tot_runs = tot_riders = tot_capacity = num = 0;
			time_range = sum_periods.Range_Format (period);

			//--- search each line for the stop and runs ----

			for (line_itr = line_map.begin (); line_itr != line_map.end (); line_itr++) {
				if (select_routes && !route_range.In_Range (line_itr->first)) continue;

				line_ptr = &line_array [line_itr->second];

				if (select_modes && !select_mode [line_ptr->Mode ()]) continue;
				
				line_type_ptr = &veh_type_array [line_ptr->Type ()];
				type_flag = ((int) line_ptr->run_types.size () > 0);

				runs = riders = capacity = 0;

				for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
					if (stop_itr->Stop () != stop) continue;

					for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
						if (run_itr->Schedule () >= low && run_itr->Schedule () < high) {
							riders += run_itr->Load ();
							runs++;

							if (type_flag) {
								type = line_ptr->Run_Type (run);
								run_type_ptr = &veh_type_array [type];
								capacity += run_type_ptr->Capacity ();
							} else {
								capacity += line_type_ptr->Capacity ();
							}
						}
					}
					if (runs == 0) break;
					Show_Progress ();

					if (capacity == 0) capacity = runs;

					factor = (double) riders / runs;
					cap_fac = (double) riders / capacity;

					Print (1, String ("%12.12s %8d %5d %7d %8d %8.1lf %8.1lf") % time_range %
						line_ptr->Route () % runs % capacity % riders % factor % cap_fac);
					
					if (!line_ptr->Name ().empty ()) {
						Print (0, String ("  %s") % line_ptr->Name ());
					}
					if (Notes_Name_Flag ()) {
						if (!line_ptr->Notes ().empty ()) {
							Print (0, String (" -- %s") % line_ptr->Notes ());
						}
					}

					tot_runs += runs;
					tot_riders += riders;
					tot_capacity += capacity;
					num++;
					break;
				}
			}
			if (num > 1) {
				Show_Progress ();
					
				factor = (double) tot_riders / tot_runs;
				cap_fac = (double) tot_riders / tot_capacity;

				Print (1, String ("%12.12s    Total %5d %7d %8d %8.1lf %8.1lf") % time_range %
					tot_runs % tot_capacity % tot_riders % factor % cap_fac);
			}
			Print (1);
		}
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Route_Header
//---------------------------------------------------------

void RiderSum::Stop_Route_Header (void)
{
	Print (1, "Stop Route Summary");
	Print (2, "Time Period     Route  Runs Capacity  Riders  LoadFac   CapFac  Name");
	Print (1);
}

/*********************************************|***********************************************

	Stop Route Summary

	Time Period     Route  Runs Capacity  Riders  LoadFac   CapFac  Name 

	Stop: dd sssssssssssssssss sssssssssssssss
	ssssssssssss dddddddd ddddd ddddddd dddddddd dddddd.d dddddd.d  sssssssssssssssssssss
	ssssssssssss    Total ddddd ddddddd dddddddd dddddd.d dddddd.d
	
**********************************************|***********************************************/ 
