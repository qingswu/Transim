//*********************************************************
//	Stop_Run_Report - write the stop run details
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Stop_Run_Report
//---------------------------------------------------------

void RiderSum::Stop_Run_Report (void)
{
	int stop, run, riders, tot_riders, tot_runs, num, period, num_periods, line, st;
	double factor;
	Dtime low, high, time;
	string time_range;

	Int_Map_Itr map_itr, line_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Line_Stop *line_stop_ptr;
	Line_Run *line_run_ptr;

	typedef struct { int line, stop, run; } Line_Stop_Run;
	typedef multimap <Dtime, Line_Stop_Run> Run_Map;
	typedef pair <Dtime, Line_Stop_Run>     Run_Map_Data;
	typedef Run_Map::iterator               Run_Map_Itr;

	Line_Stop_Run lsr;
	Run_Map run_map;
	Run_Map_Itr run_map_itr;

	Show_Message ("Stop Run Details -- Record");
	Set_Progress ();

	//---- print the report ----

	Header_Number (STOP_RUN);

	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	num = (int) (line_map.size () * stop_map.size ());

	if (!Break_Check (num + 5)) {
		Print (1);
		Stop_Run_Header ();
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

			tot_runs = tot_riders = 0;
			time_range = sum_periods.Range_Format (period);

			run_map.clear ();

			//--- search each line for the stop and runs ----

			for (line_itr = line_map.begin (); line_itr != line_map.end (); line_itr++) {
				if (select_routes && !route_range.In_Range (line_itr->first)) continue;

				line = line_itr->second;
				line_ptr = &line_array [line];

				if (select_modes && !select_mode [line_ptr->Mode ()]) continue;

				for (st=0, stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++, st++) {
					if (stop_itr->Stop () != stop) continue;

					for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
						if (run_itr->Schedule () >= low && run_itr->Schedule () < high) {
							lsr.line = line;
							lsr.stop = st;
							lsr.run = run;

							run_map.insert (Run_Map_Data (run_itr->Schedule (), lsr));
						}
					}
					break;
				}
			}

			for (run_map_itr = run_map.begin (); run_map_itr != run_map.end (); run_map_itr++) {
				time = run_map_itr->first;
				line_ptr = &line_array [run_map_itr->second.line];
				line_stop_ptr = &line_ptr->at (run_map_itr->second.stop);

				run = run_map_itr->second.run;
				line_run_ptr = &line_stop_ptr->at (run++);
				riders = line_run_ptr->Load ();

				Show_Progress ();
					
				Print (1, String ("%12.12s %8d %5d %8d") % time.Time_String () %
					line_ptr->Route () % run % riders);
				
				if (!line_ptr->Name ().empty ()) {
					Print (0, String ("  %s") % line_ptr->Name ());
				}
				if (Notes_Name_Flag ()) {
					if (!line_ptr->Notes ().empty ()) {
						Print (0, String (" -- %s") % line_ptr->Notes ());
					}
				}
				tot_runs++;
				tot_riders += riders;
			}
			if (tot_runs > 1) {
				Show_Progress ();
					
				factor = (double) tot_riders / tot_runs;

				Print (1, String ("%12.12s    Total %5d %8d  %.1lf") % time_range %
					tot_runs % tot_riders % factor);
			}
			Print (1);
		}
	}
	End_Progress ();

	Header_Number (0);
}

//---------------------------------------------------------
//	Stop_Run_Header
//---------------------------------------------------------

void RiderSum::Stop_Run_Header (void)
{
	Print (1, "Stop Run Details");
	Print (2, "Time/Period     Route   Run   Riders  Name/LoadFac");
	Print (1);
}

/*********************************************|***********************************************

	Stop Run Details

	Time/Period     Route   Run   Riders  Name/LoadFac 

	Stop: dd sssssssssssssssss sssssssssssssss
	ssssssssssss dddddddd ddddd dddddddd  sssssssssssssssssssss
	ssssssssssss dddddddd ddddd dddddddd  sssssssssssssssssssss
	ssssssssssss    Total ddddd dddddddd  ddd.d
	
**********************************************|***********************************************/ 
