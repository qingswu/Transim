//*********************************************************
//	Write_Stop_Lines - write the stop line profile
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Write_Stop_Lines
//---------------------------------------------------------

void RiderSum::Write_Stop_Lines (void)
{
	int stop, run, period, num_periods, type;
	double load;
	bool type_flag, flag;

	Int_Map_Itr map_itr;
	Line_Itr line_itr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Veh_Type_Data *line_type_ptr, *run_type_ptr;
	Integers riders, cars;

	fstream &file = stop_lines.File ();
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Show_Message (String ("Writing % -- Record") % stop_lines.File_Type ());
	Set_Progress ();

	file << "\t\t\t\t";

	for (period = 0; period < num_periods; period++) {
		file << "\t" << sum_periods.Range_Format (period) << "\t\t";
	}
	file << "\nStop\tName\tDirection\tRoute\tLabel";

	for (period = 0; period < num_periods; period++) {
		file << "\tRiders\tCars\tLoad";
	}

	//---- process each stop ----

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		stop = map_itr->first;

		if (select_stops && !stop_range.In_Range (stop)) continue;

		stop = map_itr->second;
		stop_ptr = &stop_array [stop];

		//---- process each line ----

		for (line_itr = line_array.begin (); line_itr != line_array.end (); line_itr++) {
			Show_Progress ();

			riders.assign (num_periods, 0);
			cars.assign (num_periods, 0);
			flag = false;

			line_type_ptr = &veh_type_array [line_itr->Type ()];
			type_flag = ((int) line_itr->run_types.size () > 0);

			for (stop_itr = line_itr->begin (); stop_itr != line_itr->end (); stop_itr++) {
				if (stop_itr->Stop () != stop) continue;

				for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
					period = sum_periods.Period (run_itr->Schedule ());

					if (period >= 0) {
						flag = true;
						riders [period] += run_itr->Load ();

						if (type_flag) {
							type = line_itr->Run_Type (run);
							run_type_ptr = &veh_type_array [type];
							cars [period] += run_type_ptr->Capacity ();
						} else {
							cars [period] += line_type_ptr->Capacity ();
						}
					}
				}
				break;
			}
			if (!flag) continue;

			file << "\n" << stop_ptr->Stop ();

			if (Notes_Name_Flag ()) {
				file << "\t";
				if (!stop_ptr->Name ().empty ()) {
					file << stop_ptr->Name ();
				}
				file << "\t";
				if (!stop_ptr->Notes ().empty ()) {
					file << stop_ptr->Notes ();
				}
			} else {
				file <<"\t\t";
			}

			file << "\t" << line_itr->Route () << "\t" << line_itr->Name ();

			//---- process each time period ----

			for (period = 0; period < num_periods; period++) {
				if (cars [period] > 0) {
					load = (double) riders [period] / cars [period];
				} else {
					load = 0.0;
				}
				file << "\t" << riders [period] << "\t" << cars [period] << "\t" << (String ("%.1lf") % load);
			}
		}
	}
	End_Progress ();
	stop_profile.Close ();
}
