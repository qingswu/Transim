//*********************************************************
//	Write_Stop_Route - write the stop route file
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Write_Stop_Route
//---------------------------------------------------------

void RiderSum::Write_Stop_Route (void)
{
	int stop, riders, run, runs, tot_riders, tot_runs, num, period, num_periods, type, capacity, tot_capacity;
	double factor, cap_fac;
	Dtime low, high;
	string time_range, stop_name, route_name;
	bool type_flag;

	Int_Map_Itr map_itr, line_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Veh_Type_Data *line_type_ptr, *run_type_ptr;

	fstream &file = stop_route_file.File ();
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Show_Message (String ("Writing % -- Record") % stop_route_file.File_Type ());
	Set_Progress ();

	file << "Stop\tStopName\tTimePeriod\tRoute\tRuns\tCapacity\tRiders\tLoadFac\tCapFac\tRouteName\tNotes\n";

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

					file << stop_ptr->Stop () << "\t" << stop_name << "\t" << time_range << "\t" 
						<< line_ptr->Route () << "\t" << runs << "\t" << capacity << "\t" << riders << "\t" <<
						(String ("%8.1lf") % factor) << "\t" << (String ("%6.2lf") % cap_fac);
					
					if (!line_ptr->Name ().empty ()) {
						file << "\t" << line_ptr->Name () << "\t";
					}
					if (Notes_Name_Flag ()) {
						file << line_ptr->Notes ();
					}
					file << "\n";
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

				file << stop_ptr->Stop () << "\t" << stop_name << "\t" << time_range << "\tTotal\t" 
					<< tot_runs << "\t" << tot_capacity << "\t" << tot_riders << "\t" <<
					(String ("%8.1lf") % factor) << "\t" << (String ("%6.2lf") % cap_fac) << "\t\n";
			}
		}
	}
	End_Progress ();
	stop_route_file.Close ();
}
