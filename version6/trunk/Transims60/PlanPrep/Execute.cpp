//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanPrep.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanPrep::Execute (void)
{
	//---- read the network data ----

	Data_Service::Execute ();

	//---- set the processing queue ----

	int part, num;

	num = plan_file->Num_Parts ();

	for (part=0; part < num; part++) {
		if (part > 0 && merge_flag && !merge_file.Find_File (part)) {
			Error (String ("%s %d was Not Found") % merge_file.File_Type () % part);
		}
		partition_queue.Put (part);
	}
	plan_file->Close ();
	plan_file->Reset_Counters ();

	merge_file.Close ();
	merge_file.Reset_Counters ();

	partition_queue.End_of_Queue ();

	//---- processing threads ---

	Num_Threads (MIN (Num_Threads (), num));

	if (Num_Threads () > 1) {
#ifdef THREADS		
		Threads threads;

		for (int i=0; i < Num_Threads (); i++) {
			threads.push_back (thread (Plan_Processing (this)));
		}
		threads.Join_All ();
#endif
	} else {
		Plan_Processing plan_processing (this);
		plan_processing ();
	}

	//---- combine plans ----

	if (combine_flag) {
		Combine_Plans ();
	}

	//---- combine MPI data ----

	MPI_Processing ();

	//---- repair results ----

	if (repair_flag) {
		Write (2, "Number of Repaired Plan Legs = ") << num_repair;
		Write (1, "Number of Repaired Plans = ") << repair_plans;
	}

	//---- print processing summary ----

	plan_file->Print_Summary ();

	if (merge_flag) {
		merge_file.Print_Summary ();
	}
	if (System_File_Flag (NEW_PLAN)) {
		new_plan_file->Print_Summary ();
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PlanPrep::Page_Header (void)
{
	switch (Header_Number ()) {
		case FIRST_REPORT:		//---- First Report ----
			//First_Header ();
			break;
		case SECOND_REPORT:		//---- Second Report ----
			Print (1, "Second Report Header");
			Print (1);
			break;
		default:
			break;
	}
}
