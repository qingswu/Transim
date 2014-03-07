//*********************************************************
//	Read_Plans.cpp - read travel plans
//*********************************************************

#include "Sim_Plan_Step.hpp"
#include "Simulator_Service.hpp"

//---------------------------------------------------------
//	Read_Plans
//---------------------------------------------------------

bool Sim_Plan_Step::Read_Plans (void)
{
	int i, j, num;
	Dtime time = 0;

	Plan_Data *plan_ptr;
	Time_Index *time_ptr;

#ifdef THREADS
	if (num_threads > 1) {
		plan_queue.Start_Work ();
	}
#endif

	while (stat) {
		if (first_num < 0) {
			stat = false;
			break;
		}
		plan_ptr = plan_set [first_num];
		time_ptr = time_set [first_num];

		if (time_ptr->Household () == MAX_INTEGER) {
			stat = false;
			break;
		}
		if (first) sim->Show_Progress ();

		if (plan_ptr->Depart () > sim->param.end_time_step) {
			stat = false;
			break;
		}
		if (plan_ptr->Depart () < time) {
			sim->Error ("Plans are Not Time Sorted");
			return (false);
		}

		//---- end of the current time step ----

		if (plan_ptr->Depart () > sim->time_step) break;
		time = plan_ptr->Depart ();

		//---- convert to simulator plan format ----

#ifdef THREADS
		if (num_threads > 1) {
			Plan_Data *work_plan = new Plan_Data ();
			*work_plan = *plan_ptr;
			plan_queue.Put_Work (work_plan);
		} else {
			Sim_Plan_Result ((*sim_plan_process)->Plan_Processing (plan_ptr));
		}
#else
		Sim_Plan_Result (sim_plan_process.Plan_Processing (plan_ptr));
#endif

		//---- get the next record for the current partition ----

		num = first_num;
		first_num = next [num];

		if (file_set [num]->Read_Plan (*plan_ptr)) {
			plan_ptr->Get_Time_Index (*time_ptr);

			//---- update the record order ---

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
#ifdef THREADS
	if (num_threads > 1) {
		plan_queue.Complete_Work ();
	}
#endif

	//---- aggregate the plan file statistics ----

	if (num_files > 0) {
		Plan_File *plan_file = (Plan_File *) sim->System_File_Handle (PLAN);

		plan_file->Reset_Counters ();

		for (i=0; i < num_files; i++) {
			plan_file->Add_Counters (file_set [i]);
		}
	}
	first = false;
	return (stat);
}
