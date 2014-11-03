//*********************************************************
//	Set_Partitions.cpp - set file partitions
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Set_Partitions
//---------------------------------------------------------

void Router::Set_Partitions (void)
{
	if (trip_set_flag) {
		num_file_sets = trip_file->Num_Parts ();
	}
	if (new_set_flag) {
		int num;

		if (select_flag) {
			if (First_Partition () >= 0) {
				num = Num_Partitions ();
			} else {
				num = select_map.Max_Partition () + 1;
			}
			if (trip_set_flag) {
				if (num_file_sets != num) {
					Error (String ("Trip and Selection Partitions are Not Equal (%d vs %d)") %
						num_file_sets % num);
				}
			} else {
				num_file_sets = num;
			}
			if (plan_flag) {
				num = plan_file->Num_Parts ();

				if (num_file_sets <= 1) {
					num_file_sets = num;
				} else if (num_file_sets != num) {
					Error (String ("Plan and Selection Partitions are Not Equal (%d vs %d)") %
						num % num_file_sets);
				}
			}
		} else if (plan_flag) {
			num = plan_file->Num_Parts ();

			if (trip_set_flag) {
				if (num_file_sets != num) {
					Error (String ("Trip and Plan Partitions are Not Equal (%d vs %d)") %
						num_file_sets % num);
				}
			} else {
				num_file_sets = num;
			}
		}
	}
	if (First_Partition () >= 0 && num_file_sets > Num_Partitions ()) num_file_sets = Num_Partitions ();

	if (num_file_sets == 1) {
		if (MPI_Size () == 1) Warning ("Only One File Partition will be Created");
	} else {
		if (MPI_Size () == 1) {
			Print (1);
			Write (1, "Number of File Partitions = ") << num_file_sets;
			Show_Message (1);
		}
		if (num_file_sets > Num_Partitions ()) {
			First_Partition (0);
			Last_Partition (num_file_sets - 1);
		}
	}
	if (trip_set_flag) {
		trip_file_set.Initialize (trip_file, num_file_sets);

		if (trip_memory_flag) {
			trip_arrays.Initialize (num_file_sets);
		}
	}
	if (new_set_flag) {
		if (plan_flag) {
			plan_file_set.Initialize (plan_file, num_file_sets);
		}
		if (plan_set_flag) {
			plan_ptr_arrays.resize (num_file_sets);

			if (plan_memory_flag) {
				plan_ptr_set.Initialize (num_file_sets);
			}
		}
		if (new_plan_flag) {
			new_file_set.Initialize (new_plan_file, num_file_sets);

			if (plan_memory_flag) {
				new_ptr_set.Initialize (num_file_sets);
			}
		}
	}
	if (problem_set_flag) {
		problem_set.Initialize (problem_file, num_file_sets);
	}
	thread_flag = part_processor.Initialize (this);

	if (trip_gap_map_flag && !Memory_Flag ()) {
		Trip_Gap_Map_Parts (Num_Partitions ());
	}
}
