//*********************************************************
//	Write_Stop_VehTypes - write the stop vehtype file
//*********************************************************

#include "RiderSum.hpp"

//---------------------------------------------------------
//	Write_Stop_VehTypes
//---------------------------------------------------------

void RiderSum::Write_Stop_VehTypes (void)
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
	Veh_Type_Itr type_itr;
	Integers riders, cars;

	fstream &file = stop_vehtype_file.File ();
	
	num_periods = sum_periods.Num_Periods ();
	if (num_periods == 0) num_periods = 1;

	Show_Message (String ("Writing % -- Record") % stop_vehtype_file.File_Type ());
	Set_Progress ();

	file << "\t\t\t";

	for (period = 0; period < num_periods; period++) {
		file << "\t" << sum_periods.Range_Format (period) << "\t\t";
	}
	file << "\nStop\tGroup\tSegment\tDirection\tVehType";

	for (period = 0; period < num_periods; period++) {
		file << "\tRiders\tCars\tLoad";
	}

	//---- process each stop ----

	for (map_itr = stop_map.begin (); map_itr != stop_map.end (); map_itr++) {
		stop = map_itr->first;

		if (select_stops && !stop_range.In_Range (stop)) continue;

		stop = map_itr->second;
		stop_ptr = &stop_array [stop];

		//---- process each vehicle type ----

		for (type=0, type_itr = veh_type_array.begin (); type_itr != veh_type_array.end (); type_itr++, type++) {

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

					type_flag = ((int) line_ptr->run_types.size () > 0);

					if (!type_flag && line_ptr->Type () != type) continue;

					for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
						if (stop_itr->Stop () != stop) continue;

						for (run=0, run_itr = stop_itr->begin (); run_itr != stop_itr->end (); run_itr++, run++) {
							period = sum_periods.Period (run_itr->Schedule ());

							if (period >= 0) {
								if (type_flag && line_ptr->Run_Type (run) != type) continue;

								group_flag = true;
								riders [period] += run_itr->Load ();
								cars [period] += type_itr->Capacity ();
							}
						}
						break;
					}
				}

				if (!group_flag) continue;

				file << "\n" << stop_ptr->Stop () << "\t" << group_name;

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

				file << "\t" << type_itr->Type ();

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
	}
	End_Progress ();
	stop_vehtype_file.Close ();
}
