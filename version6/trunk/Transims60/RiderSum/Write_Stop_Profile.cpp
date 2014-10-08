//*********************************************************
//	Write_Stop_Profile - write the stop profile
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Write_Stop_Profile
//---------------------------------------------------------

void RiderSum::Write_Stop_Profile (void)
{
	int n, stop, run, period, num_periods, type;
	double load;
	String group_name;
	bool type_flag, group_flag;
	
	Int_Set *group;
	Int_Set_Itr itr;
	Int_Map_Itr map_itr, line_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run_Itr run_itr;
	Veh_Type_Data *line_type_ptr, *run_type_ptr;
	Integers riders, cars;

	fstream &file = stop_profile.File ();
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Show_Message (String ("Writing % -- Record") % stop_profile.File_Type ());
	Set_Progress ();

	file << "\t\t\t";

	for (period = 0; period < num_periods; period++) {
		file << "\t" << sum_periods.Range_Format (period) << "\t\t";
	}
	file << "\nStop\tGroup\tSegment\tDirection";

	for (period = 0; period < num_periods; period++) {
		file << "\tRiders\tCars\tLoad";
	}

	//---- process each stop ----

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		stop = map_itr->first;

		if (select_stops && !stop_range.In_Range (stop)) continue;

		stop = map_itr->second;
		stop_ptr = &stop_array [stop];

		//---- process each line group ----
	
		for (n = line_equiv.First_Group (); n > 0; n = line_equiv.Next_Group ()) {

			group = line_equiv.Group_List (n);
			if (group == 0) continue;

			group_name = line_equiv.Group_Label (n);
		
			group_flag = false;
			riders.assign (num_periods, 0);
			cars.assign (num_periods, 0);

			//---- process each line in the line group ----

			for (itr = group->begin (); itr != group->end (); itr++) {

				line_itr = line_map.find (*itr);
				if (line_itr == line_map.end ()) continue;

				Show_Progress ();

				line_ptr = &line_array [line_itr->second];

				line_type_ptr = &veh_type_array [line_ptr->Type ()];
				type_flag = ((int) line_ptr->run_types.size () > 0);

				for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
					if (stop_itr->Stop () != stop) continue;

					for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
						period = sum_periods.Period (run_itr->Schedule ());

						if (period >= 0) {
							group_flag = true;
							riders [period] += run_itr->Load ();

							if (type_flag) {
								type = line_ptr->Run_Type (run);
								run_type_ptr = &veh_type_array [type];
								cars [period] += run_type_ptr->Capacity ();
							} else {
								cars [period] += line_type_ptr->Capacity ();
							}
						}
					}
					break;
				}
			}

			if (!group_flag) continue;

			file << "\n" << stop_ptr->Stop () << "\t" << group_name;

			if (Notes_Name_Flag ()) {
				if (!stop_ptr->Name ().empty ()) {
					file << "\t" << stop_ptr->Name ();
				}
				if (!stop_ptr->Notes ().empty ()) {
					file << "\t" << stop_ptr->Notes ();
				}
			}

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
