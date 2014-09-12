//*********************************************************
//	First_Plan.cpp - initialize partitioned file processing
//*********************************************************

#include "Sim_Plan_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	First_Plan
//---------------------------------------------------------

bool Sim_Plan_Step::First_Plan (void)
{
	first = true;
	stat = false;

	if (sim->System_File_Flag (PLAN)) {
		Plan_File *plan_file = (Plan_File *) sim->System_File_Handle (PLAN);

		if (plan_file) {
			int i, j, num;
			Plan_Data *plan_ptr;
			Time_Index *time_ptr;
	
			num_files = plan_file->Num_Parts ();
			first_num = -1;

			file_set.Initialize (plan_file, num_files);
			plan_set.Initialize (num_files);
			time_set.Initialize (num_files);
			next.assign (num_files, -1);

			//---- initialize the first index for each partition -----

			for (num=0; num < num_files; num++) {
				plan_ptr = plan_set [num];
				time_ptr = time_set [num];

				if (file_set [num]->Read_Plan (*plan_ptr)) {
					plan_ptr->Get_Index (*time_ptr);
					stat = true;

					//---- sort the partition numbers by time ----

					if (first_num < 0) {
						first_num = num;
					} else {
						for (i=j=first_num; i >= 0; i = next [j = i]) {
							if (*time_ptr < *time_set [i]) {
								if (i == first_num) {
									next [num] = first_num;
									first_num = num;
								} else {
									next [j] = num;
									next [num] = i;
								}
								break;
							}
							if (next [i] < 0) {
								next [i] = num;
								next [num] = -1;
								break;
							}
						}
					}
				} else {
					time_ptr->Set (MAX_INTEGER, 0, 0);
				}
			}
		}
	}
	return (stat);
}
