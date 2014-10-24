//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "PlanSelect.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void PlanSelect::Execute (void)
{

	//---- read the network data ----

	Data_Service::Execute ();

	//---- set the processing queue ----

	int part, num;
	Plan_File *file = System_Plan_File ();

	num = file->Num_Parts ();

	for (part=0; part < num; part++) {
		partition_queue.Put (part);
		if (new_plan_flag) output_queue.Put (part);
	}
	file->Close ();
	file->Reset_Counters ();

	partition_queue.End_of_Queue ();
	if (new_plan_flag) output_queue.End_of_Queue ();

	//---- processing threads ---

	Num_Threads (MIN (Num_Threads (), num));

	if (Num_Threads () > 1) {
#ifdef THREADS	
		Threads threads;

		for (int i=0; i < Num_Threads (); i++) {
			threads.push_back (thread (Plan_Processing (this)));
		}
		threads.Join_All ();
		Show_Message (1);
#endif
	} else {
		Plan_Processing plan_processing (this);
		plan_processing ();
	}

	//---- combine MPI data ----

	MPI_Processing ();

	//---- select plans ----

	if (Master ()) {

		//---- print processing summary ----

		file->Print_Summary ();

		//---- select the trip records ----

		Select_Plans ();
	}

	//---- write plans or selection records ----

	if (new_plan_flag || select_parts) {

		//---- distribute MPI selection data ----

		MPI_Write ();

		//---- write the new plan file ----

		if (new_plan_flag) {
			if (num_select > 0) {

				//---- write the selected plans ---

				if (Num_Threads () > 1) {
#ifdef THREADS
					Threads threads;

					for (int i=0; i < Num_Threads (); i++) {
						threads.push_back (thread (Plan_Output (this)));
					}
					threads.Join_All ();
#endif
				} else {
					Plan_Output plan_output (this);
					plan_output ();
				}
			
			} else {

				//---- create empty files ----

				while (output_queue.Get (part)) {
					new_plan_file->Open (part);
					new_plan_file->Close ();
				}
			}
		}

		//---- MPI Close ----

		MPI_Close ();

		//---- print output summary ----

		if (new_plan_flag) new_plan_file->Print_Summary ();
	}

	//---- write the new selection file ----

	if (System_File_Flag (NEW_SELECTION) && (!select_parts || MPI_Size () < 1)) {
		Write_Selections ();
	}

	//---- end the program ----

	Exit_Stat (DONE);
}

//---------------------------------------------------------
//	Page_Header
//---------------------------------------------------------

void PlanSelect::Page_Header (void)
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
