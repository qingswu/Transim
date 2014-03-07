//*********************************************************
//	Save_Plans.cpp -  write the path building results
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Save_Plans
//---------------------------------------------------------

bool Router::Save_Plans (Plan_Ptr_Array *array_ptr, int part)
{
	if (array_ptr == 0) return (false);

	Plan_Ptr_Itr itr;
	Plan_Data *plan_ptr;

	for (itr = array_ptr->begin (); itr != array_ptr->end (); itr++) {
		plan_ptr = *itr;
		if (!plan_ptr) continue;

		total_records++;

		if (plan_ptr->Problem () == 0) {
			if (plan_set_flag) {
				int i = plan_ptr->Partition ();
				new_plan_set [i]->Write_Plan (*plan_ptr);
			} else if (new_plan_flag) {
				new_plan_file->Write_Plan (*plan_ptr);
			}
			if (plan_ptr->Method () == REROUTE_PATH) {
				num_reroute++;
			} else if (plan_ptr->Method () == RESKIM_PATH) {
				num_reskim++;
			}
			if (trip_gap_map_flag) {
				Gap_Map_Data gap_map_data;
				Trip_Gap_Map_Stat map_stat;
				Trip_Gap_Map *trip_gap_map_ptr;

				if (plan_ptr->Constraint () == END_TIME) {
					gap_map_data.time = plan_ptr->End ();
				} else {
					gap_map_data.time = plan_ptr->Start ();
				}
				gap_map_data.current = (int) plan_ptr->Impedance ();
				gap_map_data.previous = 0;

				if (part_processor.Thread_Flag ()) {
					trip_gap_map_ptr = trip_gap_map_array [plan_ptr->Partition ()];
				} else {
					trip_gap_map_ptr = &trip_gap_map;
				}
				map_stat = trip_gap_map_ptr->insert (Trip_Gap_Map_Data (plan_ptr->Get_Trip_Index (), gap_map_data));

				if (!map_stat.second) {
					map_stat.first->second.current = (int) plan_ptr->Impedance ();
				}
			}
			if (rider_flag) {
				part_processor.Sum_Ridership (*plan_ptr, part);
			}
			if (link_person_flag) {
				part_processor.Sum_Persons (*plan_ptr, part);
			}
			if (link_vehicle_flag) {
				part_processor.Sum_Vehicles (*plan_ptr, part);
			}
		} else {
			Set_Problem ((Problem_Type) plan_ptr->Problem ());

			if (problem_flag) {
				if (problem_set_flag) {
					Write_Problem (problem_set [plan_ptr->Partition ()], plan_ptr);
				} else if (thread_flag) {
					problem_file->Lock ();
					Write_Problem (problem_file, plan_ptr);
					problem_file->UnLock ();
				} else {
					Write_Problem (problem_file, plan_ptr);
				}
			}
		}
		delete plan_ptr;
	}
	delete array_ptr;
	return (true);
}

//---------------------------------------------------------
//	Write_Problem
//---------------------------------------------------------

void Router::Write_Problem (Problem_File *file, Plan_Data *plan_ptr)
{
	file->Problem (plan_ptr->Problem ());
	file->Household (plan_ptr->Household ());
	file->Person (plan_ptr->Person ());
	file->Tour (plan_ptr->Tour ());
	file->Trip (plan_ptr->Trip ());
	file->Start (plan_ptr->Start ());
	file->End (plan_ptr->End ());
	file->Duration (plan_ptr->Duration ());
	file->Origin (plan_ptr->Origin ());
	file->Destination (plan_ptr->Destination ());
	file->Purpose (plan_ptr->Purpose ());
	file->Mode (plan_ptr->Mode ());
	file->Constraint (plan_ptr->Constraint ());
	file->Priority (plan_ptr->Priority ());
	file->Veh_Type (plan_ptr->Veh_Type ());
	file->Vehicle (plan_ptr->Vehicle ());
	file->Type (plan_ptr->Type ());
	file->Notes ((char *) Problem_Code ((Problem_Type) plan_ptr->Problem ()));

	if (!file->Write ()) {
		Warning ("Writing ") << file->File_Type ();
		problem_flag = false;
	}
	file->Add_Trip (plan_ptr->Household (), plan_ptr->Person (), plan_ptr->Tour ());
}
