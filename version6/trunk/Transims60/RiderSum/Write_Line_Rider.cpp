//*********************************************************
//	Write_Line_Rider - write the line rider file
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Write_Line_Rider
//---------------------------------------------------------

void RiderSum::Write_Line_Rider (void)
{
	int riders, board, alight, run, runs, num, length, tot_len, period, num_periods, total, capacity;
	double factor, sum_time, tot_time, time, capfac;
	Dtime low, high;

	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr, next_itr;
	Line_Run_Itr run_itr;
	Veh_Type_Data *veh_type_ptr, *run_type_ptr;

	fstream &file = line_rider_file.File ();
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Show_Message (String ("Writing % -- Record") % line_rider_file.File_Type ());
	Set_Progress ();

	file << "Route\tMode\tType\tPeriod\tStops\tRouteName\tNotes\n";
	file << "Stop\tLength\tTTime\tAlight\tBoard\tRiders\tRuns\tLoadFac\tCapacity\tCapFac\tStopName\n";

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
			
			file << line_ptr->Route () << "\t" << Transit_Code ((Transit_Type) line_ptr->Mode ()) << "\t" <<
					veh_type_ptr->Type () << "\t" << sum_periods.Range_Format (period) << "\t" << 
					line_ptr->size () << "\t";
			
			if (!line_ptr->Name ().empty ()) {
				file << line_ptr->Name () << "\t";
			}
			if (Notes_Name_Flag ()) {
				if (!line_ptr->Notes ().empty ()) {
					file << line_ptr->Notes ();
				}
			}
			file << "\n";

			time = tot_time = 0.0;
			total = length = tot_len = 0;

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
						sum_time += time;
						num++;
					}
				}
				if (runs == 0) continue;
				if (capacity == 0) capacity = runs;

				factor = DTOI (riders * 10.0 / runs) / 10.0;
				capfac = DTOI (riders * 10.0 / capacity) / 10.0;

				if (next_itr == line_ptr->end ()) runs = 0;

				if (num > 0) {
					time = sum_time / num;
				} else {
					time = 0;
				}
				file << stop_ptr->Stop () << "\t" << UnRound (length) << "\t" << time << "\t" <<
					alight << "\t" << board << "\t" << riders << "\t" << runs << "\t" << factor << 
					"\t" << capacity << "\t" << capfac << "\t";

				if (Notes_Name_Flag ()) {
					if (!stop_ptr->Name ().empty ()) {
						file << stop_ptr->Name ();
					}
					if (!stop_ptr->Notes ().empty ()) {
						file << " -- " << stop_ptr->Notes ();
					}
				}
				file << "\n";
			}
		}
	}
	End_Progress ();
	line_rider_file.Close ();
}
