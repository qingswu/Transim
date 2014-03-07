//*********************************************************
//	Write_Stop_Route - write the stop route file
//*********************************************************

#include "RiderSum.hpp"

#include "Time_Index.hpp"

//---------------------------------------------------------
//	Write_Stop_Details
//---------------------------------------------------------

void RiderSum::Write_Stop_Details (void)
{
	int stop, s, run, tot_riders, tot_runs, num, period, num_periods, capacity, tot_capacity;
	int tot_board, tot_alight;
	double cap_fac;
	Dtime low, high;
	string time_range, stop_name, route_name;

	Int_Map_Itr map_itr, line_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr;
	Line_Stop *line_stop;
	Line_Run_Itr run_itr;
	Line_Run *line_run;
	Veh_Type_Data *veh_type_ptr;

	Time_Index time_index;
	Time_Map time_sort;
	Time_Map_Itr time_itr;

	fstream &file = stop_detail_file.File ();
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Show_Message (String ("Writing % -- Record") % stop_detail_file.File_Type ());
	Set_Progress ();

	file << "Stop\tStopName\tTimePeriod\tRoute\tRun\tAlight\tBoard\tRiders\tCapacity\tCapFac\tRouteName\tNotes\n";

	//---- process each stop ----

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		stop = map_itr->first;

		if (select_stops && !stop_range.In_Range (stop)) continue;

		stop = map_itr->second;
		stop_ptr = &stop_array [stop];
		
		if (Notes_Name_Flag ()) {
			if (!stop_ptr->Name ().empty ()) {
				stop_name = stop_ptr->Name ();
			} else {
				stop_name = "";
			}
			if (!stop_ptr->Notes ().empty ()) {
				stop_name += " -- ";
				stop_name += stop_ptr->Notes ();
			}
		} else {
			stop_name = "";
		}

		//---- process each time period ----

		for (period = 0; period < num_periods; period++) {
			sum_periods.Period_Range (period, low, high);

			tot_runs = tot_riders = tot_board = tot_alight = tot_capacity = num = 0;
			time_range = sum_periods.Range_Format (period);

			time_sort.clear ();

			//--- search each line for the stop and runs ----

			for (line_itr = line_map.begin (); line_itr != line_map.end (); line_itr++) {
				if (select_routes && !route_range.In_Range (line_itr->first)) continue;

				line_ptr = &line_array [line_itr->second];

				if (select_modes && !select_mode [line_ptr->Mode ()]) continue;

				for (s=0, stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++, s++) {
					if (stop_itr->Stop () != stop) continue;

					for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
						if (run_itr->Schedule () >= low && run_itr->Schedule () < high) {
							Show_Progress ();

							time_index.Start (run_itr->Schedule ());
							time_index.Household (line_itr->second);
							time_index.Person (run);

							time_sort.insert (Time_Map_Data (time_index, s));
							num++;
						}
					}
					break;
				}
			}
			if (num > 0) {
				for (time_itr = time_sort.begin (); time_itr != time_sort.end (); time_itr++) {
					time_index = time_itr->first;
					line_ptr = &line_array [time_index.Household ()];
					line_stop = &line_ptr->at (time_itr->second);

					run = time_index.Person ();
					line_run = &line_stop->at (run);

					if (line_ptr->run_types.size () > 0) {
						veh_type_ptr = &veh_type_array [line_ptr->Run_Type (run)];
					} else {
						veh_type_ptr = &veh_type_array [line_ptr->Type ()];
					}
					capacity = veh_type_ptr->Capacity ();
					if (capacity == 0) capacity = 1;

					cap_fac = (double) run_itr->Load () / capacity;

					file << stop_ptr->Stop () << "\t" << stop_name << "\t" << line_run->Schedule ().Time_String (HOUR_CLOCK) << "\t" 
						<< line_ptr->Route () << "\t" << run << "\t" << line_run->Alight () << "\t" << line_run->Board () << "\t" 
						<< line_run->Load () << "\t" << capacity << "\t" <<  (String ("%6.2lf") % cap_fac) << "\t";
					
					if (!line_ptr->Name ().empty ()) {
						file << line_ptr->Name () << "\t";
					}
					if (Notes_Name_Flag ()) {
						if (!line_ptr->Notes ().empty ()) {
							file << line_ptr->Notes ();
						}
					}
					file << "\n";

					tot_runs++;
					tot_riders += line_run->Load ();
					tot_capacity += capacity;
					tot_board += line_run->Board ();
					tot_alight += line_run->Alight ();
				}
				cap_fac = (double) tot_riders / tot_capacity;

				file << stop_ptr->Stop () << "\t" << stop_name << "\t" << time_range << "\tTotal\t" 
					<< tot_runs << "\t" << tot_alight << "\t" << tot_board << "\t" << tot_riders << "\t" << 
					tot_capacity << "\t" << (String ("%6.2lf") % cap_fac) << "\t\n";
			}
		}
	}
	End_Progress ();
	stop_detail_file.Close ();
}
