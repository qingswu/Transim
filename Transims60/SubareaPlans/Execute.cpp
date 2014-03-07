//*********************************************************
//	Execute.cpp - main execution procedure
//*********************************************************

#include "SubareaPlans.hpp"

//---------------------------------------------------------
//	Execute
//---------------------------------------------------------

void SubareaPlans::Execute (void)
{

	//---- read the network ----

	Data_Service::Execute ();

	//---- identify subarea boundary links ----

	Sublink_Data ();

	//---- set the processing queue ----

	int part, num;

	num = plan_file->Num_Parts ();

	for (part=0; part < num; part++) {
		partition_queue.Put (part);
		output_queue.Put (part);
	}
	plan_file->Close ();
	plan_file->Reset_Counters ();

	partition_queue.End_of_Queue ();
	output_queue.End_of_Queue ();

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

	//---- write the summary report ----

	if (Report_Flag (TRIP_REPORT)) {
		Trip_Report ();
	}

	//---- write summary statistics ----

	plan_file->Print_Summary ();
	new_plan_file->Print_Summary ();

	Exit_Stat (DONE);
}
