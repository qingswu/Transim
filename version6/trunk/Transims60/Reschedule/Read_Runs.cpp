//*********************************************************
//	Read_Runs.cpp - read the run start time file
//*********************************************************

#include "Reschedule.hpp"

//---------------------------------------------------------
//	Read_Runs
//---------------------------------------------------------

void Reschedule::Read_Runs (void)
{
	int i, lvalue, count, type, num, best_id, best_count, no_match;
	String svalue;
	Dtime time;
	bool flag;

	Int_Map_Itr map_itr;
	Line_Data *line_ptr;
	Stop_Data *stop_ptr;
	Line_Stop_Itr stop_itr;
	Line_Run run_data;
	File_Itr file_itr;
	Filter_Itr filter_itr;
	Line_Filter *filter_ptr;

	if (match_dump) Print (1);

	for (file_itr = file_group.begin (); file_itr != file_group.end (); file_itr++) {

		Show_Message (String ("Reading %s -- Record") % file_itr->run_file->File_Type ());
		Set_Progress ();

		//---- clear the existing schedule records ----

		for (filter_itr = file_itr->lines.begin (); filter_itr != file_itr->lines.end (); filter_itr++) {

			map_itr = line_map.find (filter_itr->line);
			if (map_itr == line_map.end ()) continue;

			line_ptr = &line_array [map_itr->second];

			line_ptr->run_types.clear ();

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
				stop_itr->clear ();
			}
		}

		count = no_match = type = 0;

		//---- read each schedule records ----

		while (file_itr->run_file->Read ()) {
			Show_Progress ();

			//---- check the line options ----

			best_count = best_id = 0;

			for (i=0, filter_itr = file_itr->lines.begin (); filter_itr != file_itr->lines.end (); filter_itr++, i++) {

				map_itr = line_map.find (filter_itr->line);
				if (map_itr == line_map.end ()) continue;

				line_ptr = &line_array [map_itr->second];

				if (file_itr->filter >= 0) {
					lvalue = file_itr->run_file->Get_Integer (file_itr->filter);

					if (lvalue < filter_itr->low || lvalue > filter_itr->high) continue;
				}

				//---- check for complete runs ----

				flag = true;
				num = 0;

				for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
					stop_ptr = &stop_array [stop_itr->Stop ()];

					map_itr = filter_itr->stop_field.find (stop_ptr->Stop ());
					if (map_itr == filter_itr->stop_field.end ()) continue;

					svalue = file_itr->run_file->Get_String (map_itr->second); 
					if (svalue.Equals ("-")) {
						flag = false;
						break;
					} else {
						num++;
					}
				}
				if (flag && num > best_count) {
					best_id = i;
					best_count = num;
				}
			}

			if (best_count == 0) {
				no_match++;

				if (match_dump) {
					Print (1, file_itr->run_file->Record_String ());
				}
				continue;
			}

			//---- process the best route ----

			filter_ptr = &file_itr->lines [best_id];

			map_itr = line_map.find (filter_ptr->line);
			line_ptr = &line_array [map_itr->second];

			if (file_itr->type >= 0) {
				type = file_itr->run_file->Get_Integer (file_itr->type);

				map_itr = veh_type_map.find (type);
				if (map_itr == veh_type_map.end ()) {
					Warning (String ("Reschedule Route %d Vehicle Type %d was Not Found") % line_ptr->Route () % type);
				} else {
					type = map_itr->second;
				}
				line_ptr->Add_Run_Type (type);
			}
			count++;

			//---- process each stop ----

			for (stop_itr = line_ptr->begin (); stop_itr != line_ptr->end (); stop_itr++) {
				stop_ptr = &stop_array [stop_itr->Stop ()];

				map_itr = filter_ptr->stop_field.find (stop_ptr->Stop ());
				if (map_itr == filter_ptr->stop_field.end ()) continue;

				svalue = file_itr->run_file->Get_String (map_itr->second); 
				if (svalue.Equals ("-")) continue;

				if (svalue.Starts_With ("(")) {
					svalue = svalue.substr (1);
					if (svalue.Ends_With (")")) {
						svalue = svalue.substr (0, svalue.length () - 1);
					}
				}
				lvalue = svalue.Integer ();

				if (svalue.Ends_With ("P")) {
					if (lvalue < 1200) {
						lvalue += 1200;
					}
				} else if (svalue.Ends_With ("X")) {
					if (lvalue < 1200) {
						lvalue += 2400;
					} else {
						lvalue += 1200;
					}
				}
				time.Hours ((lvalue / 100) + (lvalue % 100) / 60.0);

				run_data.Schedule (time);
				stop_itr->push_back (run_data);
			}
		}
		End_Progress ();

		Print (2, String ("Number of %s Records = %d") % file_itr->run_file->File_Type () % Progress_Count ());
		Print (1, "Number of Runs Processed = ") << count;
		Print (1, "Number of Records Ignored = ") << no_match;
		if (match_dump) Print (1);
		
		file_itr->run_file->Close ();
	}
}
