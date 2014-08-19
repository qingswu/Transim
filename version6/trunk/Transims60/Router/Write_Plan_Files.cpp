//*********************************************************
//	Write_Plan_Files.cpp - sort and write plan files
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Write_Plan_Files
//---------------------------------------------------------

void Router::Write_Plan_Files (void)
{
	int i, index;
	bool first;

	Plan_Itr plan_itr;
	Plan_Ptr plan_ptr;
	Time_Index time_index;
	Time_Map_Stat time_stat;
	Time_Map_Itr time_itr;
	Trip_Map_Itr trip_itr;

	if (time_sort_flag) {
		Show_Message ("Sorting Plans by Time -- Record");
		Set_Progress ();

		for (i=0, plan_itr = plan_array.begin (); plan_itr != plan_array.end (); plan_itr++, i++) {
			if (plan_itr->Household () == 0) continue;
			Show_Progress ();

			plan_itr->Get_Index (time_index);

			time_stat = plan_time_map.insert (Time_Map_Data (time_index, i));

			if (!time_stat.second) {
				Warning (String ("Duplicate Plan Index = %s-%d-%d") % 
					time_index.Start ().Time_String () % 
					time_index.Household () % time_index.Person ());
			}
		}
		End_Progress ();
	}

	if (num_file_sets > 1) {
		Show_Message (String ("Writing %ss -- Record") % new_plan_file->File_Type ());			
	} else {
		Show_Message (String ("Writing %s -- Record") % new_plan_file->File_Type ());
	}
	Set_Progress ();
	Reset_Problems ();

	for (first = true; ; first = false) {
		if (time_sort_flag) {
			if (first) {
				time_itr = plan_time_map.begin ();
			} else {
				time_itr++;
			}
			if (time_itr == plan_time_map.end ()) break;
			index = time_itr->second;
		} else {
			if (first) {
				trip_itr = plan_trip_map.begin ();
			} else {
				trip_itr++;
			}
			if (trip_itr == plan_trip_map.end ()) break;
			index = trip_itr->second;
		}
		Show_Progress ();

		plan_ptr = &plan_array [index];
		if (plan_ptr == 0) continue;

		plan_ptr->External_IDs ();

		if (plan_ptr->Problem () == 0 && plan_ptr->size () > 0) {
			if (new_set_flag) {
				i = plan_ptr->Partition ();
				new_file_set [i]->Write_Plan (*plan_ptr);
			} else {
				new_plan_file->Write_Plan (*plan_ptr);
			}
		} else {
			Set_Problem ((Problem_Type) plan_ptr->Problem ());

			if (problem_flag) {
				if (problem_set_flag) {
					i = plan_ptr->Partition ();
					Write_Problem (problem_set [i], plan_ptr);
				} else {
					Write_Problem (problem_file, plan_ptr);
				}
			}
		}
	}
	End_Progress ();
}
